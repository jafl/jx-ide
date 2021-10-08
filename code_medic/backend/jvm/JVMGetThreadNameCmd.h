/******************************************************************************
 JVMGetThreadNameCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadNameCmd
#define _H_JVMGetThreadNameCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

namespace jvm {

class ThreadNode;

class GetThreadNameCmd : public Command, virtual public JBroadcaster
{
public:

	GetThreadNameCmd(ThreadNode* node);

	~GetThreadNameCmd();

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;
	void	HandleFailure() override;

private:

	ThreadNode*	itsNode;
};

};

#endif
