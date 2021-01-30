#include "TextEditorApp.h"
#include "TextEditingForm.h"

BOOL TextEditorApp::InitInstance() {
	TextEditingForm *textEditingForm = new TextEditingForm;

	textEditingForm->CreateEx(NULL, AfxRegisterWndClass(NULL), "Basic Text Editor", WS_POPUP,
		CRect(200, 200, 500, 500), NULL, NULL, NULL);
	textEditingForm->ShowWindow(SW_SHOW);
	textEditingForm->UpdateWindow();
	this->m_pMainWnd = textEditingForm;

	return TRUE;
}
TextEditorApp textEditorApp;