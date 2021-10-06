/******************************************************************************
 LLDBGetMemory.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetMemory
#define _H_LLDBGetMemory

#include "GetMemoryCmd.h"

class LLDBGetMemory : public GetMemoryCmd
{
public:

	LLDBGetMemory(MemoryDir* dir);

	virtual	~LLDBGetMemory();

protected:

	void	HandleSuccess(const JString& data) override;
};

#endif
