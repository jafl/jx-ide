/******************************************************************************
 GetFullPathCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetFullPath
#define _H_GDBGetFullPath

#include <GetFullPathCmd.h>

namespace gdb {

class GetFullPathCmd : public ::GetFullPathCmd
{
public:

	GetFullPathCmd(const JString& fileName, const JIndex lineIndex = 0);

	~GetFullPathCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	static JString	BuildCommand(const JString& fileName);
};

};

#endif
