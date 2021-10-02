/******************************************************************************
 LocalVarsDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_LocalVarsDir
#define _H_LocalVarsDir

#include <jx-af/jx/JXWindowDirector.h>

class JTree;
class JXTextButton;
class JXTextMenu;
class Link;
class VarTreeWidget;
class CommandDirector;
class GetLocalVars;

class LocalVarsDir : public JXWindowDirector
{
public:

public:

	LocalVarsDir(CommandDirector* supervisor);

	virtual	~LocalVarsDir();

	virtual void			Activate() override;
	virtual const JString&	GetName() const override;
	virtual bool		GetMenuIcon(const JXImage** icon) const override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*				itsLink;
	CommandDirector*	itsCommandDir;
	JTree*				itsTree;		// owned by itsWidget
	VarTreeWidget*	itsWidget;
	GetLocalVars*		itsGetLocalsCmd;
	bool			itsNeedsUpdateFlag;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JString& binaryName);

	void	Update();
	void	Rebuild();
	void	FlushOldData();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};

#endif
