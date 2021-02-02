#ifndef _COMMAND_H
#define _COMMAND_H

#include "Array.h"
#include <iostream>
#include <afxwin.h> //TCHAR

using namespace std;
typedef signed long int Long;

class TextEditingForm;
class Glyph;

class Command {
public:
	Command(TextEditingForm* textEditingForm = 0);
	Command(const Command& source);
	virtual ~Command() = 0;
	Command& operator=(const Command& source);

	virtual void Execute() = 0;
	virtual void Unexecute();

	virtual string GetType() = 0;
	virtual Command* Clone() = 0;

	//MacroCommand
	virtual Long Add(Command* command);
	virtual Long Remove(Long index);
	virtual Command* GetAt(Long index);

	virtual Long GetCapacity() const;
	virtual Long GetLength() const;

protected:
	TextEditingForm* textEditingForm;
};

//////////////////// Composite ////////////////////
//MacroCommand
class MacroCommand : public Command {
public:
	MacroCommand(TextEditingForm* textEditingForm = 0, Long capacity = 10);
	MacroCommand(const MacroCommand& source);
	virtual ~MacroCommand();
	MacroCommand& operator=(const MacroCommand& source);

	virtual void Execute();
	virtual void Unexecute();

	virtual Long Add(Command* command);
	virtual Long Remove(Long index);
	virtual Command* GetAt(Long index);

	virtual string GetType();
	virtual Command* Clone();

	virtual Long GetCapacity() const;
	virtual Long GetLength() const;

private:
	Array<Command*> commands;
	Long capacity;
	Long length;
};

inline Long MacroCommand::GetCapacity() const {
	return this->capacity;
}

inline Long MacroCommand::GetLength() const {
	return this->length;
}
//////////////////// Composite ////////////////////

