/******************************************************************************
 JVMGetStack.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetStack
#define _H_JVMGetStack

#include "GetStack.h"

class JVMGetStack : public GetStack
{
public:

	JVMGetStack(JTree* tree, StackWidget* widget);

	virtual	~JVMGetStack();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
