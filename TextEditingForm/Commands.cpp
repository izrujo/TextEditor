#include "Commands.h"
#include "TextEditingForm.h"
#include "GlyphFactory.h"
#include "Glyph.h"
#include "resource.h"
#include "Selection.h"
#include "Scanner.h"
#include "CharacterMetrics.h"
#include "CaretController.h"
#include "ScrollController.h"
#include "Scroll.h"

#include <afxwin.h>

#pragma warning(disable:4996)

Command::Command(TextEditingForm* textEditingForm) {
	this->textEditingForm = textEditingForm;
}

Command::Command(const Command& source) {
	this->textEditingForm = source.textEditingForm;
}

Command::~Command() {

}

Command& Command::operator=(const Command& source) {
	this->textEditingForm = source.textEditingForm;

	return *this;
}

//WriteCommand
WriteCommand::WriteCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

WriteCommand::WriteCommand(const WriteCommand& source)
	: Command(source) {

}

WriteCommand::~WriteCommand() {

}

WriteCommand& WriteCommand::operator=(const WriteCommand& source) {
	Command::operator=(source);

	return *this;
}

void WriteCommand::Execute() {
	GlyphFactory glyphFactory;
	TCHAR content[2];
	int nChar = this->textEditingForm->GetCurrentCharacter();
	Long row = this->textEditingForm->note->GetCurrent();
	Long column = this->textEditingForm->current->GetCurrent();

	if (nChar >= 32 || nChar == VK_TAB) {
		content[0] = nChar;
		Glyph* character = glyphFactory.Make(content);

		if (column >= this->textEditingForm->current->GetLength()) {
			this->textEditingForm->current->Add(character);
		}
		else {
			this->textEditingForm->current->Add(column, character);
		}
	}
	else if (nChar == VK_RETURN) {
		if (column < this->textEditingForm->current->GetLength()) {
			this->textEditingForm->current = this->textEditingForm->current->Divide(column);
			this->textEditingForm->note->Add(row + 1, this->textEditingForm->current);
			this->textEditingForm->current->First();
		}
		else {
			this->textEditingForm->current = glyphFactory.Make("\r\n");
			this->textEditingForm->note->Add(row + 1, this->textEditingForm->current);
		}
	}
}

string WriteCommand::GetType() {
	return "Write";
}

Command* WriteCommand::Clone() {
	return new WriteCommand(*this);
}

//ImeCompositionCommand
ImeCompositionCommand::ImeCompositionCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

ImeCompositionCommand::ImeCompositionCommand(const ImeCompositionCommand& source)
	: Command(source) {

}

ImeCompositionCommand::~ImeCompositionCommand() {

}

ImeCompositionCommand& ImeCompositionCommand::operator=(const ImeCompositionCommand& source) {
	Command::operator=(source);

	return *this;
}

void ImeCompositionCommand::Execute() {
	TCHAR(*buffer) = new TCHAR[2];
	buffer = this->textEditingForm->GetCurrentBuffer();

	Long index;

	if (this->textEditingForm->GetIsComposing() == TRUE) {
		index = this->textEditingForm->current->GetCurrent();
		this->textEditingForm->current->Remove(index - 1);
	}

	if (buffer[0] != '\0') {
		this->textEditingForm->SetIsComposing(TRUE);
		GlyphFactory glyphFactory;
		Glyph* doubleByteCharacter = glyphFactory.Make(buffer);
		index = this->textEditingForm->current->GetCurrent();

		if (index >= this->textEditingForm->current->GetLength()) {
			this->textEditingForm->current->Add(doubleByteCharacter);
		}
		else {
			this->textEditingForm->current->Add(index, doubleByteCharacter);
		}
	}
	else {
		this->textEditingForm->SetIsComposing(FALSE);
	}
}

string ImeCompositionCommand::GetType() {
	return "ImeComposition";
}

Command* ImeCompositionCommand::Clone() {
	return new ImeCompositionCommand(*this);
}

//ImeCharCommand
ImeCharCommand::ImeCharCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

ImeCharCommand::ImeCharCommand(const ImeCharCommand& source)
	: Command(source) {

}

ImeCharCommand::~ImeCharCommand() {

}

ImeCharCommand& ImeCharCommand::operator=(const ImeCharCommand& source) {
	Command::operator=(source);

	return *this;
}

void ImeCharCommand::Execute() {
	TCHAR buffer[2];
	buffer[0] = this->textEditingForm->GetCurrentBuffer()[0];
	buffer[1] = this->textEditingForm->GetCurrentBuffer()[1];
	Long column = this->textEditingForm->current->GetCurrent();

	if (this->textEditingForm->GetIsComposing() == TRUE) {
		this->textEditingForm->current->Remove(--column);
	}

	GlyphFactory glyphFactory;
	Glyph* glyph = glyphFactory.Make(buffer);

	if (column >= this->textEditingForm->current->GetLength()) {
		this->textEditingForm->current->Add(glyph);
	}
	else {
		this->textEditingForm->current->Add(column, glyph);
	}
}

string ImeCharCommand::GetType() {
	return "ImeChar";
}

Command* ImeCharCommand::Clone() {
	return new ImeCharCommand(*this);
}

