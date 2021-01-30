#include "TextEditingForm.h"
#include "GlyphFactory.h"
#include "Glyph.h"
#include "CharacterMetrics.h"
#include "Font.h"
#include "DrawingVisitor.h"
#include "CaretController.h"
#include "ScrollController.h"
#include "Scroll.h"
#include "Selection.h"
#include "CommandFactory.h"
#include "Commands.h"
#include "VScrollActionFactory.h"
#include "VScrollActions.h"
#include "HScrollActionFactory.h"
#include "HScrollActions.h"
#include "KeyActionFactory.h"
#include "KeyActions.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(TextEditingForm, CWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_CHAR()
	ON_MESSAGE(WM_IME_COMPOSITION, OnImeComposition)
	ON_MESSAGE(WM_IME_CHAR, OnImeChar)
	ON_MESSAGE(WM_IME_STARTCOMPOSITION, OnImeStartComposition)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND_RANGE(IDC_EDIT_WRITE, IDC_SELECTMOVE_CTRLEND, OnCommandRange)
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

TextEditingForm::TextEditingForm() {
	this->note = NULL;
	this->current = NULL;
	this->font = NULL;
	this->characterMetrics = NULL;
	this->caretController = NULL;
	this->scrollController = NULL;
	this->selection = NULL;

	this->isComposing = FALSE;
	this->currentCharacter = '\0';
	this->currentBuffer[0] = '\0';
	this->currentBuffer[1] = '\0';
}

int TextEditingForm::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	CWnd::OnCreate(lpCreateStruct);

	GlyphFactory glyphFactory;
	this->note = glyphFactory.Make("");
	this->current = glyphFactory.Make("\r\n");
	this->note->Add(this->current);

	this->font = new Font(this);

	this->characterMetrics = new CharacterMetrics(this, this->font);

	Long index = this->note->Move(0);
	this->current = this->note->GetAt(index);
	this->current->First();
	this->Notify();

	return 0;
}

void TextEditingForm::OnClose() {
	if (this->note != NULL) {
		delete this->note;
		this->note = NULL; // OnClose ���� OnSize ȣ���.
	}
	if (this->characterMetrics != NULL) {
		delete this->characterMetrics;
	}
	if (this->selection != NULL) {
		delete this->selection;
	}

	CWnd::OnClose();
}

void TextEditingForm::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (nChar >= 32 || nChar == VK_TAB || nChar == VK_RETURN) {
		this->currentCharacter = nChar;
		if (this->selection != NULL) {
			this->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETESELECTION, 0));
		}
		this->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_WRITE, 0));
	}
}

LRESULT TextEditingForm::OnImeComposition(WPARAM wParam, LPARAM lParam) {
	if (lParam & GCS_COMPSTR) {
		this->currentBuffer[0] = (TCHAR)HIBYTE(LOWORD(wParam));
		this->currentBuffer[1] = (TCHAR)LOBYTE(LOWORD(wParam));
		if (this->selection != NULL) {
			this->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETESELECTION, 0));
		}
		this->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_IMECOMPOSITION, 0));
	}

	return ::DefWindowProc(this->m_hWnd, WM_IME_COMPOSITION, wParam, lParam);
}

LRESULT TextEditingForm::OnImeChar(WPARAM wParam, LPARAM lParam) {
	if (IsDBCSLeadByte((BYTE)(wParam >> 8)) == TRUE) {
		this->currentBuffer[0] = (TCHAR)HIBYTE(LOWORD(wParam));
		this->currentBuffer[1] = (TCHAR)LOBYTE(LOWORD(wParam));
	}
	else {
		this->currentBuffer[0] = (TCHAR)wParam;
	}

	this->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_IMECHAR, 0));

	this->isComposing = FALSE;

	return 0;
}

LRESULT TextEditingForm::OnImeStartComposition(WPARAM wParam, LPARAM lParam) {
	return 0;
}

