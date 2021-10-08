/******************************************************************************
 GetStackCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetStackCmd
#define _H_GetStackCmd

#include "Command.h"

class JTree;
class StackWidget;

class GetStackCmd : public Command
{
public:

	GetStackCmd(const JString& cmd, JTree* tree, StackWidget* widget);

	~GetStackCmd();

	JTree*			GetTree();
	StackWidget*	GetWidget();

protected:

	void	HandleFailure() override;

private:

	JTree*			itsTree;	// not owned
	StackWidget*	itsWidget;	// not owned
};


/******************************************************************************
 GetTree

 ******************************************************************************/

inline JTree*
GetStackCmd::GetTree()
{
	return itsTree;
}

/******************************************************************************
 GetWidget

 ******************************************************************************/

inline StackWidget*
GetStackCmd::GetWidget()
{
	return itsWidget;
}

#endif