//DeleteCommand
DeleteCommand::DeleteCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

DeleteCommand::DeleteCommand(const DeleteCommand& source)
	: Command(source) {

}

DeleteCommand::~DeleteCommand() {

}

DeleteCommand& DeleteCommand::operator=(const DeleteCommand& source) {
	Command::operator=(source);

	return *this;
}

void DeleteCommand::Execute() {
	Long row = this->textEditingForm->note->GetCurrent();
	Long column = this->textEditingForm->current->GetCurrent();
	Long noteLength = this->textEditingForm->note->GetLength();
	Long lineLength = this->textEditingForm->current->GetLength();

	if (column < lineLength) {
		this->textEditingForm->current->Remove(column);
	}
	else if (column >= lineLength && row < noteLength - 1) {
		Glyph* other = this->textEditingForm->note->GetAt(row + 1);
		this->textEditingForm->current->Combine(other);
		this->textEditingForm->note->Remove(row + 1);
	}
}

string DeleteCommand::GetType() {
	return "Delete";
}

Command* DeleteCommand::Clone() {
	return new DeleteCommand(*this);
}

//SelectAllCommand
SelectAllCommand::SelectAllCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

SelectAllCommand::SelectAllCommand(const SelectAllCommand& source)
	: Command(source) {

}

SelectAllCommand::~SelectAllCommand() {

}

SelectAllCommand& SelectAllCommand::operator=(const SelectAllCommand& source) {
	Command::operator=(source);

	return *this;
}

void SelectAllCommand::Execute() {
	Glyph* line;
	Long j;
	Long i = 0;
	while (i < this->textEditingForm->note->GetLength()) {
		line = this->textEditingForm->note->GetAt(i);
		j = 0;
		while (j < line->GetLength()) {
			line->GetAt(j)->Select(true);
			j++;
		}
		i++;
	}
	Long index = this->textEditingForm->note->Last();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
	this->textEditingForm->current->Last();

	if (this->textEditingForm->selection != NULL) {
		delete this->textEditingForm->selection;
	}

	//노트에 내용이 하나도 없으면 선택하지 않는다.
	if (!(this->textEditingForm->note->GetLength() <= 1 && this->textEditingForm->current->GetLength() <= 0)) {
		this->textEditingForm->selection = new Selection(0, this->textEditingForm->note->GetLength() - 1);
	}
}

string SelectAllCommand::GetType() {
	return "SelectAll";
}

Command* SelectAllCommand::Clone() {
	return new SelectAllCommand(*this);
}

//CopyCommand
CopyCommand::CopyCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

CopyCommand::CopyCommand(const CopyCommand& source)
	: Command(source) {

}

CopyCommand::~CopyCommand() {

}

CopyCommand& CopyCommand::operator=(const CopyCommand& source) {
	Command::operator=(source);

	return *this;
}

void CopyCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		Long start = this->textEditingForm->selection->GetStart();
		Long end = this->textEditingForm->selection->GetEnd();
		CString clipBoard;
		string content;
		Glyph* line;
		Glyph* character;
		Long column = 0;
		Long j;
		Long i = start;
		while (i <= end) {
			content = "";
			line = this->textEditingForm->note->GetAt(i);
			j = 0;
			while (j < line->GetLength()) {
				character = line->GetAt(j);
				if (character->GetIsSelected()) {
					column = j + 1;
					content += character->GetContent();
				}
				j++;
			}

			if (column >= line->GetLength()) {
				content.append("\r\n");
			}
			clipBoard.Append(content.c_str());
			i++;
		}

		HANDLE handle = 0;
		char* address = NULL;
		handle = ::GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, clipBoard.GetLength() + 1);
		address = (char*)::GlobalLock(handle);
		if (address == NULL) {
			::GlobalFree(handle);
		}
		else {
			strcpy(address, clipBoard);
		}
		if (::OpenClipboard(this->textEditingForm->m_hWnd)) {
			::EmptyClipboard();
			::SetClipboardData(CF_TEXT, handle);
			::CloseClipboard();
		}
		::GlobalUnlock(handle);
	}
}

string CopyCommand::GetType() {
	return "Copy";
}

Command* CopyCommand::Clone() {
	return new CopyCommand(*this);
}

//DeleteSelectionCommand
DeleteSelectionCommand::DeleteSelectionCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

DeleteSelectionCommand::DeleteSelectionCommand(const DeleteSelectionCommand& source)
	: Command(source) {

}

DeleteSelectionCommand::~DeleteSelectionCommand() {

}

DeleteSelectionCommand& DeleteSelectionCommand::operator=(const DeleteSelectionCommand& source) {
	Command::operator=(source);

	return *this;
}

void DeleteSelectionCommand::Execute() {
	Long i;
	Long start = this->textEditingForm->selection->GetStart();
	Long end = this->textEditingForm->selection->GetEnd();
	Glyph* character;
	Glyph* line;
	Long j;
	i = start;
	while (i <= end) {
		line = this->textEditingForm->note->GetAt(i);
		j = 0;
		while (j < line->GetLength()) {
			character = line->GetAt(j);
			if (character->GetIsSelected()) {
				line->Remove(j--);
			}
			j++;
		}
		if (j >= line->GetLength() && i < end) {
			this->textEditingForm->current = line->Combine(this->textEditingForm->note->GetAt(i + 1));
			this->textEditingForm->note->Remove(i + 1);
			i--;
			end--;
		}
		i++;
	}

	this->textEditingForm->note->Move(start);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(start);

	if (this->textEditingForm->selection != NULL) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
		this->textEditingForm->note->UnselectAll();
	}
}

