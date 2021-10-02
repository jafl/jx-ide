/******************************************************************************
 JVMGetFullPath.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetFullPath
#define _H_JVMGetFullPath

#include "GetFullPathCmd.h"

class JVMGetFullPath : public GetFullPathCmd
{
public:

	JVMGetFullPath(const JString& fileName, const JIndex lineIndex = 0);

	virtual	~JVMGetFullPath();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
