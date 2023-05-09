/******************************************************************************
 Array1DDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_Array1DDir
#define _H_Array1DDir

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JRange.h>

class JTree;
class JTreeNode;
class JXTextButton;
class JXTextMenu;
class Link;
class VarTreeWidget;
class ArrayExprInput;
class ArrayIndexInput;
class CommandDirector;

class Array1DDir : public JXWindowDirector
{
public:

	Array1DDir(CommandDirector* supervisor, const JString& expr);
	Array1DDir(std::istream& input, const JFileVersion vers,
				 CommandDirector* supervisor);

	~Array1DDir() override;

	void	Activate() override;
	bool	Deactivate() override;
	bool	GetMenuIcon(const JXImage** icon) const override;

	void	StreamOut(std::ostream& output);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*				itsLink;
	CommandDirector*	itsCommandDir;
	JTree*				itsTree;			// owned by itsWidget
	VarTreeWidget*		itsWidget;
	JString				itsExpr;
	JIntRange			itsDisplayRange;
	JIntRange			itsRequestRange;
	JTreeNode*			itsCurrentNode;		// owned by itsTree
	bool				itsWaitingForReloadFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsActionMenu;

// begin JXLayout

	ArrayExprInput*  itsExprInput;
	JXTextButton*    itsStopButton;
	ArrayIndexInput* itsStartIndex;
	ArrayIndexInput* itsEndIndex;

// end JXLayout

private:

	void	Array1DDirX(CommandDirector* supervisor);
	void	BuildWindow();
	void	UpdateWindowTitle();

	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	BeginCreateNodes();
	void	CreateNextNode();
	JString	GetExpression(const JInteger i) const;
	void	CreateNodesFinished();
};

#endif
