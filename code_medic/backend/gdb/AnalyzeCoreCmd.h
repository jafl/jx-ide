/******************************************************************************
 AnalyzeCoreCmd.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBAnalyzeCoreCmd
#define _H_GDBAnalyzeCoreCmd

#include "Command.h"

namespace gdb {

class AnalyzeCoreCmd : public Command
{
public:

	AnalyzeCoreCmd(const JString& cmd);

	~AnalyzeCoreCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
