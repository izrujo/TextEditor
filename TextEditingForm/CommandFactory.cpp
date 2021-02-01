#include "CommandFactory.h"
#include "Commands.h"
#include "TextEditingForm.h"
#include "resource.h"

CommandFactory::CommandFactory(TextEditingForm* textEditingForm) {
	this->textEditingForm = textEditingForm;
}

CommandFactory::CommandFactory(const CommandFactory& source) {
	this->textEditingForm = source.textEditingForm;
}

CommandFactory::~CommandFactory() {

}

CommandFactory& CommandFactory::operator=(const CommandFactory& source) {
	this->textEditingForm = source.textEditingForm;

	return *this;
}

Command* CommandFactory::Make(int uID) {
	Command* command = 0;
	
	//////////////////// Main ////////////////////
	if (uID == IDC_EDIT_WRITE) {
		command = new WriteCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_IMECOMPOSITION) {
		command = new ImeCompositionCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_IMECHAR) {
		command = new ImeCharCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_DELETE) {
		command = new DeleteCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_COPY) {
		command = new CopyCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_DELETESELECTION) {
		command = new DeleteSelectionCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_CUT) {
		command = new CutCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_PASTE) {
		command = new PasteCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_SELECTALL) {
		command = new SelectAllCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_UNDO) {
		command = new UndoCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_REDO) {
		command = new RedoCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_FIND) {
		command = new FindCommand(this->textEditingForm);
	}
	else if (uID == IDC_EDIT_REPLACE) {
		command = new ReplaceCommand(this->textEditingForm);
	}
	//////////////////// Main ////////////////////
	//////////////////// Basic ////////////////////
	else if (uID == IDC_BASIC_WRITE) {
		command = new WriteBasicCommand(this->textEditingForm);
	}
	else if (uID == IDC_BASIC_IMECOMPOSITION) {
		command = new ImeCompositionBasicCommand(this->textEditingForm);
	}
	else if (uID == IDC_BASIC_IMECHAR) {
		command = new ImeCharBasicCommand(this->textEditingForm);
	}
	else if (uID == IDC_BASIC_DELETE) {
		command = new DeleteBasicCommand(this->textEditingForm);
	}
	else if (uID == IDC_BASIC_COPY) {
		command = new CopyBasicCommand(this->textEditingForm);
	}
	else if (uID == IDC_BASIC_DELETESELECTION) {
		command = new DeleteSelectionBasicCommand(this->textEditingForm);
	}
	//////////////////// Basic ////////////////////
	//////////////////// Move ////////////////////
	else if (uID == IDC_MOVE_LEFT) {
		command = new LeftCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_RIGHT) {
		command = new RightCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_UP) {
		command = new UpCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_DOWN) {
		command = new DownCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_HOME) {
		command = new HomeCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_END) {
		command = new EndCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_CTRLLEFT) {
		command = new CtrlLeftCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_CTRLRIGHT) {
		command = new CtrlRightCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_CTRLHOME) {
		command = new CtrlHomeCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_CTRLEND) {
		command = new CtrlEndCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_PAGEUP) {
		command = new PageUpCommand(this->textEditingForm);
	}
	else if (uID == IDC_MOVE_PAGEDOWN) {
		command = new PageDownCommand(this->textEditingForm);
	}
	//////////////////// Move ////////////////////
	//////////////////// Select ////////////////////
	else if (uID == IDC_SELECTMOVE_LEFT) {
		command = new ShiftLeftCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_RIGHT) {
		command = new ShiftRightCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_UP) {
		command = new ShiftUpCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_DOWN) {
		command = new ShiftDownCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_HOME) {
		command = new ShiftHomeCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_END) {
		command = new ShiftEndCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_CTRLLEFT) {
		command = new ShiftCtrlLeftCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_CTRLRIGHT) {
		command = new ShiftCtrlRightCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_CTRLHOME) {
		command = new ShiftCtrlHomeCommand(this->textEditingForm);
	}
	else if (uID == IDC_SELECTMOVE_CTRLEND) {
		command = new ShiftCtrlEndCommand(this->textEditingForm);
	}
	//////////////////// Select ////////////////////

	return command;
}