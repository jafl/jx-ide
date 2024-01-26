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

	~StackDir() override;

	void			Activate() override;
	const JString&	GetName() const override;
	bool			GetMenuIcon(const JXImage** icon) const override;

	StackWidget*	GetStackWidget();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CommandDirector*	itsCommandDir;

	JXTextMenu*	itsFileMenu;

// begin JXLayout

	StackWidget* itsWidget;

// end JXLayout

private:

	void	BuildWindow(CommandDirector* supervisor);
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);
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
