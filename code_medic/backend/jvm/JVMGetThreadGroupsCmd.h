/******************************************************************************
 JVMGetThreadGroupsCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadGroupsCmd
#define _H_JVMGetThreadGroupsCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class JTreeNode;
class JVMThreadNode;

class JVMGetThreadGroupsCmd : public Command, virtual public JBroadcaster
{
public:

	JVMGetThreadGroupsCmd(JTreeNode* root, JVMThreadNode* parent);

	virtual	~JVMGetThreadGroupsCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JTreeNode*		itsRoot;
	JVMThreadNode*	itsParent;	// can be nullptr
};

#endif
