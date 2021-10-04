/******************************************************************************
 JVMGetFrameCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetFrameCmd
#define _H_JVMGetFrameCmd

#include "GetFrameCmd.h"

class StackWidget;

class JVMGetFrameCmd : public GetFrameCmd
{
public:

	JVMGetFrameCmd(StackWidget* widget);

	virtual	~JVMGetFrameCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	StackWidget*	itsWidget;
};

#endif
