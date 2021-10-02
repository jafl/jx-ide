/******************************************************************************
 StackDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_StackDir
#define _H_StackDir

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class CommandDirector;
class StackWidget;

class StackDir : public JXWindowDirector
{
public:

	StackDir(CommandDirector* supervisor);

	virtual	~StackDir();

	virtual void			Activate() override;
	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

	StackWidget*	GetStackWidget();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CommandDirector*	itsCommandDir;
	StackWidget*		itsWidget;

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


/******************************************************************************
 GetStackWidget

 ******************************************************************************/

inline StackWidget*
StackDir::GetStackWidget()
{
	return itsWidget;
}

#endif