string DeleteSelectionCommand::GetType() {
	return "DeleteSelection";
}

Command* DeleteSelectionCommand::Clone() {
	return new DeleteSelectionCommand(*this);
}

//CutCommand
CutCommand::CutCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

CutCommand::CutCommand(const CutCommand& source)
	: Command(source) {

}

CutCommand::~CutCommand() {

}

CutCommand& CutCommand::operator=(const CutCommand& source) {
	Command::operator=(source);

	return *this;
}

void CutCommand::Execute() { //테스트 요망..
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_COPY, 0));
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETESELECTION, 0));
}

string CutCommand::GetType() {
	return "Cut";
}

Command* CutCommand::Clone() {
	return new CutCommand(*this);
}

//PasteCommand
PasteCommand::PasteCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

PasteCommand::PasteCommand(const PasteCommand& source)
	: Command(source) {

}

PasteCommand::~PasteCommand() {

}

PasteCommand& PasteCommand::operator=(const PasteCommand& source) {
	Command::operator=(source);

	return *this;
}

void PasteCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETESELECTION, 0));
	}

	//시스템 클립보드에서 복사된 문자열을 가져오다.
	string clipBoard;
	HANDLE handle;
	LPSTR address = NULL;
	if (::IsClipboardFormatAvailable(CF_TEXT) != FALSE) {
		if (::OpenClipboard(this->textEditingForm->m_hWnd)) {
			handle = GetClipboardData(CF_TEXT);
			if (handle != NULL) {
				address = (LPSTR)::GlobalLock(handle);
				if (address != 0) {
					clipBoard = address;
				}
				::GlobalUnlock(handle);
			}
			CloseClipboard();
		}
	}

	//복사한 문자열을 임시 Note로 만들다.
	Scanner scanner(clipBoard);
	GlyphFactory glyphFactory;
	Glyph* glyphClipBoard = glyphFactory.Make("");
	Glyph* clipBoardLine = glyphFactory.Make("\r\n");
	glyphClipBoard->Add(clipBoardLine);
	while (scanner.IsEnd() == FALSE) {
		string token = scanner.GetToken();
		if (token != "\r\n") {
			Glyph* glyph = glyphFactory.Make(token.c_str());
			clipBoardLine->Add(glyph);
		}
		else {
			clipBoardLine = glyphFactory.Make(token.c_str());
			glyphClipBoard->Add(clipBoardLine);
		}
		scanner.Next();
	}
	Long i = 0;
	//현재 줄의 현재 위치에서 나누다.
	Long current = this->textEditingForm->current->GetCurrent();
	Glyph* line = this->textEditingForm->current->Divide(current);
	//현재 줄의 현재 위치부터 복사한 노트의 첫 번째 줄의 글자를 하나씩 추가한다.
	Glyph* copiedLine = glyphClipBoard->GetAt(i++);
	Long j = 0;
	while (j < copiedLine->GetLength()) {
		this->textEditingForm->current->Add(copiedLine->GetAt(j));
		j++;
	}
	//복사한 노트의 줄 수만큼 반복한다.
	while (i < glyphClipBoard->GetLength()) {
		//복사한 노트의 현재 줄을 가져오다.
		copiedLine = glyphClipBoard->GetAt(i);
		//원래 노트의 현재 위치에 가져온 줄을 추가하다.
		Long noteCurrent = this->textEditingForm->note->GetCurrent();
		this->textEditingForm->note->Add(noteCurrent + 1, copiedLine);
		i++;
	}
	//마지막으로 추가한 줄을 현재 줄로 한다.
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->textEditingForm->note->GetCurrent());
	//마지막 줄에 아까 캐럿 위치에서 나눈 줄을 이어 붙이다.
	this->textEditingForm->current->Combine(line);
}

string PasteCommand::GetType() {
	return "Paste";
}

Command* PasteCommand::Clone() {
	return new PasteCommand(*this);
}

//=============== Move Command ===============
//LeftCommand
LeftCommand::LeftCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

LeftCommand::LeftCommand(const LeftCommand& source)
	: Command(source) {
}

LeftCommand::~LeftCommand() {

}

LeftCommand& LeftCommand::operator=(const LeftCommand& source) {
	Command::operator=(source);

	return *this;
}

void LeftCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		Long start = this->textEditingForm->selection->GetStart();
		this->textEditingForm->note->Move(start);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(start);
		bool isSelected = false;
		Long i = 0;
		while (i < this->textEditingForm->current->GetLength() && isSelected == false) {
			isSelected = this->textEditingForm->current->GetAt(i)->GetIsSelected();
			i++;
		}
		Long startColumn = i-1;
		if (isSelected == false) {
			startColumn++;
		}
		this->textEditingForm->current->Move(startColumn);

		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	else {
		if (this->textEditingForm->current->GetCurrent() > 0) {
			this->textEditingForm->current->Previous();
		}
		else if (this->textEditingForm->note->GetCurrent() > 0) {
			Long index = this->textEditingForm->note->Previous();
			this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
			this->textEditingForm->current->Last();
		}
	}
}

