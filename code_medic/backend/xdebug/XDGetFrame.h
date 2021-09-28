/******************************************************************************
 XDGetFrame.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetFrame
#define _H_XDGetFrame

#include "CMGetFrame.h"

class CMStackWidget;

class XDGetFrame : public CMGetFrame
{
public:

	XDGetFrame(CMStackWidget* widget);

	virtual	~XDGetFrame();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	CMStackWidget*	itsWidget;
};

#endif
