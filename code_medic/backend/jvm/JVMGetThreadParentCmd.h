/******************************************************************************
 JVMGetThreadParentCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadParentCmd
#define _H_JVMGetThreadParentCmd

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

namespace jvm {

class ThreadNode;

class GetThreadParentCmd : public Command, virtual public JBroadcaster
{
public:

	GetThreadParentCmd(ThreadNode* node, const bool checkOnly = false);

	~GetThreadParentCmd() override;

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;
	void	HandleFailure() override;

private:

	ThreadNode*	itsNode;
	const bool	itsCheckOnlyFlag;
};

};

#endif
