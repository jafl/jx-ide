/******************************************************************************
 JVMDisplaySourceForMainCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMDisplaySourceForMainCmd
#define _H_JVMDisplaySourceForMainCmd

#include "DisplaySourceForMainCmd.h"

class JVMDisplaySourceForMainCmd : public DisplaySourceForMainCmd
{
public:

	JVMDisplaySourceForMainCmd(SourceDirector* sourceDir);

	virtual	~JVMDisplaySourceForMainCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
