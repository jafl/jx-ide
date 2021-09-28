/******************************************************************************
 JVMGetThreadName.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadName
#define _H_JVMGetThreadName

#include "CMCommand.h"
#include <jx-af/jcore/JBroadcaster.h>

class JVMThreadNode;

class JVMGetThreadName : public CMCommand, virtual public JBroadcaster
{
public:

	JVMGetThreadName(JVMThreadNode* node);

	virtual	~JVMGetThreadName();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	HandleFailure() override;

private:

	JVMThreadNode*	itsNode;
};

#endif