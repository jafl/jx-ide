/******************************************************************************
 JVMDisplaySourceForMain.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMDisplaySourceForMain
#define _H_JVMDisplaySourceForMain

#include "DisplaySourceForMainCmd.h"

class JVMDisplaySourceForMain : public DisplaySourceForMainCmd
{
public:

	JVMDisplaySourceForMain(SourceDirector* sourceDir);

	virtual	~JVMDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
