/******************************************************************************
 JVMGetCompletionsCmd.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JVMGetCompletionsCmd
#define _H_JVMGetCompletionsCmd

#include "GetCompletionsCmd.h"

class CommandInput;
class CommandOutputDisplay;

namespace jvm {

class GetCompletionsCmd : public ::GetCompletionsCmd
{
public:

	GetCompletionsCmd(CommandInput* input, CommandOutputDisplay* history);

	virtual	~GetCompletionsCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

};

#endif