void TextEditingForm::OnPaint() {
	CPaintDC dc(this);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bitmap;

	CRect rect;
	this->GetClientRect(&rect);
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* oldBitmap = memDC.SelectObject(&bitmap);
	memDC.FillSolidRect(&rect, RGB(255, 255, 255));

	CFont* oldFont;
	COLORREF oldColor;
	CFont font;
	this->font->Create(font);
	oldFont = memDC.SelectObject(&font);
	oldColor = memDC.SetTextColor(this->font->GetColor());

	Visitor* drawingVisitor = new DrawingVisitor(&memDC, this->characterMetrics, this->scrollController, this->selection);

	this->note->Accept(drawingVisitor);

	if (drawingVisitor != NULL) {
		delete drawingVisitor;
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(oldFont);
	memDC.SetTextColor(oldColor);

	memDC.SelectObject(oldBitmap);
	bitmap.DeleteObject();
	memDC.DeleteDC();
}

void TextEditingForm::OnSize(UINT nType, int cs, int cy) {
	if (this->scrollController == NULL) {
		this->scrollController = new ScrollController(this);
	}
	if (this->note != NULL) {
		this->Notify();
	}
}

void TextEditingForm::OnSetFocus(CWnd* pNewWnd) {
	if (this->caretController == NULL) {
		this->caretController = new CaretController(this);
	}
	this->Notify();
}

void TextEditingForm::OnKillFocus(CWnd* pNewWnd) {
	if (this->caretController != NULL) {
		delete this->caretController;
		this->caretController = NULL;
	}
}

void TextEditingForm::OnLButtonDown(UINT nFlag, CPoint point) {
	Long noteCurrent = this->note->GetCurrent();
	Long lineCurrent = this->current->GetCurrent();

	Long row = this->characterMetrics->GetRow(this->scrollController->GetVerticalScroll()->GetPosition() + point.y);
	if (row >= this->note->GetLength()) {
		row = this->note->GetLength() - 1;
	}
	Long index = this->note->Move(row);
	this->current = this->note->GetAt(index);
	Long column = this->characterMetrics->GetColumn(this->current, this->scrollController->GetHorizontalScroll()->GetPosition() + point.x);
	Long lineIndex = this->current->Move(column);

	//�����ϴ� �߰�
	if (nFlag != 5) {
		if (this->selection != NULL) {
			this->note->UnselectAll();
			delete this->selection;
			this->selection = NULL;
		}
	}

	if (nFlag == 5) {
		Long startRow = noteCurrent;
		Long endRow = index;
		Long startColumn = lineCurrent;
		Long endColumn = lineIndex;
		if (noteCurrent > index) {
			startRow = index;
			endRow = noteCurrent;
			startColumn = lineIndex;
			endColumn = lineCurrent;
		}
		else if (noteCurrent == index) {
			if (lineCurrent > lineIndex) {
				startColumn = lineIndex;
				endColumn = lineCurrent;
			}
		}
		Glyph* line;
		Glyph* character;
		Long length;
		Long j;
		Long i = startRow;
		while (i <= endRow) {
			line = this->note->GetAt(i);

			length = line->GetLength();
			if (i == endRow) {
				length = endColumn;
			}

			j = 0;
			if (i == startRow) {
				j = startColumn;
			}

			while (j < length) {
				character = line->GetAt(j);
				if (!character->GetIsSelected()) {
					character->Select(true);
				}
				else {
					character->Select(false);
				}
				j++;
			}
			i++;
		}

		Long start = startRow;
		Long end = endRow;
		if (this->selection != NULL) {
			Long originStart = this->selection->GetStart();
			Long originEnd = this->selection->GetEnd();
			if (originEnd == noteCurrent) { //������ ��
				start = originStart;
				end = row;
			}
			else if (originStart == noteCurrent) { //���� ������ ��
				end = originEnd;
				start = row;
			}
			delete this->selection;
			this->selection = NULL;
		}
		this->selection = new Selection(start, end);

		if (start == end && this->note->IsSelecting() == false) {
			delete this->selection;
			this->selection = NULL;
		}
	}

	this->Notify();
	this->Invalidate(); //�����ϴ� �߰�
}

void TextEditingForm::OnMouseMove(UINT nFlag, CPoint point) {
	if (nFlag == MK_LBUTTON) {
		Long noteCurrent = this->note->GetCurrent();
		Long lineCurrent = this->current->GetCurrent();

		Long row = this->characterMetrics->GetRow(this->scrollController->GetVerticalScroll()->GetPosition() + point.y);
		if (row >= this->note->GetLength()) {
			row = this->note->GetLength() - 1;
		}
		Long index = this->note->Move(row);
		this->current = this->note->GetAt(index);
		Long column = this->characterMetrics->GetColumn(this->current, this->scrollController->GetHorizontalScroll()->GetPosition() + point.x);
		Long lineIndex = this->current->Move(column);

		Long startRow = noteCurrent;
		Long endRow = index;
		Long startColumn = lineCurrent;
		Long endColumn = lineIndex;
		if (noteCurrent > index) {
			startRow = index;
			endRow = noteCurrent;
			startColumn = lineIndex;
			endColumn = lineCurrent;
		}
		else if (noteCurrent == index) {
			if (lineCurrent > lineIndex) {
				startColumn = lineIndex;
				endColumn = lineCurrent;
			}
		}
		Glyph* line;
		Glyph* character;
		Long length;
		Long j;
		Long i = startRow;
		while (i <= endRow) {
			line = this->note->GetAt(i);

			length = line->GetLength();
			if (i == endRow) {
				length = endColumn;
			}

			j = 0;
			if (i == startRow) {
				j = startColumn;
			}

			while (j < length) {
				character = line->GetAt(j);
				if (!character->GetIsSelected()) {
					character->Select(true);
				}
				else {
					character->Select(false);
				}
				j++;
			}
			i++;
		}

		Long start = startRow;
		Long end = endRow;
		if (this->selection != NULL) {
			Long originStart = this->selection->GetStart();
			Long originEnd = this->selection->GetEnd();
			if (originEnd == noteCurrent) { //������ ��
				start = originStart;
				end = row;
			}
			else if (originStart == noteCurrent) { //���� ������ ��
				end = originEnd;
				start = row;
			}
			delete this->selection;
			this->selection = NULL;
		}
		this->selection = new Selection(start, end);

		if (start == end && this->note->IsSelecting() == false) {
			delete this->selection;
			this->selection = NULL;
		}

		this->Notify();
		this->Invalidate();
	}
}

void TextEditingForm::OnCommandRange(UINT uID) {
	CommandFactory commandFactory(this);
	Command* command = commandFactory.Make(uID);
	if (command != NULL) {
		command->Execute();
		delete command;
	}

	if (this->scrollController != NULL) {
		delete this->scrollController;
	}
	this->scrollController = new ScrollController(this);

	this->Notify();
	this->Invalidate();

	Long x = this->characterMetrics->GetX(this->current) + 1; // 
	Long y = this->characterMetrics->GetY(this->note->GetCurrent() + 1); // 0���̽��̹Ƿ� 1����
	this->scrollController->SmartScrollToPoint(x, y);
}

void TextEditingForm::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	KeyActionFactory keyActionFactory(this);
	KeyAction* keyAction = keyActionFactory.Make(nChar);

	if (keyAction != 0) {
		keyAction->OnKeyDown(nChar, nRepCnt, nFlags);
		Long x = this->characterMetrics->GetX(this->current) + 1; // 
		Long y = this->characterMetrics->GetY(this->note->GetCurrent() + 1); // 0���̽��̹Ƿ� 1����
		this->scrollController->SmartScrollToPoint(x, y);
		delete keyAction;
	}
	this->Notify();
	this->Invalidate();
}

