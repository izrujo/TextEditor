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
#include "HistoryBook.h"
#include "FindReplaceDialog.h"
#include "DummyManager.h"

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

void Command::Unexecute() {

}

Long Command::Add(Command* command) {
	return -1;
}

Long Command::Remove(Long index) {
	return -1;
}

Command* Command::GetAt(Long index) {
	return 0;
}

Long Command::GetCapacity() const {
	return 0;
}

Long Command::GetLength() const {
	return -1;
}

//////////////////// Composite ////////////////////
//MacroCommand
MacroCommand::MacroCommand(TextEditingForm* textEditingForm, Long capacity)
	: Command(textEditingForm), commands(10) {
	this->capacity = 10;
	this->length = 0;
}

MacroCommand::MacroCommand(const MacroCommand& source)
	: Command(source.textEditingForm), commands(source.capacity) {
	Command* command;
	Long i = 0;
	while (i < source.length) {
		command = const_cast<MacroCommand&>(source).commands[i]->Clone();
		this->commands.Store(i, command);
		i++;
	}
	this->capacity = source.capacity;
	this->length = source.length;
}

MacroCommand::~MacroCommand() {
	Long i = 0;
	while (i < this->length) {
		if (this->commands[i] != 0) {
			delete this->commands[i];
		}
		i++;
	}
}

MacroCommand& MacroCommand::operator=(const MacroCommand& source) {
	Command::operator=(source);
	Long i = 0;
	while (i < this->length) {
		if (this->commands[i] != 0) {
			delete this->commands[i];
		}
		i++;
	}

	this->commands = source.commands;
	this->capacity = source.capacity;

	i = 0;
	while (i < this->length) {
		this->commands.Modify(i, const_cast<MacroCommand&>(source).commands[i]->Clone());
		i++;
	}

	this->length = source.length;

	return *this;
}

void MacroCommand::Execute() {
	Long i = 0;
	while (i < this->length) {
		this->commands[i]->Execute();
		i++;
	}
}

void MacroCommand::Unexecute() {
	Long i = this->length - 1;
	while (i >= 0) {
		this->commands[i]->Unexecute();
		i--;
	}
}

Long MacroCommand::Add(Command* command) {
	Long index;
	if (this->length < this->capacity) {
		index = this->commands.Store(this->length, command);
	}
	else {
		index = this->commands.AppendFromRear(command);
		this->capacity++;
	}
	this->length++;

	return index;
}

Long MacroCommand::Remove(Long index) {
	if (this->commands[index] != 0) {
		delete this->commands.GetAt(index);
	}
	index = this->commands.Delete(index);
	this->capacity--;
	this->length--;

	return index;
}

Command* MacroCommand::GetAt(Long index) {
	return this->commands.GetAt(index);
}

string MacroCommand::GetType() {
	return "Macro";
}

Command* MacroCommand::Clone() {
	return new MacroCommand(*this);
}
//////////////////// Composite ////////////////////

//////////////////// Basic ////////////////////
//WriteBasicCommand
WriteBasicCommand::WriteBasicCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

WriteBasicCommand::WriteBasicCommand(const WriteBasicCommand& source)
	: Command(source) {

}

WriteBasicCommand::~WriteBasicCommand() {

}

WriteBasicCommand& WriteBasicCommand::operator=(const WriteBasicCommand& source) {
	Command::operator=(source);

	return *this;
}

