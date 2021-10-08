/******************************************************************************
 JVMGetThreadCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetThreadCmd
#define _H_JVMGetThreadCmd

#include "GetThreadCmd.h"

namespace jvm {

class GetThreadCmd : public ::GetThreadCmd
{
public:

	GetThreadCmd(ThreadsWidget* widget);

	~GetThreadCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
