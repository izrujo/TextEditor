#include "KeyActions.h"
#include "KeyActionFactory.h"
#include "TextEditingForm.h"
#include "Glyph.h"
#include "CharacterMetrics.h"
#include "Characters.h"
#include "Note.h"
#include "Line.h"
#include "Composite.h"
#include "ScrollController.h"
#include "CaretController.h"
#include "Scroll.h"
#include "Selection.h"

#include "resource.h"

#include <string>

//KeyAction
KeyAction::KeyAction(TextEditingForm* textEditingForm) {
	this->textEditingForm = textEditingForm;
}

KeyAction::KeyAction(const KeyAction& source) {
	this->textEditingForm = source.textEditingForm;
}

KeyAction::~KeyAction() {

}

KeyAction& KeyAction::operator=(const KeyAction& source) {
	this->textEditingForm = source.textEditingForm;

	return *this;
}

//LeftKeyAction
LeftKeyAction::LeftKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {

}

LeftKeyAction::LeftKeyAction(const LeftKeyAction& source)
	: KeyAction(source) {

}

LeftKeyAction::~LeftKeyAction() {

}

LeftKeyAction& LeftKeyAction::operator=(const LeftKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

void LeftKeyAction::OnKeyDown(UINT nChar, UINT nRepeatCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_LEFT, 0));
}

//RightKeyAction
RightKeyAction::RightKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

RightKeyAction::RightKeyAction(const RightKeyAction& source)
	: KeyAction(source) {
}

RightKeyAction::~RightKeyAction() {

}

RightKeyAction& RightKeyAction::operator=(const RightKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

void RightKeyAction::OnKeyDown(UINT nChar, UINT nRepeatCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_RIGHT, 0));
}

//UpKeyAction
UpKeyAction::UpKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {

}

UpKeyAction::UpKeyAction(const UpKeyAction& source)
	: KeyAction(source) {

}

UpKeyAction::~UpKeyAction() {

}

UpKeyAction& UpKeyAction::operator=(const UpKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

void UpKeyAction::OnKeyDown(UINT nChar, UINT nRepeatCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_UP, 0));
}

//DownKeyAction
DownKeyAction::DownKeyAction(TextEditingForm* textEditingForm)
	:KeyAction(textEditingForm) {

}

DownKeyAction::DownKeyAction(const DownKeyAction& source)
	: KeyAction(source) {

}

DownKeyAction::~DownKeyAction() {

}

DownKeyAction& DownKeyAction::operator=(const DownKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void DownKeyAction::OnKeyDown(UINT nChar, UINT nRepeatCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_DOWN, 0));
}

//HomeKeyAction
HomeKeyAction::HomeKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

HomeKeyAction::HomeKeyAction(const HomeKeyAction& source)
	: KeyAction(source) {
}

HomeKeyAction::~HomeKeyAction() {

}

HomeKeyAction& HomeKeyAction::operator=(const HomeKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void HomeKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_HOME, 0));
}

//EndKeyAction
EndKeyAction::EndKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

EndKeyAction::EndKeyAction(const EndKeyAction& source)
	: KeyAction(source) {
}

EndKeyAction::~EndKeyAction() {

}

EndKeyAction& EndKeyAction::operator=(const EndKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void EndKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_END, 0));
}

//CtrlLeftKeyAction
CtrlLeftKeyAction::CtrlLeftKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlLeftKeyAction::CtrlLeftKeyAction(const CtrlLeftKeyAction& source)
	: KeyAction(source) {
}

CtrlLeftKeyAction::~CtrlLeftKeyAction() {

}

CtrlLeftKeyAction& CtrlLeftKeyAction::operator=(const CtrlLeftKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void CtrlLeftKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_CTRLLEFT, 0));
}

//CtrlRightKeyAction
CtrlRightKeyAction::CtrlRightKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlRightKeyAction::CtrlRightKeyAction(const CtrlRightKeyAction& source)
	: KeyAction(source) {
}

CtrlRightKeyAction::~CtrlRightKeyAction() {

}

CtrlRightKeyAction& CtrlRightKeyAction::operator=(const CtrlRightKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void CtrlRightKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_CTRLRIGHT, 0));
}

//CtrlHomeKeyAction
CtrlHomeKeyAction::CtrlHomeKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlHomeKeyAction::CtrlHomeKeyAction(const CtrlHomeKeyAction& source)
	: KeyAction(source) {
}

CtrlHomeKeyAction::~CtrlHomeKeyAction() {

}