string LeftCommand::GetType() {
	return "Left";
}

Command* LeftCommand::Clone() {
	return new LeftCommand(*this);
}

//RightCommand
RightCommand::RightCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

RightCommand::RightCommand(const RightCommand& source)
	: Command(source) {
}

RightCommand::~RightCommand() {

}

RightCommand& RightCommand::operator=(const RightCommand& source) {
	Command::operator=(source);

	return *this;
}

void RightCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		Long end = this->textEditingForm->selection->GetEnd();
		this->textEditingForm->note->Move(end);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(end);
		bool isSelected = false;
		Long i = this->textEditingForm->current->GetLength() - 1;
		while (i >= 0 && isSelected == false) {
			isSelected = this->textEditingForm->current->GetAt(i)->GetIsSelected();
			i--;
		}
		Long endColumn = i + 2;
		if (isSelected == false) {
			endColumn--;
		}
		this->textEditingForm->current->Move(endColumn);

		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	else {
		if (this->textEditingForm->current->GetCurrent() < this->textEditingForm->current->GetLength()) {
			this->textEditingForm->current->Next();
		}
		else if (this->textEditingForm->note->GetCurrent() < this->textEditingForm->note->GetLength() - 1) {
			Long index = this->textEditingForm->note->Next();
			this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
			this->textEditingForm->current->First();
		}
	}
}

string RightCommand::GetType() {
	return "Right";
}

Command* RightCommand::Clone() {
	return new RightCommand(*this);
}

//UpCommand
UpCommand::UpCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

UpCommand::UpCommand(const UpCommand& source)
	: Command(source) {
}

UpCommand::~UpCommand() {

}

UpCommand& UpCommand::operator=(const UpCommand& source) {
	Command::operator=(source);

	return *this;
}

void UpCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	if (this->textEditingForm->note->GetCurrent() > 0) {
		Long x = this->textEditingForm->characterMetrics->GetX(this->textEditingForm->current);
		Long index = this->textEditingForm->note->Previous();
		this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
		Long column = this->textEditingForm->characterMetrics->GetColumn(this->textEditingForm->current, x);
		this->textEditingForm->current->Move(column);
	}
}

string UpCommand::GetType() {
	return "Up";
}

Command* UpCommand::Clone() {
	return new UpCommand(*this);
}

//DownCommand
DownCommand::DownCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

DownCommand::DownCommand(const DownCommand& source)
	: Command(source) {
}

DownCommand::~DownCommand() {

}

DownCommand& DownCommand::operator=(const DownCommand& source) {
	Command::operator=(source);

	return *this;
}

void DownCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	if (this->textEditingForm->note->GetCurrent() < this->textEditingForm->note->GetLength() - 1) {
		Long x = this->textEditingForm->characterMetrics->GetX(this->textEditingForm->current);
		Long index = this->textEditingForm->note->Next();
		this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
		Long column = this->textEditingForm->characterMetrics->GetColumn(this->textEditingForm->current, x);
		this->textEditingForm->current->Move(column);
	}
}

string DownCommand::GetType() {
	return "Down";
}

Command* DownCommand::Clone() {
	return new DownCommand(*this);
}

//HomeCommand
HomeCommand::HomeCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

HomeCommand::HomeCommand(const HomeCommand& source)
	: Command(source) {
}

HomeCommand::~HomeCommand() {

}

HomeCommand& HomeCommand::operator=(const HomeCommand& source) {
	Command::operator=(source);

	return *this;
}

void HomeCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->current->GetCurrent();
	this->textEditingForm->current->First();
}

string HomeCommand::GetType() {
	return "Home";
}

Command* HomeCommand::Clone() {
	return new HomeCommand(*this);
}

//EndCommand
EndCommand::EndCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

EndCommand::EndCommand(const EndCommand& source)
	: Command(source) {
}

EndCommand::~EndCommand() {

}

EndCommand& EndCommand::operator=(const EndCommand& source) {
	Command::operator=(source);

	return *this;
}

void EndCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->current->GetCurrent();
	this->textEditingForm->current->Last();
}

string EndCommand::GetType() {
	return "End";
}

Command* EndCommand::Clone() {
	return new EndCommand(*this);
}

//CtrlLeftCommand
CtrlLeftCommand::CtrlLeftCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

CtrlLeftCommand::CtrlLeftCommand(const CtrlLeftCommand& source)
	: Command(source) {
}

CtrlLeftCommand::~CtrlLeftCommand() {

}

CtrlLeftCommand& CtrlLeftCommand::operator=(const CtrlLeftCommand& source) {
	Command::operator=(source);

	return *this;
}

void CtrlLeftCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	Long index = this->textEditingForm->note->MovePreviousWord();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
}

string CtrlLeftCommand::GetType() {
	return "CtrlLeft";
}

