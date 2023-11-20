/******************************************************************************
 GetCompletionsCmd.h

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_GDBGetCompletionsCmd
#define _H_GDBGetCompletionsCmd

#include <GetCompletionsCmd.h>

class CommandInput;
class CommandOutputDisplay;

namespace gdb {

class GetCompletionsCmd : public ::GetCompletionsCmd
{
public:

	GetCompletionsCmd(CommandInput* input, CommandOutputDisplay* history);

	~GetCompletionsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JString					itsPrefix;
	CommandInput*			itsInput;
	CommandOutputDisplay*	itsHistory;

private:

	static JString	BuildCommand(const JString& prefix);
};

};

#endif
