/******************************************************************************
 GetCompletionsCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetCompletionsCmd
#define _H_LLDBGetCompletionsCmd

#include <GetCompletionsCmd.h>

class CommandInput;
class CommandOutputDisplay;

namespace lldb {

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
};

};

#endif