Command* CtrlLeftCommand::Clone() {
	return new CtrlLeftCommand(*this);
}

//CtrlRightCommand
CtrlRightCommand::CtrlRightCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

CtrlRightCommand::CtrlRightCommand(const CtrlRightCommand& source)
	: Command(source) {
}

CtrlRightCommand::~CtrlRightCommand() {

}

CtrlRightCommand& CtrlRightCommand::operator=(const CtrlRightCommand& source) {
	Command::operator=(source);

	return *this;
}

void CtrlRightCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	Long index = this->textEditingForm->note->MoveNextWord();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
}

string CtrlRightCommand::GetType() {
	return "CtrlRight";
}

Command* CtrlRightCommand::Clone() {
	return new CtrlRightCommand(*this);
}

//CtrlHomeCommand
CtrlHomeCommand::CtrlHomeCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

CtrlHomeCommand::CtrlHomeCommand(const CtrlHomeCommand& source)
	: Command(source) {
}

CtrlHomeCommand::~CtrlHomeCommand() {

}

CtrlHomeCommand& CtrlHomeCommand::operator=(const CtrlHomeCommand& source) {
	Command::operator=(source);

	return *this;
}

void CtrlHomeCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	Long index = this->textEditingForm->note->First();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
	this->textEditingForm->current->First();
}

string CtrlHomeCommand::GetType() {
	return "CtrlHome";
}

Command* CtrlHomeCommand::Clone() {
	return new CtrlHomeCommand(*this);
}

//CtrlEndCommand
CtrlEndCommand::CtrlEndCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

CtrlEndCommand::CtrlEndCommand(const CtrlEndCommand& source)
	: Command(source) {
}

CtrlEndCommand::~CtrlEndCommand() {

}

CtrlEndCommand& CtrlEndCommand::operator=(const CtrlEndCommand& source) {
	Command::operator=(source);

	return *this;
}

void CtrlEndCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->note->UnselectAll();
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	Long index = this->textEditingForm->note->Last();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
	this->textEditingForm->current->Last();
}

string CtrlEndCommand::GetType() {
	return "CtrlEnd";
}

Command* CtrlEndCommand::Clone() {
	return new CtrlEndCommand(*this);
}

//PageUpCommand
PageUpCommand::PageUpCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

PageUpCommand::PageUpCommand(const PageUpCommand& source)
	: Command(source) {
}

PageUpCommand::~PageUpCommand() {

}

PageUpCommand& PageUpCommand::operator=(const PageUpCommand& source) {
	Command::operator=(source);

	return *this;
}

void PageUpCommand::Execute() {
	Long position = this->textEditingForm->scrollController->PageUp();
	Long previous = this->textEditingForm->SetScrollPos(SB_VERT, position, TRUE);
	position = this->textEditingForm->GetScrollPos(SB_VERT);
	this->textEditingForm->scrollController->MoveVerticalScroll(position);
	this->textEditingForm->ScrollWindow(0, previous - position);
	// Caret의 x값은 노트의 위치에 스크롤 위치를 뺀 상태에 값이 들어가 있다. 따라서 현재 x값을 다시 더한다.
	Long x = this->textEditingForm->caretController->GetCaretX() +
		this->textEditingForm->scrollController->GetHorizontalScroll()->GetPosition();
	// Caret의 x값은 노트의 위치에 스크롤 위치를 뺀 상태에 값이 들어가 있다.따라서 이전 y값을 다시 더한다.
	Long y = this->textEditingForm->caretController->GetCaretY() + previous;

	Long row = this->textEditingForm->characterMetrics->GetRow(y - (previous - position));
	if (row < 0) {
		row = 0;
	}
	Long index = this->textEditingForm->note->Move(row); // row가 벗어나는 경우는 없나?
	this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
	Long column = this->textEditingForm->characterMetrics->GetColumn(this->textEditingForm->current, x);
	this->textEditingForm->current->Move(column);
}

string PageUpCommand::GetType() {
	return "PageUp";
}

Command* PageUpCommand::Clone() {
	return new PageUpCommand(*this);
}

//PageDownCommand
PageDownCommand::PageDownCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

PageDownCommand::PageDownCommand(const PageDownCommand& source)
	: Command(source) {
}

PageDownCommand::~PageDownCommand() {

}

PageDownCommand& PageDownCommand::operator=(const PageDownCommand& source) {
	Command::operator=(source);

	return *this;
}

void PageDownCommand::Execute() {
	Long position = this->textEditingForm->scrollController->PageDown();
	Long previous = this->textEditingForm->SetScrollPos(SB_VERT, position, TRUE);
	this->textEditingForm->ScrollWindow(0, previous - position);
	Long x = this->textEditingForm->caretController->GetCaretX() +
		this->textEditingForm->scrollController->GetHorizontalScroll()->GetPosition();
	Long y = this->textEditingForm->caretController->GetCaretY() + previous;
	Long row = this->textEditingForm->characterMetrics->GetRow(y - (previous - position));
	if (row > this->textEditingForm->note->GetLength() - 1) {
		row = this->textEditingForm->note->GetLength() - 1;
	}
	Long index = this->textEditingForm->note->Move(row);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(index);
	Long column = this->textEditingForm->characterMetrics->GetColumn(this->textEditingForm->current, x);
	this->textEditingForm->current->Move(column);
}

