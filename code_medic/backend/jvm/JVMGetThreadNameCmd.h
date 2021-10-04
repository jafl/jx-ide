/******************************************************************************
 JVMGetThreadNameCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadNameCmd
#define _H_JVMGetThreadNameCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class JVMThreadNode;

class JVMGetThreadNameCmd : public Command, virtual public JBroadcaster
{
public:

	JVMGetThreadNameCmd(JVMThreadNode* node);

	virtual	~JVMGetThreadNameCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	HandleFailure() override;

private:

	JVMThreadNode*	itsNode;
};

#endif