void WriteBasicCommand::Execute() {
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

string WriteBasicCommand::GetType() {
	return "WriteBasic";
}

Command* WriteBasicCommand::Clone() {
	return new WriteBasicCommand(*this);
}

//ImeCompositionBasicCommand
ImeCompositionBasicCommand::ImeCompositionBasicCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

ImeCompositionBasicCommand::ImeCompositionBasicCommand(const ImeCompositionBasicCommand& source)
	: Command(source) {

}

ImeCompositionBasicCommand::~ImeCompositionBasicCommand() {

}

ImeCompositionBasicCommand& ImeCompositionBasicCommand::operator=(const ImeCompositionBasicCommand& source) {
	Command::operator=(source);

	return *this;
}

void ImeCompositionBasicCommand::Execute() {
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

string ImeCompositionBasicCommand::GetType() {
	return "ImeCompositionBasic";
}

Command* ImeCompositionBasicCommand::Clone() {
	return new ImeCompositionBasicCommand(*this);
}

//ImeCharBasicCommand
ImeCharBasicCommand::ImeCharBasicCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

ImeCharBasicCommand::ImeCharBasicCommand(const ImeCharBasicCommand& source)
	: Command(source) {

}

ImeCharBasicCommand::~ImeCharBasicCommand() {

}

ImeCharBasicCommand& ImeCharBasicCommand::operator=(const ImeCharBasicCommand& source) {
	Command::operator=(source);

	return *this;
}

void ImeCharBasicCommand::Execute() {
	TCHAR buffer[2];
	buffer[0] = this->textEditingForm->GetCurrentBuffer()[0];
	buffer[1] = this->textEditingForm->GetCurrentBuffer()[1];
	Long column = this->textEditingForm->current->GetCurrent();

	if (this->textEditingForm->GetIsComposing() == TRUE) {
		this->textEditingForm->current->Remove(column);
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

string ImeCharBasicCommand::GetType() {
	return "ImeCharBasic";
}

Command* ImeCharBasicCommand::Clone() {
	return new ImeCharBasicCommand(*this);
}

//DeleteBasicCommand
DeleteBasicCommand::DeleteBasicCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

DeleteBasicCommand::DeleteBasicCommand(const DeleteBasicCommand& source)
	: Command(source) {

}

DeleteBasicCommand::~DeleteBasicCommand() {

}

DeleteBasicCommand& DeleteBasicCommand::operator=(const DeleteBasicCommand& source) {
	Command::operator=(source);

	return *this;
}

void DeleteBasicCommand::Execute() {
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

string DeleteBasicCommand::GetType() {
	return "DeleteBasic";
}

Command* DeleteBasicCommand::Clone() {
	return new DeleteBasicCommand(*this);
}

//CopyBasicCommand
CopyBasicCommand::CopyBasicCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

CopyBasicCommand::CopyBasicCommand(const CopyBasicCommand& source)
	: Command(source) {

}

CopyBasicCommand::~CopyBasicCommand() {

}

CopyBasicCommand& CopyBasicCommand::operator=(const CopyBasicCommand& source) {
	Command::operator=(source);

	return *this;
}

void CopyBasicCommand::Execute() {
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

string CopyBasicCommand::GetType() {
	return "CopyBasic";
}

Command* CopyBasicCommand::Clone() {
	return new CopyBasicCommand(*this);
}

//DeleteSelectionBasicCommand
DeleteSelectionBasicCommand::DeleteSelectionBasicCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {

}

DeleteSelectionBasicCommand::DeleteSelectionBasicCommand(const DeleteSelectionBasicCommand& source)
	: Command(source) {

}

DeleteSelectionBasicCommand::~DeleteSelectionBasicCommand() {

}

DeleteSelectionBasicCommand& DeleteSelectionBasicCommand::operator=(const DeleteSelectionBasicCommand& source) {
	Command::operator=(source);

	return *this;
}

void DeleteSelectionBasicCommand::Execute() {
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

string DeleteSelectionBasicCommand::GetType() {
	return "DeleteSelectionBasic";
}

Command* DeleteSelectionBasicCommand::Clone() {
	return new DeleteSelectionBasicCommand(*this);
}
//////////////////// Basic ////////////////////

//////////////////// Main ////////////////////
//WriteCommand
WriteCommand::WriteCommand(TextEditingForm * textEditingForm)
	: Command(textEditingForm) {
	this->nChar = -1;
	this->row = -1;
	this->column = -1;
}

WriteCommand::WriteCommand(const WriteCommand& source)
	: Command(source) {
	this->nChar = source.nChar;
	this->row = source.row;
	this->column = source.column;
}

WriteCommand::~WriteCommand() {

}

WriteCommand& WriteCommand::operator=(const WriteCommand& source) {
	Command::operator=(source);
	this->nChar = source.nChar;
	this->row = source.row;
	this->column = source.column;

	return *this;
}

void WriteCommand::Execute() {
	if (this->nChar == -1 && this->row == -1 && this->column == -1) {
		this->nChar = this->textEditingForm->GetCurrentCharacter();
		this->row = this->textEditingForm->note->GetCurrent();
		this->column = this->textEditingForm->current->GetCurrent();
	}
	else {
		this->textEditingForm->SetCurrentCharacter(this->nChar);
	}

	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long distance;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		distance = dummyManager->CountDistance(this->row, this->column);
		this->row = dummyManager->Unfold(this->row);
		dummyManager->CountIndex(distance, &this->row, &this->column);
	}

	this->textEditingForm->note->Move(this->row);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->row);
	this->textEditingForm->current->Move(this->column);
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BASIC_WRITE, 0));

	//========== 자동 개행 처리 2 ==========
	Long rowIndex;
	Long columnIndex;
	if (dummyManager != NULL) {
		Long lastFoldedRow = dummyManager->Fold(this->row);
		if (this->nChar == VK_RETURN) {
			dummyManager->Fold(lastFoldedRow + 1);
		}
		dummyManager->CountIndex(distance + 1, &rowIndex, &columnIndex);

		delete dummyManager;

		this->textEditingForm->note->Move(rowIndex);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(rowIndex);
		this->textEditingForm->current->Move(columnIndex);
	}
	//========== 자동 개행 처리 2 ==========
}

void WriteCommand::Unexecute() {
	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long distance;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		Long unfoldedRow = dummyManager->Unfold(this->row);
		if (this->nChar == VK_RETURN) {
			dummyManager->Unfold(unfoldedRow + 1);
		}
	}
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->note->Move(this->row);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->row);
	this->textEditingForm->current->Move(this->column);

	Glyph* index;
	if (this->nChar >= 32 || this->nChar == VK_TAB) {
		this->textEditingForm->current->Remove(this->column);
	}
	else if (this->nChar == VK_RETURN) {
		index = this->textEditingForm->note->GetAt(this->textEditingForm->note->GetCurrent() + 1);
		this->textEditingForm->current->Combine(index);
		this->textEditingForm->note->Remove(this->textEditingForm->note->GetCurrent() + 1);
		this->textEditingForm->current->Move(this->column);
	}
	if (this->textEditingForm->selection != NULL) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
		this->textEditingForm->note->UnselectAll();
	}

	//========== 자동 개행 처리 2 ==========
	Long rowIndex;
	Long columnIndex;
	if (dummyManager != NULL) {
		dummyManager->Fold(this->row);
		distance = dummyManager->CountDistance(this->row, this->column);
		dummyManager->CountIndex(distance, &rowIndex, &columnIndex);
		delete dummyManager;

		this->textEditingForm->note->Move(rowIndex);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(rowIndex);
		this->textEditingForm->current->Move(columnIndex);
	}
	//========== 자동 개행 처리 2 ==========
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

	Long row = this->textEditingForm->note->GetCurrent();
	Long column = this->textEditingForm->current->GetCurrent();
	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long distance = 0;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());

		distance = dummyManager->CountDistance(row, column);
		row = dummyManager->Unfold(row);
		dummyManager->CountIndex(distance, &row, &column);
	}
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->note->Move(row);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
	this->textEditingForm->current->Move(column);

	if (this->textEditingForm->GetIsComposing() == TRUE) {
		distance--; //자동개행 추가
	}

	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BASIC_IMECOMPOSITION, 0));

	//========== 자동 개행 처리 2 ==========
	Long rowIndex;
	Long columnIndex;
	if (dummyManager != NULL) {
		dummyManager->Fold(row);
		dummyManager->CountIndex(distance + 1, &rowIndex, &columnIndex);
		delete dummyManager;

		this->textEditingForm->note->Move(rowIndex);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(rowIndex);
		this->textEditingForm->current->Move(columnIndex);
	}
	//========== 자동 개행 처리 2 ==========
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
	this->buffer = new TCHAR[2];
	this->buffer[0] = '\0';
	this->buffer[1] = '\0';
	this->row = -1;
	this->column = -1;
}

