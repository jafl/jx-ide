/******************************************************************************
 XDGetCompletionsCmd.h

	Copyright (C) 2007 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_XDGetCompletionsCmd
#define _H_XDGetCompletionsCmd

#include "GetCompletionsCmd.h"

class CommandInput;
class CommandOutputDisplay;

namespace xdebug {

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
