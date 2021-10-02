/******************************************************************************
 JVMGetFrame.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetFrame
#define _H_JVMGetFrame

#include "GetFrameCmd.h"

class StackWidget;

class JVMGetFrame : public GetFrameCmd
{
public:

	JVMGetFrame(StackWidget* widget);

	virtual	~JVMGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	StackWidget*	itsWidget;
};

#endif
