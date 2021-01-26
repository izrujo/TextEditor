#ifndef _COMMANDFACTORY_H
#define _COMMANDFACTORY_H

class TextEditingForm;
class Command;

class CommandFactory {
public:
	CommandFactory(TextEditingForm* textEditingForm = 0);
	CommandFactory(const CommandFactory& source);
	~CommandFactory();
	CommandFactory& operator=(const CommandFactory& source);

	Command* Make(int uID);
private:
	TextEditingForm* textEditingForm;
};

#endif //_COMMANDFACTORY_H