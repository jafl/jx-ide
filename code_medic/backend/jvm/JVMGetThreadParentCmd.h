/******************************************************************************
 JVMGetThreadParentCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadParentCmd
#define _H_JVMGetThreadParentCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class JVMThreadNode;

class JVMGetThreadParentCmd : public Command, virtual public JBroadcaster
{
public:

	JVMGetThreadParentCmd(JVMThreadNode* node, const bool checkOnly = false);

	virtual	~JVMGetThreadParentCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	HandleFailure() override;

private:

	JVMThreadNode*	itsNode;
	const bool	itsCheckOnlyFlag;
};

#endif
