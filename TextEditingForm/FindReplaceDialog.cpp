#include "FindReplaceDialog.h"
#include "TextEditingForm.h"
#include "Glyph.h"
#include "Selection.h"
#include "DummyManager.h"

FindReplaceDialog::FindReplaceDialog(BOOL findOnly, string findWhat, CWnd* parent)
	: CFindReplaceDialog() {
	(findOnly == TRUE) ? (this->m_fr.lpTemplateName = MAKEINTRESOURCE(IDF)) : (this->m_fr.lpTemplateName = MAKEINTRESOURCE(IDR));
	this->Create(findOnly, (LPCTSTR)findWhat.c_str(), NULL, FR_ENABLETEMPLATE | FR_HIDEWHOLEWORD | FR_DOWN, parent);
}

BOOL FindReplaceDialog::OnInitDialog() {
	CFindReplaceDialog::OnInitDialog();

	return FALSE;
}

BOOL FindReplaceDialog::WrapAround() {
	BOOL wrapAround = FALSE;

	int check = ((CButton*)GetDlgItem(IDC_CHECKBOX_WRAPAROUND))->GetCheck();

	if (check == BST_CHECKED) {
		wrapAround = TRUE;
	}

	return wrapAround;
}

BOOL FindReplaceDialog::Find() {
	BOOL isFindSuccess;
	TextEditingForm* textEditingForm= (TextEditingForm*)this->GetParent();

	BOOL isSearchDown = this->SearchDown();

	//========== �ڵ� ���� ó�� 1 ==========
	DummyManager* dummyManager = 0;
	Long currentRow = textEditingForm->note->GetCurrent();
	Long currentColumn = textEditingForm->current->GetCurrent();
	Long start = currentRow;
	Long end = textEditingForm->note->GetLength() - 1;
	if (isSearchDown == FALSE) {
		start = 0;
		end = currentRow;
	}
	Long distance = 0;
	if (textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(textEditingForm->note, textEditingForm->characterMetrics, rect.Width());
		distance = dummyManager->CountDistance(currentRow, currentColumn);
		dummyManager->Unfold(&start, &end); //���� ����� ������.
		dummyManager->CountIndex(distance, &currentRow, &currentColumn);
		textEditingForm->note->Move(currentRow);
		textEditingForm->current = textEditingForm->note->GetAt(currentRow);
		textEditingForm->current->Move(currentColumn);
	}
	//========== �ڵ� ���� ó�� 1 ==========

	String allContents = textEditingForm->note->GetContent();
	CString findString = this->GetFindString();
	String myFindString((LPCTSTR)findString);
	Long stringLength = myFindString.GetLength();
	
	BOOL wrapAround = this->WrapAround(); //Ŀ�����ؼ� '������ ��ġ'�� �ٲٱ�
	BOOL matchCase = this->MatchCase();
	if (matchCase == FALSE) {
		allContents.MakeLower();
		myFindString.MakeLower();
	}

	//String ������ ĳ����ġ ã��
	Long current = 0;
	Long lineLength;
	Long i = 0;
	while (i < textEditingForm->note->GetCurrent()) {
		lineLength = textEditingForm->note->GetAt(i)->GetLength();
		current += lineLength + 1;
		i++;
	}
	if (i == textEditingForm->note->GetCurrent()) {
		current += textEditingForm->note->GetAt(i)->GetCurrent();
	}

	if (isSearchDown == TRUE) {
		current = allContents.Find((char*)(LPCTSTR)findString, current);
		if (wrapAround == TRUE && current == -1) {
			textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_CTRLHOME, 0));
			current = 0; //String ������ ĳ����ġ - distance
			i = 0;
			while (i < textEditingForm->note->GetCurrent()) {
				lineLength = textEditingForm->note->GetAt(i)->GetLength();
				current += lineLength + 1;
				i++;
			}
			if (i == textEditingForm->note->GetCurrent()) {
				current += textEditingForm->note->GetAt(i)->GetCurrent();
			}
			current = allContents.Find((char*)(LPCTSTR)findString, current);
		}
	}
	else {
		current = allContents.ReversedFind((char*)(LPCTSTR)findString, current);
		if (wrapAround == TRUE && current == -1) {
			textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_CTRLEND, 0));
			current = 0; //String ������ ĳ����ġ
			i = 0;
			while (i < textEditingForm->note->GetCurrent()) {
				lineLength = textEditingForm->note->GetAt(i)->GetLength();
				current += lineLength + 1;
				i++;
			}
			if (i == textEditingForm->note->GetCurrent()) {
				current += textEditingForm->note->GetAt(i)->GetCurrent();
			}
			current = allContents.ReversedFind((char*)(LPCTSTR)findString, current);
		}
	}
	Long selectStart=0;
	Long startDistance=0;
	Long index = 0;
	if (current != -1) {
		if (textEditingForm->selection != NULL) {
			delete textEditingForm->selection;
			textEditingForm->selection = NULL;
			textEditingForm->note->UnselectAll();
		}

		//Glyph �� ��ġ ���ϰ� �̵��ϱ�
		if (isSearchDown == FALSE) {
			current += stringLength;
		}
		i = 0;
		Long linesLength = 0;
		while (linesLength <= current) {
			linesLength += textEditingForm->note->GetAt(i++)->GetLength() + 1;
		}

		textEditingForm->note->Move(--i);
		textEditingForm->current = textEditingForm->note->GetAt(i);
		linesLength -= textEditingForm->note->GetAt(i)->GetLength() + 1;
		index = current - linesLength;
		textEditingForm->current->Move(index);

		//�����ϱ�
		i = 1;
		while (i <= stringLength) {
			if (isSearchDown == TRUE) {
				if (textEditingForm->current->GetCurrent() >= textEditingForm->current->GetLength()) {
					textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_RIGHT, 0));
				}
				textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_RIGHT, 0));
			}
			else {
				if (textEditingForm->current->GetCurrent() <= 0) {
					textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_LEFT, 0));
				}
				textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_LEFT, 0));
			}
			i++;
		}
		isFindSuccess = true;
		selectStart = textEditingForm->selection->GetStart(); //�ڵ�����
	}
	else {
		isFindSuccess = false;
	}

	//========== �ڵ� ���� ó�� 2 ==========
	if (dummyManager != NULL) {
		startDistance = dummyManager->CountDistance(selectStart, index); //�ڵ�����
		currentRow = textEditingForm->note->GetCurrent();
		currentColumn = textEditingForm->current->GetCurrent();
		distance = dummyManager->CountDistance(currentRow, currentColumn);

		Long lastFoldedRow;
		Long i = start;
		while (i <= end && end < textEditingForm->note->GetLength()) {
			lastFoldedRow = dummyManager->Fold(i);
			end += lastFoldedRow - i;
			i = lastFoldedRow + 1;
		}
		dummyManager->CountIndex(distance, &currentRow, &currentColumn);
		dummyManager->CountIndex(startDistance, &selectStart, &index);

		delete dummyManager;

		if (textEditingForm->selection != NULL && isFindSuccess == true) {
			delete textEditingForm->selection;
			textEditingForm->selection = NULL;
			textEditingForm->selection = new Selection(selectStart, currentRow);
		}

		textEditingForm->note->Move(currentRow);
		textEditingForm->current = textEditingForm->note->GetAt(currentRow);
		textEditingForm->current->Move(currentColumn);
	}
	//========== �ڵ� ���� ó�� 2 ==========

	return isFindSuccess;
}

void FindReplaceDialog::Replace() {
	TextEditingForm* textEditingForm = (TextEditingForm*)this->GetParent();
	//���õ� ���ڵ�(ã��)�� �����.
	if (textEditingForm->selection != NULL) {
		textEditingForm->SetIsDeleteSelectionByInput(TRUE);
		textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETESELECTION, 0));
		textEditingForm->SetIsDeleteSelectionByInput(FALSE);

		//���� �ڸ��� �ٲ� ������ ����.
		CString replaceString = this->GetReplaceString();
		String myReplaceString((LPCTSTR)replaceString);
		Long stringLength = myReplaceString.GetLength();
		Long i = 0;
		while (i < replaceString.GetLength()) {
			if (myReplaceString.IsHangle(i) == false) {
				textEditingForm->SetCurrentCharacter(myReplaceString.GetAt(i));
				textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_WRITE, 0));
			}
			else {
				textEditingForm->SetCurrentBuffer((TCHAR*)myReplaceString.GetDoubleByteAt(i).c_str());
				i++;
				textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_IMECHAR, 0));
			}
			i++;
		}
	}
}