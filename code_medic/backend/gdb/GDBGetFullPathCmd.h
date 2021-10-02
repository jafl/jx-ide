/******************************************************************************
 GDBGetFullPathCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetFullPath
#define _H_GDBGetFullPath

#include "GetFullPathCmd.h"

class GDBGetFullPathCmd : public GetFullPathCmd
{
public:

	GDBGetFullPathCmd(const JString& fileName, const JIndex lineIndex = 0);

	virtual	~GDBGetFullPathCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	static JString	BuildCommand(const JString& fileName);
};

#endif
