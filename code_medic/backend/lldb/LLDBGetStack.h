/******************************************************************************
 LLDBGetStack.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetStack
#define _H_LLDBGetStack

#include "GetStackCmd.h"

class LLDBGetStackArguments;

class LLDBGetStack : public GetStackCmd
{
public:

	LLDBGetStack(JTree* tree, StackWidget* widget);

	virtual	~LLDBGetStack();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
