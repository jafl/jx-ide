/******************************************************************************
 BreakpointsDir.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_BreakpointsDir
#define _H_BreakpointsDir

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JXColHeaderWidget;
class CommandDirector;
class BreakpointTable;

class BreakpointsDir : public JXWindowDirector
{
public:

	BreakpointsDir(CommandDirector* supervisor);

	~BreakpointsDir() override;

	const JString&	GetName() const override;
	bool			GetMenuIcon(const JXImage** icon) const override;

	CommandDirector*	GetCommandDirector();
	BreakpointTable*	GetBreakpointTable();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CommandDirector*	itsCommandDir;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

// begin tablelayout

	BreakpointTable*   itsTable;
	JXColHeaderWidget* itsColHeader;

// end tablelayout

private:

	void	BuildWindow(CommandDirector* supervisor);
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};


/******************************************************************************
 GetCommandDirector

 ******************************************************************************/

inline CommandDirector*
BreakpointsDir::GetCommandDirector()
{
	return itsCommandDir;
}

/******************************************************************************
 GetBreakpointTable

 ******************************************************************************/

inline BreakpointTable*
BreakpointsDir::GetBreakpointTable()
{
	return itsTable;
}

#endif
