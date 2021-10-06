/******************************************************************************
 XDGetFullPath.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetFullPath
#define _H_XDGetFullPath

#include "GetFullPathCmd.h"

class XDGetFullPath : public GetFullPathCmd
{
public:

	XDGetFullPath(const JString& fileName, const JIndex lineIndex = 0);

	virtual	~XDGetFullPath();

protected:

	void	HandleSuccess(const JString& data) override;
};

#endif
