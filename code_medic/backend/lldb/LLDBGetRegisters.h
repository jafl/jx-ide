/******************************************************************************
 LLDBGetRegisters.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetRegisters
#define _H_LLDBGetRegisters

#include "GetRegisters.h"

class LLDBGetRegisters : public GetRegisters
{
public:

	LLDBGetRegisters(RegistersDir* dir);

	virtual	~LLDBGetRegisters();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
