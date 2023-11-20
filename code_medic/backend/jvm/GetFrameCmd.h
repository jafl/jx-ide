/******************************************************************************
 GetFrameCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetFrameCmd
#define _H_JVMGetFrameCmd

#include <GetFrameCmd.h>

class StackWidget;

namespace jvm {

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
