/******************************************************************************
 XDDisplaySourceForMain.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDDisplaySourceForMain
#define _H_XDDisplaySourceForMain

#include "CMDisplaySourceForMain.h"

class XDDisplaySourceForMain : public CMDisplaySourceForMain
{
public:

	XDDisplaySourceForMain(CMSourceDirector* sourceDir);

	virtual	~XDDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
