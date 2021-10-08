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

	~GetFrameCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	StackWidget*	itsWidget;
};

};

#endif
