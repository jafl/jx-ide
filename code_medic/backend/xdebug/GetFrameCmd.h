/******************************************************************************
 GetFrameCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetFrameCmd
#define _H_XDGetFrameCmd

#include <GetFrameCmd.h>

class StackWidget;

namespace xdebug {

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