void TextEditingForm::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	HScrollActionFactory hScrollFactory(this);
	HScrollAction* hScrollAction = hScrollFactory.Make(nSBCode);
	if (hScrollAction != NULL) {
		hScrollAction->OnHScroll(nSBCode, nPos, pScrollBar);
		delete hScrollAction;
	}
	this->Invalidate();
}

void TextEditingForm::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	VScrollActionFactory vScrollActionFactory(this);

	VScrollAction* vScrollAction = vScrollActionFactory.Make(nSBCode);
	if (vScrollAction != NULL) {
		vScrollAction->OnVScroll(nSBCode, nPos, pScrollBar);
		delete vScrollAction;
	}
	this->Invalidate();

}

BOOL TextEditingForm::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {

	LONG style = this->GetStyle();

	if ((style & WS_VSCROLL) != 0) {
		Long position = this->scrollController->Rotate(zDelta);
		Long previousPosition = this->SetScrollPos(SB_VERT, position, TRUE);
		position = this->GetScrollPos(SB_VERT);
		this->scrollController->MoveVerticalScroll(position);
		this->ScrollWindow(0, previousPosition - position);
		this->Notify();
	}
	return TRUE;
}

BOOL TextEditingForm::OnEraseBkgnd(CDC* pDC) {
	return TRUE;
}