ImeCharCommand::ImeCharCommand(const ImeCharCommand& source)
	: Command(source) {
	this->buffer = new TCHAR[2];
	this->buffer[0] = source.buffer[0];
	this->buffer[1] = source.buffer[1];
	this->row = source.row;
	this->column = source.column;
}

ImeCharCommand::~ImeCharCommand() {
	if (this->buffer != 0) {
		delete[] this->buffer;
	}
}

ImeCharCommand& ImeCharCommand::operator=(const ImeCharCommand& source) {
	Command::operator=(source);
	if (this->buffer != 0) {
		delete[] this->buffer;
	}
	this->buffer = new TCHAR[2];
	this->buffer[0] = source.buffer[0];
	this->buffer[1] = source.buffer[1];
	this->row = source.row;
	this->column = source.column;

	return *this;
}

void ImeCharCommand::Execute() {
	bool needtoRemove = false;
	if (this->buffer[0] == '\0' && this->row == -1 && this->column == -1) {
		this->buffer[0] = this->textEditingForm->GetCurrentBuffer()[0];
		this->buffer[1] = this->textEditingForm->GetCurrentBuffer()[1];
		this->row = this->textEditingForm->note->GetCurrent();
		this->column = this->textEditingForm->current->GetCurrent();
		if (this->textEditingForm->GetIsComposing() == TRUE) {
			needtoRemove = true;
		}
	}
	else {
		this->textEditingForm->SetCurrentBuffer(this->buffer);
	}

	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long distance = 0;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		distance = dummyManager->CountDistance(this->row, this->column);
		this->row = dummyManager->Unfold(this->row);
		dummyManager->CountIndex(distance, &this->row, &this->column);
	}
	//========== 자동 개행 처리 1 ==========

	if (needtoRemove == true) {
		this->column--;
	}
	else {
		distance++;
	}

	this->textEditingForm->note->Move(this->row);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->row);
	this->textEditingForm->current->Move(this->column);

	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BASIC_IMECHAR, 0));

	//========== 자동 개행 처리 2 ==========
	Long rowIndex;
	Long columnIndex;
	if (dummyManager != NULL) {
		dummyManager->Fold(this->row);
		dummyManager->CountIndex(distance, &rowIndex, &columnIndex);
		delete dummyManager;

		this->textEditingForm->note->Move(rowIndex);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(rowIndex);
		this->textEditingForm->current->Move(columnIndex);
	}
	//========== 자동 개행 처리 2 ==========
}

