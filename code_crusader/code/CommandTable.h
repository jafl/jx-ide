/******************************************************************************
 CommandTable.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CommandTable
#define _H_CommandTable

#include <jx-af/jx/JXEditTable.h>
#include <jx-af/jcore/JFont.h>
#include "CommandManager.h"	// need defn of CmdInfo

class JXTextButton;
class JXTextMenu;
class JXInputField;
class JXColHeaderWidget;
class ListCSF;

class CommandTable : public JXEditTable
{
public:

	CommandTable(const CommandManager::CmdList& cmdList,
				   JXTextButton* addCmdButton, JXTextButton* removeCmdButton,
				   JXTextButton* duplicateCmdButton,
				   JXTextButton* exportButton, JXTextButton* importButton,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~CommandTable();

	void	GetCommandList(CommandManager::CmdList* cmdList) const;

	void	ReadGeometry(std::istream& input);
	void	WriteGeometry(std::ostream& output) const;
	void	SetColTitles(JXColHeaderWidget* widget) const;

	virtual bool	IsEditable(const JPoint& cell) const override;
	virtual void		HandleKeyPress(const JUtf8Character& c,
									   const int keySym, const JXKeyModifiers& modifiers) override;

	// called by CommandSelection

	void	RemoveCommand();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;

	virtual bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDEnter() override;
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	virtual void		HandleDNDLeave() override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CommandManager::CmdList*	itsCmdList;
	JString						itsBasePath;

	JXInputField*	itsTextInput;			// nullptr unless editing
	JFont			itsFont;

	JXTextMenu*		itsOptionsMenu;

	JXTextButton*	itsAddCmdButton;
	JXTextButton*	itsRemoveCmdButton;
	JXTextButton*	itsDuplicateCmdButton;
	JXTextButton*	itsExportButton;
	JXTextButton*	itsImportButton;
	ListCSF*		itsCSF;

	JPoint	itsStartPt;
	JIndex	itsDNDRowIndex;
	Atom	itsCommandXAtom;

private:

	void	FinishCmdListCopy(CommandManager::CmdList* cmdList) const;

	void	AddCommand();
	void	DuplicateCommand();

	void	ExportAllCommands();
	void	ImportCommands();

	void	UpdateOptionsMenu();
	void	HandleOptionsMenu(const JIndex index);

	void	UpdateButtons();

	// not allowed

	CommandTable(const CommandTable& source);
	const CommandTable& operator=(const CommandTable& source);
};

#endif
