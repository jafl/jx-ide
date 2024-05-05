/******************************************************************************
 VarTypeCmd.h

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBVarTypeCmd
#define _H_GDBVarTypeCmd

#include <VarTypeCmd.h>

namespace gdb {

class VarTypeCmd : public ::VarTypeCmd
{
public:

	VarTypeCmd(const JString& cmd);

	~VarTypeCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