void ImeCharCommand::Unexecute() {
	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long distance;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		dummyManager->Unfold(this->row);
	}
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->note->Move(this->row);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->row);
	this->textEditingForm->current->Move(this->column);

	this->textEditingForm->current->Remove(this->column);

	//========== 자동 개행 처리 2 ==========
	Long rowIndex;
	Long columnIndex;
	if (dummyManager != NULL) {
		dummyManager->Fold(this->row);
		distance = dummyManager->CountDistance(this->row, this->column);
		dummyManager->CountIndex(distance, &rowIndex, &columnIndex);
		delete dummyManager;

		this->textEditingForm->note->Move(rowIndex);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(rowIndex);
		this->textEditingForm->current->Move(columnIndex);
	}
	//========== 자동 개행 처리 2 ==========

	if (this->textEditingForm->selection != NULL) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
		this->textEditingForm->note->UnselectAll();
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
	this->row = -1;
	this->noteLength = -1;
	this->column = -1;
	this->lineLength = -1;
	this->character = 0;
}

DeleteCommand::DeleteCommand(const DeleteCommand& source)
	: Command(source) {
	this->row = source.row;
	this->noteLength = source.noteLength;
	this->column = source.column;
	this->lineLength = source.lineLength;
	this->character = 0;
	if (source.character != 0) {
		this->character = source.character->Clone();
	}
}

DeleteCommand::~DeleteCommand() {
	if (this->character != NULL) {
		delete this->character;
	}
}

DeleteCommand& DeleteCommand::operator=(const DeleteCommand& source) {
	Command::operator=(source);
	this->row = source.row;
	this->noteLength = source.noteLength;
	this->column = source.column;
	this->lineLength = source.lineLength;
	this->character = 0;
	if (source.character != 0) {
		this->character = source.character->Clone();
	}

	return *this;
}

void DeleteCommand::Execute() {
	if (this->row == -1 && this->noteLength == -1 && this->column == -1 && this->lineLength == -1 && this->character == 0) {
		this->row = this->textEditingForm->note->GetCurrent();
		this->noteLength = this->textEditingForm->note->GetLength();
		this->column = this->textEditingForm->current->GetCurrent();
		this->lineLength = this->textEditingForm->current->GetLength();
		this->character = 0;
		if (this->column < this->textEditingForm->current->GetLength()) {
			this->character = this->textEditingForm->current->GetAt(this->column)->Clone();
		}
	}

	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long distance;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		distance = dummyManager->CountDistance(this->row, this->column);
		this->row = dummyManager->Unfold(this->row);
		dummyManager->CountIndex(distance, &this->row, &this->column);
		if (this->character == 0 && this->column >= this->textEditingForm->note->GetAt(this->row)->GetLength()
			&& this->row + 1 < this->textEditingForm->note->GetLength() - 1) {
			dummyManager->Unfold(this->row + 1);
		}
		this->noteLength = this->textEditingForm->note->GetLength();
		this->lineLength = this->textEditingForm->note->GetAt(this->row)->GetLength();
		if (this->column < this->lineLength) {
			this->character = this->textEditingForm->note->GetAt(this->row)->GetAt(this->column)->Clone();
		}
	}
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->note->Move(this->row);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->row);
	this->textEditingForm->current->Move(this->column);

	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BASIC_DELETE, 0));

	//========== 자동 개행 처리 2 ==========
	Long rowIndex;
	Long columnIndex;
	if (dummyManager != NULL) {
		dummyManager->Fold(this->row);
		dummyManager->CountIndex(distance, &rowIndex, &columnIndex);
		delete dummyManager;

		this->textEditingForm->note->Move(rowIndex);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(rowIndex);
		this->textEditingForm->current->Move(columnIndex);
	}
	//========== 자동 개행 처리 2 ==========
}

