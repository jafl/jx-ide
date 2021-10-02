/******************************************************************************
 GDBGetStack.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStack
#define _H_GDBGetStack

#include "GetStack.h"

class GDBGetStackArguments;

class GDBGetStack : public GetStack
{
public:

	GDBGetStack(JTree* tree, StackWidget* widget);

	virtual	~GDBGetStack();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	GDBGetStackArguments*	itsArgsCmd;
};

#endif
