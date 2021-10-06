/******************************************************************************
 GDBGetFrameCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetFrameCmd
#define _H_GDBGetFrameCmd

#include "GetFrameCmd.h"

class StackWidget;

namespace gdb {

class GetFrameCmd : public ::GetFrameCmd
{
public:

	GetFrameCmd(StackWidget* widget);

	virtual	~GetFrameCmd();

protected:

	void	HandleSuccess(const JString& data) override;

private:

	StackWidget*	itsWidget;
};

};

#endif
