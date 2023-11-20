/******************************************************************************
 GetFullPathCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetFullPathCmd
#define _H_LLDBGetFullPathCmd

#include <GetFullPathCmd.h>

namespace lldb {

class GetFullPathCmd : public ::GetFullPathCmd
{
public:

	GetFullPathCmd(const JString& fileName, const JIndex lineIndex = 0);

	~GetFullPathCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
