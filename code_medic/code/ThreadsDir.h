/******************************************************************************
 ThreadsDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_ThreadsDir
#define _H_ThreadsDir

#include <jx-af/jx/JXWindowDirector.h>

class JXTextButton;
class JXTextMenu;

class CommandDirector;
class ThreadsWidget;

class ThreadsDir : public JXWindowDirector
{
public:

	ThreadsDir(CommandDirector* supervisor);

	~ThreadsDir() override;

	void			Activate() override;
	const JString&	GetName() const override;
	bool			GetMenuIcon(const JXImage** icon) const override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CommandDirector*	itsCommandDir;
	ThreadsWidget*		itsWidget;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(CommandDirector* supervisor);
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};

#endif
