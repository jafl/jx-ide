/******************************************************************************
 JVMVarCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMVarCmd
#define _H_JVMVarCmd

#include "VarCmd.h"

namespace jvm {

class VarCmd : public ::VarCmd
{
public:

	VarCmd(const JString& cmd);

	~VarCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
