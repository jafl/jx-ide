/******************************************************************************
 XDGetStack.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetStack
#define _H_XDGetStack

#include "GetStackCmd.h"

class XDGetStack : public GetStackCmd
{
public:

	XDGetStack(JTree* tree, StackWidget* widget);

	virtual	~XDGetStack();

protected:

	void	HandleSuccess(const JString& data) override;
};

#endif
