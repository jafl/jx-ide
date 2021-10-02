/******************************************************************************
 LLDBVarCommand.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBVarCommand
#define _H_LLDBVarCommand

#include "VarCommand.h"

class LLDBVarCommand : public VarCommand
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