void DeleteCommand::Unexecute() {
	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long distance;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		dummyManager->Unfold(this->row);
	}
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->note->Move(this->row);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->row);
	this->textEditingForm->current->Move(this->column);

	Glyph* line;
	if (this->column < this->lineLength) {
		this->character->Select(false);
		this->textEditingForm->current->Add(this->column, this->character->Clone());
	}
	else if (this->column >= this->lineLength && this->row < this->noteLength - 1) {
		if (this->character == 0) {
			line = this->textEditingForm->current->Divide(this->column);
			this->textEditingForm->note->Add(this->row + 1, line);
		}
		else {
			this->character->Select(false);
			this->textEditingForm->current->Add(this->column, this->character->Clone());
		}
	}
	this->textEditingForm->note->Move(this->row);
	this->textEditingForm->current->Move(this->column);

	//========== 자동 개행 처리 2 ==========
	Long rowIndex;
	Long columnIndex;
	if (dummyManager != NULL) {
		Long lastFoldedRow = dummyManager->Fold(this->row);
		if (this->character == 0) {
			dummyManager->Fold(lastFoldedRow + 1);
		}

		distance = dummyManager->CountDistance(this->row, this->column);
		dummyManager->CountIndex(distance, &rowIndex, &columnIndex);
		delete dummyManager;

		this->textEditingForm->note->Move(rowIndex);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(rowIndex);
		this->textEditingForm->current->Move(columnIndex);
	}
	//========== 자동 개행 처리 2 ==========
}

string DeleteCommand::GetType() {
	return "Delete";
}

Command* DeleteCommand::Clone() {
	return new DeleteCommand(*this);
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
	Long row = this->textEditingForm->note->GetCurrent();
	Long column = this->textEditingForm->current->GetCurrent();
	Long originStart = this->textEditingForm->selection->GetStart();
	Long originEnd = this->textEditingForm->selection->GetEnd();
	Long start = originStart;
	Long end = originEnd;
	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		dummyManager->Unfold(&start, &end);

		if (this->textEditingForm->selection != NULL) {
			delete this->textEditingForm->selection;
			this->textEditingForm->selection = NULL;
		}
		this->textEditingForm->selection = new Selection(start, end);
	}
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BASIC_COPY, 0));

	//========== 자동 개행 처리 2 ==========
	if (dummyManager != NULL) {
		Long lastFoldedRow;
		Long i = start;
		while (i <= end && end < this->textEditingForm->note->GetLength()) {
			lastFoldedRow = dummyManager->Fold(i);
			end += lastFoldedRow - i;
			i = lastFoldedRow + 1;
		}
		delete dummyManager;

		if (this->textEditingForm->selection != NULL) {
			delete this->textEditingForm->selection;
			this->textEditingForm->selection = NULL;
		}
		this->textEditingForm->selection = new Selection(originStart, originEnd);

		this->textEditingForm->note->Move(row);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(row);
		this->textEditingForm->current->Move(column);
	}
	//========== 자동 개행 처리 2 ==========
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
	this->startRow = -1;
	this->startColumn = -1;
	this->endRow = -1;
	this->endColumn = -1;
	this->selecteds = "";
}

DeleteSelectionCommand::DeleteSelectionCommand(const DeleteSelectionCommand& source)
	: Command(source) {
	this->startRow = source.startRow;
	this->startColumn = source.startColumn;
	this->endRow = source.endRow;
	this->endColumn = source.endColumn;
	this->selecteds = source.selecteds;
}

DeleteSelectionCommand::~DeleteSelectionCommand() {

}

DeleteSelectionCommand& DeleteSelectionCommand::operator=(const DeleteSelectionCommand& source) {
	Command::operator=(source);
	this->startRow = source.startRow;
	this->startColumn = source.startColumn;
	this->endRow = source.endRow;
	this->endColumn = source.endColumn;
	this->selecteds = source.selecteds;

	return *this;
}

