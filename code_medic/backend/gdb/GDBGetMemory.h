/******************************************************************************
 GDBGetMemory.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetMemory
#define _H_GDBGetMemory

#include "GetMemory.h"

class GDBGetMemory : public GetMemory
{
public:

	GDBGetMemory(MemoryDir* dir);

	virtual	~GDBGetMemory();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
