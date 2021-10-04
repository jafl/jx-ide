/******************************************************************************
 JVMGetThreadsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadsCmd
#define _H_JVMGetThreadsCmd

#include "GetThreadsCmd.h"

class JTree;
class JTreeNode;
class JVMThreadNode;

class JVMGetThreadsCmd : public GetThreadsCmd
{
public:

	JVMGetThreadsCmd(JTree* tree, ThreadsWidget* widget);

	virtual	~JVMGetThreadsCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;

private:

	void	CopyTree(JVMThreadNode* src, JTreeNode* dest);
};

#endif
