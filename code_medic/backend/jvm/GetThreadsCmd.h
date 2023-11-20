/******************************************************************************
 GetThreadsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadsCmd
#define _H_JVMGetThreadsCmd

#include <GetThreadsCmd.h>

class JTree;
class JTreeNode;

namespace jvm {

class ThreadNode;

class GetThreadsCmd : public ::GetThreadsCmd
{
public:

	GetThreadsCmd(JTree* tree, ThreadsWidget* widget);

	~GetThreadsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;

private:

	void	CopyTree(ThreadNode* src, JTreeNode* dest);
};

};

#endif
