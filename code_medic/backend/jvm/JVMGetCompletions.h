/******************************************************************************
 JVMGetCompletions.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_JVMGetCompletions
#define _H_JVMGetCompletions

#include "GetCompletionsCmd.h"

class CommandInput;
class CommandOutputDisplay;

class JVMGetCompletions : public GetCompletionsCmd
{
public:

	JVMGetCompletions(CommandInput* input, CommandOutputDisplay* history);

	virtual	~JVMGetCompletions();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
