/******************************************************************************
 LLDBGetCompletions.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetCompletions
#define _H_LLDBGetCompletions

#include "GetCompletionsCmd.h"

class CommandInput;
class CommandOutputDisplay;

class LLDBGetCompletions : public GetCompletionsCmd
{
public:

	LLDBGetCompletions(CommandInput* input, CommandOutputDisplay* history);

	virtual	~LLDBGetCompletions();

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JString					itsPrefix;
	CommandInput*			itsInput;
	CommandOutputDisplay*	itsHistory;
};

#endif
