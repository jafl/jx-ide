/******************************************************************************
 JVMGetThreadParent.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadParent
#define _H_JVMGetThreadParent

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class JVMThreadNode;

class JVMGetThreadParent : public Command, virtual public JBroadcaster
{
public:

	JVMGetThreadParent(JVMThreadNode* node, const bool checkOnly = false);

	virtual	~JVMGetThreadParent();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	HandleFailure() override;

private:

	JVMThreadNode*	itsNode;
	const bool	itsCheckOnlyFlag;
};

#endif
