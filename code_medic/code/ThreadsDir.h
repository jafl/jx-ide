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

	virtual	~ThreadsDir();

	virtual void			Activate() override;
	virtual const JString&	GetName() const override;
	virtual bool			GetMenuIcon(const JXImage** icon) const override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CommandDirector*	itsCommandDir;
	ThreadsWidget*	itsWidget;

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