CtrlHomeKeyAction& CtrlHomeKeyAction::operator=(const CtrlHomeKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void CtrlHomeKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_CTRLHOME, 0));
}

//CtrlEndKeyAction
CtrlEndKeyAction::CtrlEndKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlEndKeyAction::CtrlEndKeyAction(const CtrlEndKeyAction& source)
	: KeyAction(source) {
}

CtrlEndKeyAction::~CtrlEndKeyAction() {

}

CtrlEndKeyAction& CtrlEndKeyAction::operator=(const CtrlEndKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void CtrlEndKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_CTRLEND, 0));
}

//DeleteKeyAction
DeleteKeyAction::DeleteKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

DeleteKeyAction::DeleteKeyAction(const DeleteKeyAction& source)
	: KeyAction(source) {
}

DeleteKeyAction::~DeleteKeyAction() {

}

DeleteKeyAction& DeleteKeyAction::operator=(const DeleteKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void DeleteKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (this->textEditingForm->selection == NULL) {
		this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETE, 0));
	}
	else {
		this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETESELECTION, 0));
	}
}

//BackSpaceKeyAction
BackSpaceKeyAction::BackSpaceKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

BackSpaceKeyAction::BackSpaceKeyAction(const BackSpaceKeyAction& source)
	: KeyAction(source) {
}

BackSpaceKeyAction::~BackSpaceKeyAction() {

}

BackSpaceKeyAction& BackSpaceKeyAction::operator=(const BackSpaceKeyAction& source) {
	KeyAction::operator=(source);

	return *this;
}

void BackSpaceKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (this->textEditingForm->selection == NULL) {
		if (this->textEditingForm->note->GetCurrent() > 0 || this->textEditingForm->current->GetCurrent() > 0) {
			this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_LEFT, 0));
			this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETE, 0));
		}
	}
	else {
		this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_DELETESELECTION, 0));
	}
}

//PageUpKeyAction
PageUpKeyAction::PageUpKeyAction(TextEditingForm* textEditingForm)
	:KeyAction(textEditingForm) {

}

PageUpKeyAction::PageUpKeyAction(const PageUpKeyAction& source)
	: KeyAction(source) {

}

PageUpKeyAction::~PageUpKeyAction() {

}

PageUpKeyAction& PageUpKeyAction::operator=(const PageUpKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

void PageUpKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_PAGEUP, 0));
}

//PageDownKeyAction
PageDownKeyAction::PageDownKeyAction(TextEditingForm* textEditingForm)
	:KeyAction(textEditingForm) {
}

PageDownKeyAction::PageDownKeyAction(const PageDownKeyAction& source)
	: KeyAction(source) {

}

PageDownKeyAction::~PageDownKeyAction() {

}

PageDownKeyAction& PageDownKeyAction::operator=(const PageDownKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

void PageDownKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_MOVE_PAGEDOWN, 0));
}

//ShiftLeftKeyAction
ShiftLeftKeyAction::ShiftLeftKeyAction(TextEditingForm* textEditingForm)
	:KeyAction(textEditingForm) {

}

ShiftLeftKeyAction::ShiftLeftKeyAction(const ShiftLeftKeyAction& source)
	: KeyAction(textEditingForm) {

}

ShiftLeftKeyAction::~ShiftLeftKeyAction() {

}

void ShiftLeftKeyAction::OnKeyDown(UINT nChar, UINT nRepeatCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_LEFT, 0));
}

ShiftLeftKeyAction& ShiftLeftKeyAction::operator = (const ShiftLeftKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

//ShiftRightKeyAction
ShiftRightKeyAction::ShiftRightKeyAction(TextEditingForm* textEditingForm)
	:KeyAction(textEditingForm) {
}

ShiftRightKeyAction::ShiftRightKeyAction(const ShiftRightKeyAction& source)
	: KeyAction(textEditingForm) {
}

ShiftRightKeyAction::~ShiftRightKeyAction() {
}

void ShiftRightKeyAction::OnKeyDown(UINT nChar, UINT nRepeatCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_RIGHT, 0));
}

ShiftRightKeyAction& ShiftRightKeyAction::operator=(const ShiftRightKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

//ShiftUpKeyAction
ShiftUpKeyAction::ShiftUpKeyAction(TextEditingForm* textEditingForm)
	:KeyAction(textEditingForm) {

}

ShiftUpKeyAction::ShiftUpKeyAction(const ShiftUpKeyAction& source)
	: KeyAction(source) {

}

ShiftUpKeyAction::~ShiftUpKeyAction() {

}

void ShiftUpKeyAction::OnKeyDown(UINT nChar, UINT nRepeatCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_UP, 0));
}

