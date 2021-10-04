/******************************************************************************
 JVMGetCompletionsCmd.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JVMGetCompletionsCmd
#define _H_JVMGetCompletionsCmd

#include "GetCompletionsCmd.h"

class CommandInput;
class CommandOutputDisplay;

class JVMGetCompletionsCmd : public GetCompletionsCmd
{
public:

	JVMGetCompletionsCmd(CommandInput* input, CommandOutputDisplay* history);

	virtual	~JVMGetCompletionsCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
