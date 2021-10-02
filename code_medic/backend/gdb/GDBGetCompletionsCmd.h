/******************************************************************************
 GDBGetCompletionsCmd.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GDBGetCompletionsCmd
#define _H_GDBGetCompletionsCmd

#include "GetCompletionsCmd.h"

class CommandInput;
class CommandOutputDisplay;

class GDBGetCompletionsCmd : public GetCompletionsCmd
{
public:

	GDBGetCompletionsCmd(CommandInput* input, CommandOutputDisplay* history);

	virtual	~GDBGetCompletionsCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JString					itsPrefix;
	CommandInput*			itsInput;
	CommandOutputDisplay*	itsHistory;

private:

	static JString	BuildCommand(const JString& prefix);
};

#endif
