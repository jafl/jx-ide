/******************************************************************************
 JVMGetThreadGroupsCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadGroupsCmd
#define _H_JVMGetThreadGroupsCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class JTreeNode;

namespace jvm {

class ThreadNode;

class GetThreadGroupsCmd : public Command, virtual public JBroadcaster
{
public:

	GetThreadGroupsCmd(JTreeNode* root, ThreadNode* parent);

	virtual	~GetThreadGroupsCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JTreeNode*	itsRoot;
	ThreadNode*	itsParent;	// can be nullptr
};

};

#endif
