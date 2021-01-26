#ifndef KEYACTIONFACTORY_H
#define KEYACTIONFACTORY_H

#include <afxwin.h>

class KeyAction;
class TextEditingForm;

class KeyActionFactory {
public:
	KeyActionFactory(TextEditingForm *textEditingForm = 0);
	KeyActionFactory(const KeyActionFactory& source);
	~KeyActionFactory();
	KeyActionFactory& operator=(const KeyActionFactory& source);

	KeyAction* Make(UINT nChar);
private:
	TextEditingForm *textEditingForm;
};

#endif // !KEYACTIONFACTORY_H