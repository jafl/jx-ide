/******************************************************************************
 XDGetStack.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetStack
#define _H_XDGetStack

#include "GetStack.h"

class XDGetStack : public GetStack
{
public:

	XDGetStack(JTree* tree, StackWidget* widget);

	virtual	~XDGetStack();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