void DeleteSelectionCommand::Execute() {
	//startRow, startColumn, endRow, endColumn, selecteds(string)
	if (this->startRow == -1 && this->startColumn == -1 && this->endRow == -1 && this->endColumn == -1 && this->selecteds == "") {
		this->startRow = this->textEditingForm->selection->GetStart();
		this->endRow = this->textEditingForm->selection->GetEnd();
		Glyph* character;
		Glyph* line;

		bool isSelected = false;
		line = this->textEditingForm->note->GetAt(this->startRow); //시작 행
		Long i = 0;
		while (i < line->GetLength() && isSelected == false) { //시작 행의 개수만큼 그리고 현재 글자가 선택되어있지 않은 동안 반복하다.
			character = line->GetAt(i); //시작 행에서 글자를 가져오다.
			isSelected = character->GetIsSelected(); //현재 글자의 선택여부를 확인하다.
			i++;
		}
		this->startColumn = i - 1;
		if (isSelected == false) {
			this->startColumn++;
		}

		isSelected = true;
		line = this->textEditingForm->note->GetAt(this->endRow); //끝 행
		i = 0;
		if (this->startRow == this->endRow) {
			i = this->startColumn;
		}
		while (i < line->GetLength() && isSelected == true) { //끝 행의 개수만큼 그리고 현재 글자가 선택되어있는 동안 반복하다.
			character = line->GetAt(i); //끝 행에서 글자를 가져오다.
			isSelected = character->GetIsSelected(); //현재 글자의 선택여부를 확인하다.
			i++;
		}
		this->endColumn = i - 1;
		if (isSelected == true) {
			this->endColumn++;
		}
		this->selecteds = this->textEditingForm->note->GetContent(this->startRow, this->startColumn, this->endRow, this->endColumn);
	}

	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long startDistance;
	Long endDistance;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		startDistance = dummyManager->CountDistance(this->startRow, this->startColumn);
		endDistance = dummyManager->CountDistance(this->endRow, this->endColumn);
		dummyManager->Unfold(&this->startRow, &this->endRow);
		dummyManager->CountIndex(startDistance, &this->startRow, &this->startColumn); //reexecute에서는 필요없음.
		dummyManager->CountIndex(endDistance, &this->endRow, &this->endColumn); //reexecute에서는 필요없음.

		this->selecteds = this->textEditingForm->note->GetContent(this->startRow, this->startColumn, this->endRow, this->endColumn);
	}
	//========== 자동 개행 처리 1 ==========

	if (this->textEditingForm->selection != NULL) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
		this->textEditingForm->note->UnselectAll();
	}

	this->textEditingForm->selection = new Selection(this->startRow, this->endRow);
	this->textEditingForm->note->Select(this->startRow, this->startColumn, this->endRow, this->endColumn);

	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_BASIC_DELETESELECTION, 0));

	//========== 자동 개행 처리 2 ==========
	Long rowIndex;
	Long columnIndex;
	if (dummyManager != NULL) {
		dummyManager->Fold(this->startRow);
		dummyManager->CountIndex(startDistance, &rowIndex, &columnIndex);
		delete dummyManager;

		this->textEditingForm->note->Move(rowIndex);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(rowIndex);
		this->textEditingForm->current->Move(columnIndex);
	}
	//========== 자동 개행 처리 2 ==========
}

void DeleteSelectionCommand::Unexecute() {
	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		dummyManager->Unfold(this->startRow);
	}
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->note->Move(this->startRow);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->startRow);
	this->textEditingForm->current->Move(this->startColumn);

	GlyphFactory glyphFactory;
	Scanner scanner(this->selecteds);
	while (scanner.IsEnd() == false) {
		string token = scanner.GetToken();
		Glyph* glyph = glyphFactory.Make(token.c_str());
		Long row = this->textEditingForm->note->GetCurrent();
		Long column = this->textEditingForm->current->GetCurrent();
		if (token != "\n") {
			if (column >= this->textEditingForm->current->GetLength()) {
				this->textEditingForm->current->Add(glyph);
			}
			else {
				this->textEditingForm->current->Add(column, glyph);
			}
		}
		else {
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
		scanner.Next();
	}

	//========== 자동 개행 처리 2 ==========
	Long startDistance;
	Long endDistance;
	if (dummyManager != NULL) {
		startDistance = dummyManager->CountDistance(this->startRow, this->startColumn);
		endDistance = dummyManager->CountDistance(this->endRow, this->endColumn);
		Long end = this->endRow;
		Long lastFoldedRow;
		Long i = this->startRow;
		while (i <= end && end < this->textEditingForm->note->GetLength()) {
			lastFoldedRow = dummyManager->Fold(i);
			end += lastFoldedRow - i;
			i = lastFoldedRow + 1;
		}
		dummyManager->CountIndex(startDistance, &this->startRow, &this->startColumn);
		dummyManager->CountIndex(endDistance, &this->endRow, &this->endColumn);
		delete dummyManager;

		if (this->textEditingForm->selection != 0) {
			delete this->textEditingForm->selection;
			this->textEditingForm->selection = 0;
			this->textEditingForm->note->UnselectAll();
		}
	}
	//========== 자동 개행 처리 2 ==========

	this->textEditingForm->note->Select(this->startRow, this->startColumn, this->endRow, this->endColumn);
	this->textEditingForm->selection = new Selection(this->startRow, this->endRow);

	this->textEditingForm->note->Move(this->endRow);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->endRow);
	this->textEditingForm->current->Move(this->endColumn);
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

void CutCommand::Execute() {
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
	this->startRow = -1;
	this->startColumn = -1;
	this->endRow = -1;
	this->endColumn = -1;
	this->pasteds = "";
}

PasteCommand::PasteCommand(const PasteCommand& source)
	: Command(source) {
	this->startRow = source.startRow;
	this->startColumn = source.startColumn;
	this->endRow = source.endRow;
	this->endColumn = source.endColumn;
	this->pasteds = source.pasteds;
}

PasteCommand::~PasteCommand() {

}

