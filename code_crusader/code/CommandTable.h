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

	bool	IsEditable(const JPoint& cell) const override;
	void		HandleKeyPress(const JUtf8Character& c,
									   const int keySym, const JXKeyModifiers& modifiers) override;

	// called by CommandSelection

	void	RemoveCommand();

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	void		HandleDNDEnter() override;
	void		HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	void		HandleDNDLeave() override;
	void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	bool		ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

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
