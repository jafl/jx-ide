/******************************************************************************
 LLDBVarCommand.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBVarCommand
#define _H_LLDBVarCommand

#include "CMVarCommand.h"

class LLDBVarCommand : public CMVarCommand
{
public:

	LLDBVarCommand(const JString& cmd);

	virtual	~LLDBVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JString	itsExpr;
};

#endif