PasteCommand& PasteCommand::operator=(const PasteCommand& source) {
	Command::operator=(source);
	this->startRow = source.startRow;
	this->startColumn = source.startColumn;
	this->endRow = source.endRow;
	this->endColumn = source.endColumn;
	this->pasteds = source.pasteds;

	return *this;
}

void PasteCommand::Execute() {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->SetIsDeleteSelectionByInput(TRUE);
		this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETESELECTION, 0));
		this->textEditingForm->SetIsDeleteSelectionByInput(FALSE);
	}

	//정보 저장
	if (this->startRow == -1 && this->startColumn == -1 && this->pasteds == "") {
		this->startRow = this->textEditingForm->note->GetCurrent();
		this->startColumn = this->textEditingForm->current->GetCurrent();
		HANDLE handle;
		LPSTR address = NULL;
		if (::IsClipboardFormatAvailable(CF_TEXT) != FALSE) {
			if (::OpenClipboard(this->textEditingForm->m_hWnd)) {
				handle = GetClipboardData(CF_TEXT);
				if (handle != NULL) {
					address = (LPSTR)::GlobalLock(handle);
					this->pasteds = address;
					::GlobalUnlock(handle);
				}
				CloseClipboard();
			}
		}
	}

	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long distance = 0;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		distance = dummyManager->CountDistance(this->startRow, this->startColumn);
		this->startRow = dummyManager->Unfold(this->startRow);
		dummyManager->CountIndex(distance, &this->startRow, &this->startColumn);
	}
	//========== 자동 개행 처리 1 ==========

	this->textEditingForm->note->Move(this->startRow);
	this->textEditingForm->current = this->textEditingForm->note->GetAt(this->startRow);
	this->textEditingForm->current->Move(this->startColumn);

	//========== Paste 실제 처리 ==========
	//복사한 문자열을 임시 Note로 만들다.
	Scanner scanner(this->pasteds);
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

	//정보 저장
	if (this->endRow == -1 && this->endColumn == -1) {
		this->endRow = this->textEditingForm->note->GetCurrent();
		this->endColumn = this->textEditingForm->current->GetCurrent();
	}
	//========== Paste 실제 처리 ==========

	//========== 자동 개행 처리 2 ==========
	if (dummyManager != NULL) {
		distance = dummyManager->CountDistance(this->endRow, this->endColumn); //편 상태
		Long lastFoldedRow;
		Long end = this->endRow;
		Long i = this->startRow;
		while (i <= end) {
			lastFoldedRow = dummyManager->Fold(i);
			end += lastFoldedRow - i;
			i = lastFoldedRow + 1;
		}
		dummyManager->CountIndex(distance, &this->endRow, &this->endColumn); //접은 상태
		delete dummyManager;

		this->textEditingForm->note->Move(this->endRow);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(this->endRow);
		this->textEditingForm->current->Move(this->endColumn);
	}
	//========== 자동 개행 처리 2 ==========
}

