/******************************************************************************
 JVMGetThreadGroups.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadGroups
#define _H_JVMGetThreadGroups

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class JTreeNode;
class JVMThreadNode;

class JVMGetThreadGroups : public Command, virtual public JBroadcaster
{
public:

	JVMGetThreadGroups(JTreeNode* root, JVMThreadNode* parent);

	virtual	~JVMGetThreadGroups();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JTreeNode*		itsRoot;
	JVMThreadNode*	itsParent;	// can be nullptr
};

#endif
