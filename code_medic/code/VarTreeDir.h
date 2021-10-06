/******************************************************************************
 VarTreeDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_VarTreeDir
#define _H_VarTreeDir

#include <jx-af/jx/JXWindowDirector.h>

class JTree;
class JXTextButton;
class JXTextMenu;
class Link;
class VarTreeWidget;
class CommandDirector;

class VarTreeDir : public JXWindowDirector
{
public:

public:

	VarTreeDir(CommandDirector* supervisor);

	virtual	~VarTreeDir();

	void	DisplayExpression(const JString& expr);

	void			Activate() override;
	bool		Deactivate() override;
	const JString&	GetName() const override;
	bool		GetMenuIcon(const JXImage** icon) const override;

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*				itsLink;
	CommandDirector*	itsCommandDir;
	JTree*				itsTree;		// owned by itsWidget
	VarTreeWidget*	itsWidget;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();
	void	UpdateWindowTitle(const JString& binaryName);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};

#endif