void PasteCommand::Unexecute() {
	//========== 자동 개행 처리 1 ==========
	DummyManager* dummyManager = 0;
	Long startDistance;
	Long endDistance;
	if (this->textEditingForm->GetIsLockedHScroll() == TRUE) {
		CRect rect;
		this->textEditingForm->GetClientRect(rect);
		dummyManager = new DummyManager(this->textEditingForm->note, this->textEditingForm->characterMetrics, rect.Width());
		startDistance = dummyManager->CountDistance(this->startRow, this->startColumn);
		endDistance = dummyManager->CountDistance(this->endRow, this->endColumn); //접은 상태
		dummyManager->Unfold(&this->startRow, &this->endRow); //편 상태
		dummyManager->CountIndex(startDistance, &this->startRow, &this->startColumn);
		dummyManager->CountIndex(endDistance, &this->endRow, &this->endColumn); //편 상태
	}
	//========== 자동 개행 처리 1 ==========

	Long i;
	Glyph* line;
	Long length;
	Long j;
	Long end = this->endRow;
	i = this->startRow;
	while (i <= end) {
		line = this->textEditingForm->note->GetAt(i);
		j = this->startColumn;
		length = line->GetLength();
		if (i == end) {
			length = j + this->endColumn;
		}
		while (j < length) {
			line->Remove(j--);
			length--;
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

	//========== 자동 개행 처리 2 ==========
	if (dummyManager != NULL) {
		dummyManager->Fold(this->startRow);
		dummyManager->CountIndex(startDistance, &this->startRow, &this->startColumn);
		delete dummyManager;

		this->textEditingForm->note->Move(this->startRow);
		this->textEditingForm->current = this->textEditingForm->note->GetAt(this->startRow);
		this->textEditingForm->current->Move(this->startColumn);
	}
	//========== 자동 개행 처리 2 ==========

	if (this->textEditingForm->selection != 0) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = 0;
		this->textEditingForm->note->UnselectAll();
	}
}

string PasteCommand::GetType() {
	return "Paste";
}

Command* PasteCommand::Clone() {
	return new PasteCommand(*this);
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

//UndoCommand
UndoCommand::UndoCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

UndoCommand::UndoCommand(const UndoCommand& source)
	: Command(source) {
}

UndoCommand::~UndoCommand() {

}

UndoCommand& UndoCommand::operator=(const UndoCommand& source) {
	Command::operator=(source);

	return *this;
}

void UndoCommand::Execute() {
	if (this->textEditingForm->undoHistoryBook->GetLength() > 0) {
		Command* command = this->textEditingForm->undoHistoryBook->OpenAt();
		command->Unexecute();

		this->textEditingForm->redoHistoryBook->Write(command->Clone());
		this->textEditingForm->undoHistoryBook->Erase();
	}
}

string UndoCommand::GetType() {
	return "Undo";
}

Command* UndoCommand::Clone() {
	return new UndoCommand(*this);
}

//RedoCommand
RedoCommand::RedoCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

RedoCommand::RedoCommand(const RedoCommand& source)
	: Command(source) {
}

RedoCommand::~RedoCommand() {

}

RedoCommand& RedoCommand::operator=(const RedoCommand& source) {
	Command::operator=(source);

	return *this;
}

void RedoCommand::Execute() {
	if (this->textEditingForm->redoHistoryBook->GetLength() > 0) {
		Command* command = this->textEditingForm->redoHistoryBook->OpenAt();
		command->Execute();

		this->textEditingForm->undoHistoryBook->Write(command->Clone());
		this->textEditingForm->redoHistoryBook->Erase();
	}
	if (this->textEditingForm->selection != NULL) {
		delete this->textEditingForm->selection;
		this->textEditingForm->selection = NULL;
		this->textEditingForm->note->UnselectAll();
	}
}

string RedoCommand::GetType() {
	return "Redo";
}

Command* RedoCommand::Clone() {
	return new RedoCommand(*this);
}

//FindCommand
FindCommand::FindCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

FindCommand::FindCommand(const FindCommand& source)
	: Command(source) {
}

FindCommand::~FindCommand() {

}

FindCommand& FindCommand::operator=(const FindCommand& source) {
	Command::operator=(source);

	return *this;
}

void FindCommand::Execute() {
	if (this->textEditingForm->GetIsUnlockedFindReplaceDialog() == TRUE && this->textEditingForm->findReplaceDialog == NULL) {
		string selectedContent = "";
		if (this->textEditingForm->selection != NULL) {
			selectedContent = textEditingForm->note->GetSelectedContent
			(this->textEditingForm->selection->GetStart(), this->textEditingForm->selection->GetEnd());
		}
		this->textEditingForm->findReplaceDialog = new FindReplaceDialog(TRUE, selectedContent, this->textEditingForm);
		this->textEditingForm->findReplaceDialog->SetActiveWindow();
		this->textEditingForm->findReplaceDialog->ShowWindow(TRUE);
	}
}

string FindCommand::GetType() {
	return "Find";
}

Command* FindCommand::Clone() {
	return new FindCommand(*this);
}

//ReplaceCommand
ReplaceCommand::ReplaceCommand(TextEditingForm* textEditingForm)
	: Command(textEditingForm) {
}

ReplaceCommand::ReplaceCommand(const ReplaceCommand& source)
	: Command(source) {
}

ReplaceCommand::~ReplaceCommand() {

}

ReplaceCommand& ReplaceCommand::operator=(const ReplaceCommand& source) {
	Command::operator=(source);

	return *this;
}

void ReplaceCommand::Execute() {
	if (this->textEditingForm->GetIsUnlockedFindReplaceDialog() == TRUE && this->textEditingForm->findReplaceDialog == NULL) {
		string selectedContent = "";
		if (this->textEditingForm->selection != NULL) {
			selectedContent = textEditingForm->note->GetSelectedContent
			(this->textEditingForm->selection->GetStart(), this->textEditingForm->selection->GetEnd());
		}
		this->textEditingForm->findReplaceDialog = new FindReplaceDialog(FALSE, selectedContent, this->textEditingForm);
		this->textEditingForm->findReplaceDialog->SetActiveWindow();
		this->textEditingForm->findReplaceDialog->ShowWindow(TRUE);
	}
}

string ReplaceCommand::GetType() {
	return "Replace";
}

Command* ReplaceCommand::Clone() {
	return new ReplaceCommand(*this);
}
//////////////////// Main ////////////////////

//////////////////// Move ////////////////////
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
//////////////////// Move ////////////////////

//////////////////// Select ////////////////////
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
//////////////////// Select ////////////////////