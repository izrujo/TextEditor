#include "TextEditorApp.h"
#include "TextEditingForm.h"

BOOL TextEditorApp::InitInstance() {
	TextEditingForm *textEditingForm = new TextEditingForm;

	SetRegistryKey("TextEditingForm");
	LoadStdProfileSettings(8);

	textEditingForm->Create(NULL, "Basic Text Editor");
	textEditingForm->ShowWindow(SW_MAXIMIZE);
	textEditingForm->UpdateWindow();
	this->m_pMainWnd = textEditingForm;

	return TRUE;
}
TextEditorApp textEditorApp;