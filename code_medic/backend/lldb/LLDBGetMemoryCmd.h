/******************************************************************************
 LLDBGetMemoryCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetMemoryCmd
#define _H_LLDBGetMemoryCmd

#include "GetMemoryCmd.h"

namespace lldb {

class GetMemoryCmd : public ::GetMemoryCmd
{
public:

	GetMemoryCmd(MemoryDir* dir);

	~GetMemoryCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
