/******************************************************************************
 LLDBGetFrameCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetFrameCmd
#define _H_LLDBGetFrameCmd

#include "GetFrameCmd.h"

class StackWidget;

namespace lldb {

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
