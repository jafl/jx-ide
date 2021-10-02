/******************************************************************************
 GDBGetFrameCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetFrameCmd
#define _H_GDBGetFrameCmd

#include "GetFrameCmd.h"

class StackWidget;

class GDBGetFrameCmd : public GetFrameCmd
{
public:

	GDBGetFrameCmd(StackWidget* widget);

	virtual	~GDBGetFrameCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	StackWidget*	itsWidget;
};

#endif
