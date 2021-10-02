/******************************************************************************
 XDGetFrame.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetFrame
#define _H_XDGetFrame

#include "GetFrameCmd.h"

class StackWidget;

class XDGetFrame : public GetFrameCmd
{
public:

	XDGetFrame(StackWidget* widget);

	virtual	~XDGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	StackWidget*	itsWidget;
};

#endif