string PageDownCommand::GetType() {
	return "PageDown";
}

Command* PageDownCommand::Clone() {
	return new PageDownCommand(*this);
}

//ShiftLeftCommand
ShiftLeftCommand::ShiftLeftCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftLeftCommand::ShiftLeftCommand(const ShiftLeftCommand& source)
	: Command(source) {
}

ShiftLeftCommand::~ShiftLeftCommand() {

}

ShiftLeftCommand& ShiftLeftCommand::operator=(const ShiftLeftCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftLeftCommand::Execute() {
	Glyph* character;
	Long column;
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long row = noteCurrent;
	Long lineCurrent = this->textEditingForm->current->GetCurrent();
	if (lineCurrent > 0) {
		column = this->textEditingForm->current->Previous();
		character = this->textEditingForm->current->GetAt(column);
		(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
	}
	else if (noteCurrent > 0) {
		row = this->textEditingForm->note->Previous();
		this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
		this->textEditingForm->current->Last();
	}

	Long start = row;
	Long end = noteCurrent;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originStart == noteCurrent) { //선택할 때
			start = row;
			end = originEnd;
		}
		else if (originEnd == noteCurrent) { //선택 해제할 때
			end = row;
			start = originStart;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftLeftCommand::GetType() {
	return "ShiftLeft";
}

Command* ShiftLeftCommand::Clone() {
	return new ShiftLeftCommand(*this);
}

//ShiftRightCommand
ShiftRightCommand::ShiftRightCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftRightCommand::ShiftRightCommand(const ShiftRightCommand& source)
	: Command(source) {
}

ShiftRightCommand::~ShiftRightCommand() {

}

ShiftRightCommand& ShiftRightCommand::operator=(const ShiftRightCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftRightCommand::Execute() {
	Glyph* character;
	Long column;
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long row = noteCurrent;
	Long lineCurrent = this->textEditingForm->current->GetCurrent();
	if (lineCurrent < this->textEditingForm->current->GetLength()) {
		column = this->textEditingForm->current->Next();
		character = this->textEditingForm->current->GetAt(column - 1);
		(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
	}
	else if (noteCurrent < this->textEditingForm->note->GetLength() - 1) {
		row = this->textEditingForm->note->Next();
		this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
		this->textEditingForm->current->First();
	}

	Long start = noteCurrent;
	Long end = row;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originEnd == noteCurrent) { //선택할 때
			start = originStart;
			end = row;
		}
		else if (originStart == noteCurrent) { //선택 해제할 때
			end = originEnd;
			start = row;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftRightCommand::GetType() {
	return "ShiftRight";
}

Command* ShiftRightCommand::Clone() {
	return new ShiftRightCommand(*this);
}

//ShiftUpCommand
ShiftUpCommand::ShiftUpCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftUpCommand::ShiftUpCommand(const ShiftUpCommand& source)
	: Command(source) {
}

ShiftUpCommand::~ShiftUpCommand() {

}

ShiftUpCommand& ShiftUpCommand::operator=(const ShiftUpCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftUpCommand::Execute() {
	Glyph* character;
	Long lineCurrent = this->textEditingForm->current->GetCurrent();
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long row = noteCurrent;
	if (noteCurrent > 0) {
		Long i = lineCurrent;
		while (i > 0) {
			character = this->textEditingForm->current->GetAt(i - 1);
			(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
			i--;
		}

		Long x = this->textEditingForm->characterMetrics->GetX(this->textEditingForm->current);
		row = this->textEditingForm->note->Previous();
		this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
		Long column = this->textEditingForm->characterMetrics->GetColumn(this->textEditingForm->current, x);
		this->textEditingForm->current->Move(column);

		i = this->textEditingForm->current->GetLength();
		while (i > column) {
			character = this->textEditingForm->current->GetAt(i - 1);
			(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
			i--;
		}
	}

	Long start = row;
	Long end = noteCurrent;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originStart == noteCurrent) { //선택할 때
			start = row;
			end = originEnd;
		}
		else if (originEnd == noteCurrent) { //선택 해제할 때
			end = row;
			start = originStart;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftUpCommand::GetType() {
	return "ShiftUp";
}

Command* ShiftUpCommand::Clone() {
	return new ShiftUpCommand(*this);
}

//ShiftDownCommand
ShiftDownCommand::ShiftDownCommand(TextEditingForm * textEditingForm)
	: Command(textEditingForm) {
}

ShiftDownCommand::ShiftDownCommand(const ShiftDownCommand& source)
	: Command(source) {
}

ShiftDownCommand::~ShiftDownCommand() {

}

ShiftDownCommand& ShiftDownCommand::operator=(const ShiftDownCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftDownCommand::Execute() {
	Glyph* character;
	Long lineCurrent = this->textEditingForm->current->GetCurrent();
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long row = noteCurrent;
	if (noteCurrent < this->textEditingForm->note->GetLength() - 1) {
		Long i = lineCurrent;
		while (i < this->textEditingForm->current->GetLength()) {
			character = this->textEditingForm->current->GetAt(i);
			(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
			i++;
		}

		Long x = this->textEditingForm->characterMetrics->GetX(this->textEditingForm->current);
		row = this->textEditingForm->note->Next();
		this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
		Long column = this->textEditingForm->characterMetrics->GetColumn(this->textEditingForm->current, x);
		this->textEditingForm->current->Move(column);

		i = 0;
		while (i < column) {
			character = this->textEditingForm->current->GetAt(i);
			(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
			i++;
		}
	}

	Long start = noteCurrent;
	Long end = row;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originEnd == noteCurrent) { //선택할 때
			start = originStart;
			end = row;
		}
		else if (originStart == noteCurrent) { //선택 해제할 때
			end = originEnd;
			start = row;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftDownCommand::GetType() {
	return "ShiftDown";
}

Command* ShiftDownCommand::Clone() {
	return new ShiftDownCommand(*this);
}

//ShiftHomeCommand
ShiftHomeCommand::ShiftHomeCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftHomeCommand::ShiftHomeCommand(const ShiftHomeCommand& source)
	: Command(source) {
}

ShiftHomeCommand::~ShiftHomeCommand() {

}

ShiftHomeCommand& ShiftHomeCommand::operator=(const ShiftHomeCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftHomeCommand::Execute() {
	Glyph* character;
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long row = noteCurrent;
	Long lineCurrent = this->textEditingForm->current->GetCurrent();
	Long index = this->textEditingForm->current->First();
	Long i = lineCurrent;
	while (i > index) {
		character = this->textEditingForm->current->GetAt(i - 1);
		(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
		i--;
	}

	Long start = row;
	Long end = noteCurrent;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originStart == noteCurrent) { //선택할 때
			start = row;
			end = originEnd;
		}
		else if (originEnd == noteCurrent) { //선택 해제할 때
			end = row;
			start = originStart;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftHomeCommand::GetType() {
	return "ShiftHome";
}

Command* ShiftHomeCommand::Clone() {
	return new ShiftHomeCommand(*this);
}

//ShiftEndCommand
ShiftEndCommand::ShiftEndCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftEndCommand::ShiftEndCommand(const ShiftEndCommand& source)
	: Command(source) {
}

ShiftEndCommand::~ShiftEndCommand() {

}

ShiftEndCommand& ShiftEndCommand::operator=(const ShiftEndCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftEndCommand::Execute() {
	Glyph* character;
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long lineCurrent = this->textEditingForm->current->GetCurrent();
	Long index = this->textEditingForm->current->Last();
	Long row = this->textEditingForm->note->GetCurrent();
	Long i = lineCurrent;
	while (i < index) {
		character = this->textEditingForm->current->GetAt(i);
		(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
		i++;
	}

	Long start = noteCurrent;
	Long end = row;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originEnd == noteCurrent) { //선택할 때
			start = originStart;
			end = row;
		}
		else if (originStart == noteCurrent) { //선택 해제할 때
			end = originEnd;
			start = row;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftEndCommand::GetType() {
	return "ShiftEnd";
}

Command* ShiftEndCommand::Clone() {
	return new ShiftEndCommand(*this);
}

//ShiftCtrlLeftCommand
ShiftCtrlLeftCommand::ShiftCtrlLeftCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftCtrlLeftCommand::ShiftCtrlLeftCommand(const ShiftCtrlLeftCommand& source)
	: Command(source) {
}

ShiftCtrlLeftCommand::~ShiftCtrlLeftCommand() {

}

ShiftCtrlLeftCommand& ShiftCtrlLeftCommand::operator=(const ShiftCtrlLeftCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftCtrlLeftCommand::Execute() {
	Glyph* character;
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long row = noteCurrent;

	Long lineCurrent = this->textEditingForm->current->GetCurrent();

	row = this->textEditingForm->note->MovePreviousWord();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
	while (row >= 0) {
		row = this->textEditingForm->note->MovePreviousWord();
		this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
	}

	Long lineNext = this->textEditingForm->current->GetCurrent();
	Glyph* line;
	Long column;
	Long j;
	Long i = noteCurrent;
	while (i >= row) {
		line = this->textEditingForm->note->GetAt(i);
		column = 0;
		if (i <= row) {
			column = lineNext;
		}
		j = 0;
		if (i == noteCurrent) {
			j = lineCurrent;
		}
		while (j > column) {
			character = line->GetAt(j - 1);
			(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
			j--;
		}
		i--;
	}

	Long start = row;
	Long end = noteCurrent;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originStart == noteCurrent) { //선택할 때
			start = row;
			end = originEnd;
		}
		else if (originEnd == noteCurrent) { //선택 해제할 때
			end = row;
			start = originStart;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftCtrlLeftCommand::GetType() {
	return "ShiftCtrlLeft";
}

Command* ShiftCtrlLeftCommand::Clone() {
	return new ShiftCtrlLeftCommand(*this);
}

//ShiftCtrlRightCommand
ShiftCtrlRightCommand::ShiftCtrlRightCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftCtrlRightCommand::ShiftCtrlRightCommand(const ShiftCtrlRightCommand& source)
	: Command(source) {
}

ShiftCtrlRightCommand::~ShiftCtrlRightCommand() {

}

ShiftCtrlRightCommand& ShiftCtrlRightCommand::operator=(const ShiftCtrlRightCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftCtrlRightCommand::Execute() {
	Glyph* character;
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long row = noteCurrent;

	Long lineCurrent = this->textEditingForm->current->GetCurrent();

	row = this->textEditingForm->note->MoveNextWord();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
	while (row + 1 < this->textEditingForm->note->GetLength()) {
		row = this->textEditingForm->note->MoveNextWord();
		this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
	}

	Long lineNext = this->textEditingForm->current->GetCurrent();
	Glyph* line;
	Long column;
	Long j;
	Long i = noteCurrent;
	while (i <= row) {
		line = this->textEditingForm->note->GetAt(i);
		column = line->GetLength();
		if (i >= row) {
			column = lineNext;
		}
		j = 0;
		if (i == noteCurrent) {
			j = lineCurrent;
		}
		while (j < column) {
			character = line->GetAt(j);
			(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
			j++;
		}
		i++;
	}

	Long start = noteCurrent;
	Long end = row;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originEnd == noteCurrent) { //선택할 때
			start = originStart;
			end = row;
		}
		else if (originStart == noteCurrent) { //선택 해제할 때
			end = originEnd;
			start = row;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftCtrlRightCommand::GetType() {
	return "ShiftCtrlRight";
}

Command* ShiftCtrlRightCommand::Clone() {
	return new ShiftCtrlRightCommand(*this);
}

//ShiftCtrlHomeCommand
ShiftCtrlHomeCommand::ShiftCtrlHomeCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftCtrlHomeCommand::ShiftCtrlHomeCommand(const ShiftCtrlHomeCommand& source)
	: Command(source) {
}

ShiftCtrlHomeCommand::~ShiftCtrlHomeCommand() {

}

ShiftCtrlHomeCommand& ShiftCtrlHomeCommand::operator=(const ShiftCtrlHomeCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftCtrlHomeCommand::Execute() {
	Glyph* character;
	Glyph* line;
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long lineCurrent = this->textEditingForm->current->GetCurrent();

	Long row = this->textEditingForm->note->First();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
	this->textEditingForm->current->First();

	Long j;
	Long i = noteCurrent;
	while (i >= row) {
		line = this->textEditingForm->note->GetAt(i);
		j = line->GetLength();
		if (i == noteCurrent) {
			j = lineCurrent;
		}
		while (j > 0) {
			character = line->GetAt(j - 1);
			(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
			j--;
		}
		i--;
	}

	Long start = row;
	Long end = noteCurrent;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originStart == noteCurrent) { //선택할 때
			start = row;
			end = originEnd;
		}
		else if (originEnd == noteCurrent) { //선택 해제할 때
			end = row;
			start = originStart;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftCtrlHomeCommand::GetType() {
	return "ShiftCtrlHome";
}

Command* ShiftCtrlHomeCommand::Clone() {
	return new ShiftCtrlHomeCommand(*this);
}

//ShiftCtrlEndCommand
ShiftCtrlEndCommand::ShiftCtrlEndCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ShiftCtrlEndCommand::ShiftCtrlEndCommand(const ShiftCtrlEndCommand& source)
	: Command(source) {
}

ShiftCtrlEndCommand::~ShiftCtrlEndCommand() {

}

ShiftCtrlEndCommand& ShiftCtrlEndCommand::operator=(const ShiftCtrlEndCommand& source) {
	Command::operator=(source);

	return *this;
}

void ShiftCtrlEndCommand::Execute() {
	Glyph* character;
	Glyph* line;
	Long noteCurrent = this->textEditingForm->note->GetCurrent();
	Long lineCurrent = this->textEditingForm->current->GetCurrent();

	Long row = this->textEditingForm->note->Last();
	this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
	this->textEditingForm->current->Last();

	Long j;
	Long i = noteCurrent;
	while (i <= row) {
		line = this->textEditingForm->note->GetAt(i);
		j = 0;
		if (i == noteCurrent) {
			j = lineCurrent;
		}
		while (j < line->GetLength()) {
			character = line->GetAt(j);
			(!character->GetIsSelected()) ? (character->Select(true)) : (character->Select(false));
			j++;
		}
		i++;
	}

	Long start = noteCurrent;
	Long end = row;
	if (this->textEditingForm->selection != NULL) {
		Long originStart = this->textEditingForm->selection->GetStart();
		Long originEnd = this->textEditingForm->selection->GetEnd();
		if (originEnd == noteCurrent) { //선택할 때
			start = originStart;
			end = row;
		}
		else if (originStart == noteCurrent) { //선택 해제할 때
			end = originEnd;
			start = row;
		}
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
	this->textEditingForm->selection = new Selection(start, end);

	if (start == end && this->textEditingForm->note->IsSelecting() == false) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
	}
}

string ShiftCtrlEndCommand::GetType() {
	return "ShiftCtrlEnd";
}

Command* ShiftCtrlEndCommand::Clone() {
	return new ShiftCtrlEndCommand(*this);
}
//=============== Move Command ===============