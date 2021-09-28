/******************************************************************************
 CMThreadsDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMThreadsDir
#define _H_CMThreadsDir

#include <jx-af/jx/JXWindowDirector.h>

class JXTextButton;
class JXTextMenu;

class CMCommandDirector;
class CMThreadsWidget;

class CMThreadsDir : public JXWindowDirector
{
public:

	CMThreadsDir(CMCommandDirector* supervisor);

	virtual	~CMThreadsDir();

	virtual void			Activate() override;
	virtual const JString&	GetName() const override;
	virtual bool			GetMenuIcon(const JXImage** icon) const override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMCommandDirector*	itsCommandDir;
	CMThreadsWidget*	itsWidget;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(CMCommandDirector* supervisor);
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};

#endif
