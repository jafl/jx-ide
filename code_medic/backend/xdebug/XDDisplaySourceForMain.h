/******************************************************************************
 XDDisplaySourceForMain.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDDisplaySourceForMain
#define _H_XDDisplaySourceForMain

#include "DisplaySourceForMainCmd.h"

class XDDisplaySourceForMain : public DisplaySourceForMainCmd
{
public:

	XDDisplaySourceForMain(SourceDirector* sourceDir);

	virtual	~XDDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
