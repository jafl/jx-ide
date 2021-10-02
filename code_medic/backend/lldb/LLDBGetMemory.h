/******************************************************************************
 LLDBGetMemory.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetMemory
#define _H_LLDBGetMemory

#include "GetMemory.h"

class LLDBGetMemory : public GetMemory
{
public:

	LLDBGetMemory(MemoryDir* dir);

	virtual	~LLDBGetMemory();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
