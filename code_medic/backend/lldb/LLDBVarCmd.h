/******************************************************************************
 LLDBVarCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBVarCmd
#define _H_LLDBVarCmd

#include "VarCmd.h"

namespace lldb {

class VarCmd : public ::VarCmd
{
public:

	VarCmd(const JString& cmd);

	~VarCmd();

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JString	itsExpr;
};

};

#endif
