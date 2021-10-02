/******************************************************************************
 GetStack.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetStack
#define _H_GetStack

#include "Command.h"

class JTree;
class StackWidget;

class GetStack : public Command
{
public:

	GetStack(const JString& cmd, JTree* tree, StackWidget* widget);

	virtual	~GetStack();

	JTree*			GetTree();
	StackWidget*	GetWidget();

protected:

	virtual void	HandleFailure();

private:

	JTree*			itsTree;	// not owned
	StackWidget*	itsWidget;	// not owned
};


/******************************************************************************
 GetTree

 ******************************************************************************/

inline JTree*
GetStack::GetTree()
{
	return itsTree;
}

/******************************************************************************
 GetWidget

 ******************************************************************************/

inline StackWidget*
GetStack::GetWidget()
{
	return itsWidget;
}

#endif
