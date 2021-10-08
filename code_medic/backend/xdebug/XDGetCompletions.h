/******************************************************************************
 XDGetCompletions.h

	Copyright (C) 2007 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_XDGetCompletions
#define _H_XDGetCompletions

#include "GetCompletionsCmd.h"

class CommandInput;
class CommandOutputDisplay;

class XDGetCompletions : public GetCompletionsCmd
{
public:

	XDGetCompletions(CommandInput* input, CommandOutputDisplay* history);

	~XDGetCompletions();

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JString					itsPrefix;
	CommandInput*			itsInput;
	CommandOutputDisplay*	itsHistory;

private:

	static JString	BuildCommand(const JString& prefix);
};

#endif
