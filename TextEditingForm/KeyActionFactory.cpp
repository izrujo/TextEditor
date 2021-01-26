#include "KeyActionFactory.h"
#include "KeyActions.h"
#include "TextEditingForm.h"
#include <WinUser.h>

KeyActionFactory::KeyActionFactory(TextEditingForm *textEditingForm) {
	this->textEditingForm = textEditingForm;
}

KeyActionFactory::KeyActionFactory(const KeyActionFactory& source) {
	this->textEditingForm = source.textEditingForm;
}

KeyActionFactory::~KeyActionFactory() {

}

KeyActionFactory& KeyActionFactory::operator=(const KeyActionFactory& source) {
	this->textEditingForm = source.textEditingForm;

	return *this;
}

KeyAction* KeyActionFactory::Make(UINT nChar) {
	KeyAction* keyAction = 0;

	SHORT isCtrl = GetKeyState(VK_CONTROL) & 0X8000;
	SHORT isShift = GetKeyState(VK_SHIFT) & 0X8000;

	//Shift + Ctrl + KEY
	if (isShift && isCtrl && nChar == VK_LEFT) {
		keyAction = new ShiftCtrlLeftKeyAction(this->textEditingForm);
	}
	else if (isShift && isCtrl && nChar == VK_RIGHT) {
		keyAction = new ShiftCtrlRightKeyAction(this->textEditingForm);
	}
	else if (isShift && isCtrl && nChar == VK_HOME) {
		keyAction = new ShiftCtrlHomeKeyAction(this->textEditingForm);
	}
	else if (isShift && isCtrl && nChar == VK_END) {
		keyAction = new ShiftCtrlEndKeyAction(this->textEditingForm);
	}
	//Shift + KEY
	else if (isShift && nChar == VK_LEFT) {
		keyAction = new ShiftLeftKeyAction(this->textEditingForm);
	}
	else if (isShift && nChar == VK_RIGHT) {
		keyAction = new ShiftRightKeyAction(this->textEditingForm);
	}
	else if (isShift && nChar == VK_UP) {
		keyAction = new ShiftUpKeyAction(this->textEditingForm);
	}
	else if (isShift && nChar == VK_DOWN) {
		keyAction = new ShiftDownKeyAction(this->textEditingForm);
	}
	else if (isShift && nChar == VK_HOME) {
		keyAction = new ShiftHomeKeyAction(this->textEditingForm);
	}
	else if (isShift && nChar == VK_END) {
		keyAction = new ShiftEndKeyAction(this->textEditingForm);
	}
	//Ctrl + KEY
	else if (isCtrl && nChar == VK_LEFT) {
		keyAction = new CtrlLeftKeyAction(this->textEditingForm);
	}
	else if (isCtrl && nChar == VK_RIGHT) {
		keyAction = new CtrlRightKeyAction(this->textEditingForm);
	}
	else if (isCtrl && nChar == VK_HOME) {
		keyAction = new CtrlHomeKeyAction(this->textEditingForm);
	}
	else if (isCtrl && nChar == VK_END) {
		keyAction = new CtrlEndKeyAction(this->textEditingForm);
	}
	else if (isCtrl && nChar == 0x41) {
		keyAction = new CtrlAKeyAction(this->textEditingForm);
	}
	else if (isCtrl && nChar == 0x43) {
		keyAction = new CtrlCKeyAction(this->textEditingForm);
	}
	else if (isCtrl && nChar == 0x56) {
		keyAction = new CtrlVKeyAction(this->textEditingForm);
	}
	else if (isCtrl && nChar == 0x58) {
		keyAction = new CtrlXKeyAction(this->textEditingForm);
	}
	//KEY
	else if (nChar == VK_LEFT) {
		keyAction = new LeftKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_RIGHT) {
		keyAction = new RightKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_UP) {
		keyAction = new UpKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_DOWN) {
		keyAction = new DownKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_HOME) {
		keyAction = new HomeKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_END) {
		keyAction = new EndKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_DELETE) {
		keyAction = new DeleteKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_BACK) {
		keyAction = new BackSpaceKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_PRIOR) {
		keyAction = new PageUpKeyAction(this->textEditingForm);
	}
	else if (nChar == VK_NEXT) {
		keyAction = new PageDownKeyAction(this->textEditingForm);
	}

	return keyAction;
}