ShiftUpKeyAction& ShiftUpKeyAction::operator = (const ShiftUpKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

//ShiftDownKeyAction
ShiftDownKeyAction::ShiftDownKeyAction(TextEditingForm* textEditingForm)
	:KeyAction(textEditingForm) {

}

ShiftDownKeyAction::ShiftDownKeyAction(const ShiftDownKeyAction& source)
	: KeyAction(textEditingForm) {

}

ShiftDownKeyAction::~ShiftDownKeyAction() {

}

void ShiftDownKeyAction::OnKeyDown(UINT nChar, UINT nRepeatCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_DOWN, 0));
}

ShiftDownKeyAction& ShiftDownKeyAction::operator=(const ShiftDownKeyAction& source) {
	KeyAction::operator=(source);
	return *this;
}

//ShiftHomeKeyAction
ShiftHomeKeyAction::ShiftHomeKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

ShiftHomeKeyAction::ShiftHomeKeyAction(const ShiftHomeKeyAction& source)
	: KeyAction(source) {
}

ShiftHomeKeyAction::~ShiftHomeKeyAction() {
}

void ShiftHomeKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_HOME, 0));
}

ShiftHomeKeyAction& ShiftHomeKeyAction::operator =(const ShiftHomeKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//ShiftEndKeyAction
ShiftEndKeyAction::ShiftEndKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

ShiftEndKeyAction::ShiftEndKeyAction(const ShiftEndKeyAction& source)
	: KeyAction(source) {
}

ShiftEndKeyAction::~ShiftEndKeyAction() {
}

void ShiftEndKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_END, 0));
}

ShiftEndKeyAction& ShiftEndKeyAction::operator =(const ShiftEndKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//ShiftCtrlLeftKeyAction
ShiftCtrlLeftKeyAction::ShiftCtrlLeftKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

ShiftCtrlLeftKeyAction::ShiftCtrlLeftKeyAction(const ShiftCtrlLeftKeyAction& source)
	: KeyAction(source) {
}

ShiftCtrlLeftKeyAction::~ShiftCtrlLeftKeyAction() {
}

void ShiftCtrlLeftKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_CTRLLEFT, 0));
}

ShiftCtrlLeftKeyAction& ShiftCtrlLeftKeyAction::operator =(const ShiftCtrlLeftKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//ShiftCtrlRightKeyAction
ShiftCtrlRightKeyAction::ShiftCtrlRightKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

ShiftCtrlRightKeyAction::ShiftCtrlRightKeyAction(const ShiftCtrlRightKeyAction& source)
	: KeyAction(source) {
}

ShiftCtrlRightKeyAction::~ShiftCtrlRightKeyAction() {
}

void ShiftCtrlRightKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_CTRLRIGHT, 0));
}

ShiftCtrlRightKeyAction& ShiftCtrlRightKeyAction::operator =(const ShiftCtrlRightKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//ShiftCtrlHomeKeyAction
ShiftCtrlHomeKeyAction::ShiftCtrlHomeKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

ShiftCtrlHomeKeyAction::ShiftCtrlHomeKeyAction(const ShiftCtrlHomeKeyAction& source)
	: KeyAction(source) {
}

ShiftCtrlHomeKeyAction::~ShiftCtrlHomeKeyAction() {
}

void ShiftCtrlHomeKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_CTRLHOME, 0));
}

ShiftCtrlHomeKeyAction& ShiftCtrlHomeKeyAction::operator =(const ShiftCtrlHomeKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//ShiftCtrlEndKeyAction
ShiftCtrlEndKeyAction::ShiftCtrlEndKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

ShiftCtrlEndKeyAction::ShiftCtrlEndKeyAction(const ShiftCtrlEndKeyAction& source)
	: KeyAction(source) {
}

ShiftCtrlEndKeyAction::~ShiftCtrlEndKeyAction() {
}

void ShiftCtrlEndKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_SELECTMOVE_CTRLEND, 0));
}

ShiftCtrlEndKeyAction& ShiftCtrlEndKeyAction::operator =(const ShiftCtrlEndKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlAKeyAction
CtrlAKeyAction::CtrlAKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlAKeyAction::CtrlAKeyAction(const CtrlAKeyAction& source)
	: KeyAction(source) {
}

CtrlAKeyAction::~CtrlAKeyAction() {
}

void CtrlAKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_SELECTALL, 0));
}

