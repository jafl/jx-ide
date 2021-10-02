/******************************************************************************
 LLDBGetFrame.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetFrame
#define _H_LLDBGetFrame

#include "GetFrameCmd.h"

class StackWidget;

class LLDBGetFrame : public GetFrameCmd
{
public:

	LLDBGetFrame(StackWidget* widget);

	virtual	~LLDBGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	StackWidget*	itsWidget;
};

#endif
