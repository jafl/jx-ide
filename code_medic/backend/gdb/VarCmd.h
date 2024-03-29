/******************************************************************************
 VarCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBVarCmd
#define _H_GDBVarCmd

#include <VarCmd.h>

namespace gdb {

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