//////////////////// Basic ////////////////////
//WriteBasicCommand
class WriteBasicCommand : public Command {
public:
	WriteBasicCommand(TextEditingForm* textEditingForm = 0);
	WriteBasicCommand(const WriteBasicCommand& source);
	virtual ~WriteBasicCommand();
	WriteBasicCommand& operator=(const WriteBasicCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//ImeCompositionBasicCommand
class ImeCompositionBasicCommand : public Command {
public:
	ImeCompositionBasicCommand(TextEditingForm* textEditingForm = 0);
	ImeCompositionBasicCommand(const ImeCompositionBasicCommand& source);
	virtual ~ImeCompositionBasicCommand();
	ImeCompositionBasicCommand& operator=(const ImeCompositionBasicCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//ImeCharBasicCommand
class ImeCharBasicCommand : public Command {
public:
	ImeCharBasicCommand(TextEditingForm* textEditingForm = 0);
	ImeCharBasicCommand(const ImeCharBasicCommand& source);
	virtual ~ImeCharBasicCommand();
	ImeCharBasicCommand& operator=(const ImeCharBasicCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//DeleteBasicCommand
class DeleteBasicCommand : public Command {
public:
	DeleteBasicCommand(TextEditingForm* textEditingForm = 0);
	DeleteBasicCommand(const DeleteBasicCommand& source);
	virtual ~DeleteBasicCommand();
	DeleteBasicCommand& operator=(const DeleteBasicCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//CopyBasicCommand
class CopyBasicCommand : public Command {
public:
	CopyBasicCommand(TextEditingForm* textEditingForm = 0);
	CopyBasicCommand(const CopyBasicCommand& source);
	virtual ~CopyBasicCommand();
	CopyBasicCommand& operator=(const CopyBasicCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//DeleteSelectionBasicCommand
class DeleteSelectionBasicCommand : public Command {
public:
	DeleteSelectionBasicCommand(TextEditingForm* textEditingForm = 0);
	DeleteSelectionBasicCommand(const DeleteSelectionBasicCommand& source);
	virtual ~DeleteSelectionBasicCommand();
	DeleteSelectionBasicCommand& operator=(const DeleteSelectionBasicCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};
//////////////////// Basic ////////////////////

//////////////////// Main ////////////////////
//WriteCommand
class WriteCommand : public Command {
public:
	WriteCommand(TextEditingForm* textEditingForm = 0);
	WriteCommand(const WriteCommand& source);
	virtual ~WriteCommand();
	WriteCommand& operator=(const WriteCommand& source);

	virtual void Execute();
	virtual void Unexecute();

	virtual string GetType();
	virtual Command* Clone();

private:
	int nChar;
	Long row;
	Long column;
};

//ImeCompositionCommand
class ImeCompositionCommand : public Command {
public:
	ImeCompositionCommand(TextEditingForm* textEditingForm = 0);
	ImeCompositionCommand(const ImeCompositionCommand& source);
	virtual ~ImeCompositionCommand();
	ImeCompositionCommand& operator=(const ImeCompositionCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//ImeCharCommand
class ImeCharCommand : public Command {
public:
	ImeCharCommand(TextEditingForm* textEditingForm = 0);
	ImeCharCommand(const ImeCharCommand& source);
	virtual ~ImeCharCommand();
	ImeCharCommand& operator=(const ImeCharCommand& source);

	virtual void Execute();
	virtual void Unexecute();

	virtual string GetType();
	virtual Command* Clone();

private:
	TCHAR(*buffer);
	Long row;
	Long column;
};

//DeleteCommand
class DeleteCommand : public Command {
public:
	DeleteCommand(TextEditingForm* textEditingForm = 0);
	DeleteCommand(const DeleteCommand& source);
	virtual ~DeleteCommand();
	DeleteCommand& operator=(const DeleteCommand& source);

	virtual void Execute();
	virtual void Unexecute();

	virtual string GetType();
	virtual Command* Clone();

private:
	Long row;
	Long noteLength;
	Long column;
	Long lineLength;
	Glyph* character;
};

//CopyCommand
class CopyCommand : public Command {
public:
	CopyCommand(TextEditingForm* textEditingForm = 0);
	CopyCommand(const CopyCommand& source);
	virtual ~CopyCommand();
	CopyCommand& operator=(const CopyCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//DeleteSelectionCommand
class DeleteSelectionCommand : public Command {
public:
	DeleteSelectionCommand(TextEditingForm* textEditingForm = 0);
	DeleteSelectionCommand(const DeleteSelectionCommand& source);
	virtual ~DeleteSelectionCommand();
	DeleteSelectionCommand& operator=(const DeleteSelectionCommand& source);

	virtual void Execute();
	virtual void Unexecute();

	virtual string GetType();
	virtual Command* Clone();

private:
	Long startRow;
	Long startColumn;
	Long endRow;
	Long endColumn;
	string selecteds;
};

//CutCommand
class CutCommand : public Command {
public:
	CutCommand(TextEditingForm* textEditingForm = 0);
	CutCommand(const CutCommand& source);
	virtual ~CutCommand();
	CutCommand& operator=(const CutCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//PasteCommand
class PasteCommand : public Command {
public:
	PasteCommand(TextEditingForm* textEditingForm = 0);
	PasteCommand(const PasteCommand& source);
	virtual ~PasteCommand();
	PasteCommand& operator=(const PasteCommand& source);

	virtual void Execute();
	virtual void Unexecute();

	virtual string GetType();
	virtual Command* Clone();

private:
	Long startRow;
	Long startColumn;
	Long endRow;
	Long endColumn;
	string pasteds;
};

//SelectAllCommand
class SelectAllCommand : public Command {
public:
	SelectAllCommand(TextEditingForm* textEditingForm = 0);
	SelectAllCommand(const SelectAllCommand& source);
	virtual ~SelectAllCommand();
	SelectAllCommand& operator=(const SelectAllCommand& source);

	virtual void Execute();

	virtual string GetType();
	virtual Command* Clone();
};

//UndoCommand
class UndoCommand : public Command {
public:
	UndoCommand(TextEditingForm* textEditingForm = 0);
	UndoCommand(const UndoCommand& source);
	virtual ~UndoCommand();
	UndoCommand& operator=(const UndoCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//RedoCommand
class RedoCommand : public Command {
public:
	RedoCommand(TextEditingForm* textEditingForm = 0);
	RedoCommand(const RedoCommand& source);
	virtual ~RedoCommand();
	RedoCommand& operator=(const RedoCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//FindCommand
class FindCommand : public Command {
public:
	FindCommand(TextEditingForm* textEditingForm = 0);
	FindCommand(const FindCommand& source);
	virtual ~FindCommand();
	FindCommand& operator=(const FindCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ReplaceCommand
class ReplaceCommand : public Command {
public:
	ReplaceCommand(TextEditingForm* textEditingForm = 0);
	ReplaceCommand(const ReplaceCommand& source);
	virtual ~ReplaceCommand();
	ReplaceCommand& operator=(const ReplaceCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};
//////////////////// Main ////////////////////

//////////////////// Move ////////////////////
//LeftCommand
class LeftCommand : public Command {
public:
	LeftCommand(TextEditingForm* textEditingForm = 0);
	LeftCommand(const LeftCommand& source);
	virtual ~LeftCommand();
	LeftCommand& operator=(const LeftCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//RightCommand
class RightCommand : public Command {
public:
	RightCommand(TextEditingForm* textEditingForm = 0);
	RightCommand(const RightCommand& source);
	virtual ~RightCommand();
	RightCommand& operator=(const RightCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//UpCommand
class UpCommand : public Command {
public:
	UpCommand(TextEditingForm* textEditingForm = 0);
	UpCommand(const UpCommand& source);
	virtual ~UpCommand();
	UpCommand& operator=(const UpCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//DownCommand
class DownCommand : public Command {
public:
	DownCommand(TextEditingForm* textEditingForm = 0);
	DownCommand(const DownCommand& source);
	virtual ~DownCommand();
	DownCommand& operator=(const DownCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//HomeCommand
class HomeCommand : public Command {
public:
	HomeCommand(TextEditingForm* textEditingForm = 0);
	HomeCommand(const HomeCommand& source);
	virtual ~HomeCommand();
	HomeCommand& operator=(const HomeCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//EndCommand
class EndCommand : public Command {
public:
	EndCommand(TextEditingForm* textEditingForm = 0);
	EndCommand(const EndCommand& source);
	virtual ~EndCommand();
	EndCommand& operator=(const EndCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//CtrlLeftCommand
class CtrlLeftCommand : public Command {
public:
	CtrlLeftCommand(TextEditingForm* textEditingForm = 0);
	CtrlLeftCommand(const CtrlLeftCommand& source);
	virtual ~CtrlLeftCommand();
	CtrlLeftCommand& operator=(const CtrlLeftCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//CtrlRightCommand
class CtrlRightCommand : public Command {
public:
	CtrlRightCommand(TextEditingForm* textEditingForm = 0);
	CtrlRightCommand(const CtrlRightCommand& source);
	virtual ~CtrlRightCommand();
	CtrlRightCommand& operator=(const CtrlRightCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//CtrlHomeCommand
class CtrlHomeCommand : public Command {
public:
	CtrlHomeCommand(TextEditingForm* textEditingForm = 0);
	CtrlHomeCommand(const CtrlHomeCommand& source);
	virtual ~CtrlHomeCommand();
	CtrlHomeCommand& operator=(const CtrlHomeCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//CtrlEndCommand
class CtrlEndCommand : public Command {
public:
	CtrlEndCommand(TextEditingForm* textEditingForm = 0);
	CtrlEndCommand(const CtrlEndCommand& source);
	virtual ~CtrlEndCommand();
	CtrlEndCommand& operator=(const CtrlEndCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//PageUpCommand
class PageUpCommand : public Command {
public:
	PageUpCommand(TextEditingForm* textEditingForm = 0);
	PageUpCommand(const PageUpCommand& source);
	virtual ~PageUpCommand();
	PageUpCommand& operator=(const PageUpCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//PageDownCommand
class PageDownCommand : public Command {
public:
	PageDownCommand(TextEditingForm* textEditingForm = 0);
	PageDownCommand(const PageDownCommand& source);
	virtual ~PageDownCommand();
	PageDownCommand& operator=(const PageDownCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};
//////////////////// Move ////////////////////

//////////////////// Select ////////////////////
//ShiftLeftCommand
class ShiftLeftCommand : public Command {
public:
	ShiftLeftCommand(TextEditingForm* textEditingForm = 0);
	ShiftLeftCommand(const ShiftLeftCommand& source);
	virtual ~ShiftLeftCommand();
	ShiftLeftCommand& operator=(const ShiftLeftCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftRightCommand
class ShiftRightCommand : public Command {
public:
	ShiftRightCommand(TextEditingForm* textEditingForm = 0);
	ShiftRightCommand(const ShiftRightCommand& source);
	virtual ~ShiftRightCommand();
	ShiftRightCommand& operator=(const ShiftRightCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftUpCommand
class ShiftUpCommand : public Command {
public:
	ShiftUpCommand(TextEditingForm* textEditingForm = 0);
	ShiftUpCommand(const ShiftUpCommand& source);
	virtual ~ShiftUpCommand();
	ShiftUpCommand& operator=(const ShiftUpCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftDownCommand
class ShiftDownCommand : public Command {
public:
	ShiftDownCommand(TextEditingForm* textEditingForm = 0);
	ShiftDownCommand(const ShiftDownCommand& source);
	virtual ~ShiftDownCommand();
	ShiftDownCommand& operator=(const ShiftDownCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftHomeCommand
class ShiftHomeCommand : public Command {
public:
	ShiftHomeCommand(TextEditingForm* textEditingForm = 0);
	ShiftHomeCommand(const ShiftHomeCommand& source);
	virtual ~ShiftHomeCommand();
	ShiftHomeCommand& operator=(const ShiftHomeCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftEndCommand
class ShiftEndCommand : public Command {
public:
	ShiftEndCommand(TextEditingForm* textEditingForm = 0);
	ShiftEndCommand(const ShiftEndCommand& source);
	virtual ~ShiftEndCommand();
	ShiftEndCommand& operator=(const ShiftEndCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftCtrlLeftCommand
class ShiftCtrlLeftCommand : public Command {
public:
	ShiftCtrlLeftCommand(TextEditingForm* textEditingForm = 0);
	ShiftCtrlLeftCommand(const ShiftCtrlLeftCommand& source);
	virtual ~ShiftCtrlLeftCommand();
	ShiftCtrlLeftCommand& operator=(const ShiftCtrlLeftCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftCtrlRightCommand
class ShiftCtrlRightCommand : public Command {
public:
	ShiftCtrlRightCommand(TextEditingForm* textEditingForm = 0);
	ShiftCtrlRightCommand(const ShiftCtrlRightCommand& source);
	virtual ~ShiftCtrlRightCommand();
	ShiftCtrlRightCommand& operator=(const ShiftCtrlRightCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftCtrlHomeCommand
class ShiftCtrlHomeCommand : public Command {
public:
	ShiftCtrlHomeCommand(TextEditingForm* textEditingForm = 0);
	ShiftCtrlHomeCommand(const ShiftCtrlHomeCommand& source);
	virtual ~ShiftCtrlHomeCommand();
	ShiftCtrlHomeCommand& operator=(const ShiftCtrlHomeCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//ShiftCtrlEndCommand
class ShiftCtrlEndCommand : public Command {
public:
	ShiftCtrlEndCommand(TextEditingForm* textEditingForm = 0);
	ShiftCtrlEndCommand(const ShiftCtrlEndCommand& source);
	virtual ~ShiftCtrlEndCommand();
	ShiftCtrlEndCommand& operator=(const ShiftCtrlEndCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};
//////////////////// Select ////////////////////

//////////////////// Flag ////////////////////
//LockHScrollCommand
class LockHScrollCommand : public Command {
public:
	LockHScrollCommand(TextEditingForm* textEditingForm = 0);
	LockHScrollCommand(const LockHScrollCommand& source);
	virtual ~LockHScrollCommand();
	LockHScrollCommand& operator=(const LockHScrollCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//UnlockHistoryBookCommand
class UnlockHistoryBookCommand : public Command {
public:
	UnlockHistoryBookCommand(TextEditingForm* textEditingForm = 0);
	UnlockHistoryBookCommand(const UnlockHistoryBookCommand& source);
	virtual ~UnlockHistoryBookCommand();
	UnlockHistoryBookCommand& operator=(const UnlockHistoryBookCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};

//UnlockFindReplaceDialogCommand
class UnlockFindReplaceDialogCommand : public Command {
public:
	UnlockFindReplaceDialogCommand(TextEditingForm* textEditingForm = 0);
	UnlockFindReplaceDialogCommand(const UnlockFindReplaceDialogCommand& source);
	virtual ~UnlockFindReplaceDialogCommand();
	UnlockFindReplaceDialogCommand& operator=(const UnlockFindReplaceDialogCommand& source);

	virtual void Execute();
	virtual string GetType();
	virtual Command* Clone();
};
//////////////////// Flag ////////////////////

#endif //_COMMAND_H