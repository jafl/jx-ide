/******************************************************************************
 JVMGetThreads.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreads
#define _H_JVMGetThreads

#include "GetThreads.h"

class JTree;
class JTreeNode;
class JVMThreadNode;

class JVMGetThreads : public GetThreads
{
public:

	JVMGetThreads(JTree* tree, ThreadsWidget* widget);

	virtual	~JVMGetThreads();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;

private:

	void	CopyTree(JVMThreadNode* src, JTreeNode* dest);
};

#endif