CtrlAKeyAction& CtrlAKeyAction::operator =(const CtrlAKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlCKeyAction
CtrlCKeyAction::CtrlCKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlCKeyAction::CtrlCKeyAction(const CtrlCKeyAction& source)
	: KeyAction(source) {
}

CtrlCKeyAction::~CtrlCKeyAction() {
}

void CtrlCKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_COPY, 0));
}

CtrlCKeyAction& CtrlCKeyAction::operator =(const CtrlCKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlVKeyAction
CtrlVKeyAction::CtrlVKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlVKeyAction::CtrlVKeyAction(const CtrlVKeyAction& source)
	: KeyAction(source) {
}

CtrlVKeyAction::~CtrlVKeyAction() {
}

void CtrlVKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_PASTE, 0));
}

CtrlVKeyAction& CtrlVKeyAction::operator =(const CtrlVKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlXKeyAction
CtrlXKeyAction::CtrlXKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlXKeyAction::CtrlXKeyAction(const CtrlXKeyAction& source)
	: KeyAction(source) {
}

CtrlXKeyAction::~CtrlXKeyAction() {
}

void CtrlXKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (this->textEditingForm->selection != NULL) {
		this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_CUT, 0));
	}
}

CtrlXKeyAction& CtrlXKeyAction::operator =(const CtrlXKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlZKeyAction
CtrlZKeyAction::CtrlZKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlZKeyAction::CtrlZKeyAction(const CtrlZKeyAction& source)
	: KeyAction(source) {
}

CtrlZKeyAction::~CtrlZKeyAction() {
}

void CtrlZKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_UNDO, 0));
}

CtrlZKeyAction& CtrlZKeyAction::operator =(const CtrlZKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlYKeyAction
CtrlYKeyAction::CtrlYKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlYKeyAction::CtrlYKeyAction(const CtrlYKeyAction& source)
	: KeyAction(source) {
}

CtrlYKeyAction::~CtrlYKeyAction() {
}

void CtrlYKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_REDO, 0));
}

CtrlYKeyAction& CtrlYKeyAction::operator =(const CtrlYKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlFKeyAction
CtrlFKeyAction::CtrlFKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlFKeyAction::CtrlFKeyAction(const CtrlFKeyAction& source)
	: KeyAction(source) {
}

CtrlFKeyAction::~CtrlFKeyAction() {
}

void CtrlFKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_FIND, 0));
}

CtrlFKeyAction& CtrlFKeyAction::operator =(const CtrlFKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlHKeyAction
CtrlHKeyAction::CtrlHKeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlHKeyAction::CtrlHKeyAction(const CtrlHKeyAction& source)
	: KeyAction(source) {
}

CtrlHKeyAction::~CtrlHKeyAction() {
}

void CtrlHKeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_EDIT_REPLACE, 0));
}

CtrlHKeyAction& CtrlHKeyAction::operator =(const CtrlHKeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlF1KeyAction
CtrlF1KeyAction::CtrlF1KeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlF1KeyAction::CtrlF1KeyAction(const CtrlF1KeyAction& source)
	: KeyAction(source) {
}

CtrlF1KeyAction::~CtrlF1KeyAction() {
}

void CtrlF1KeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SetIsLockedHScroll(TRUE);
}

CtrlF1KeyAction& CtrlF1KeyAction::operator =(const CtrlF1KeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlF2KeyAction
CtrlF2KeyAction::CtrlF2KeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlF2KeyAction::CtrlF2KeyAction(const CtrlF2KeyAction& source)
	: KeyAction(source) {
}

CtrlF2KeyAction::~CtrlF2KeyAction() {
}

void CtrlF2KeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SetIsUnlockedHistoryBook(TRUE);
}

CtrlF2KeyAction& CtrlF2KeyAction::operator =(const CtrlF2KeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}

//CtrlF3KeyAction
CtrlF3KeyAction::CtrlF3KeyAction(TextEditingForm* textEditingForm)
	: KeyAction(textEditingForm) {
}

CtrlF3KeyAction::CtrlF3KeyAction(const CtrlF3KeyAction& source)
	: KeyAction(source) {
}

CtrlF3KeyAction::~CtrlF3KeyAction() {
}

void CtrlF3KeyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	this->textEditingForm->SetIsUnlockedFindReplaceDialog(TRUE);
}

CtrlF3KeyAction& CtrlF3KeyAction::operator =(const CtrlF3KeyAction& source) {
	KeyAction::operator =(source);

	return *this;
}