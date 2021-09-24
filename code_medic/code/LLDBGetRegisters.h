/******************************************************************************
 LLDBGetRegisters.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetRegisters
#define _H_LLDBGetRegisters

#include "CMGetRegisters.h"

class LLDBGetRegisters : public CMGetRegisters
{
public:

	LLDBGetRegisters(CMRegistersDir* dir);

	virtual	~LLDBGetRegisters();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
