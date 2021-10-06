/******************************************************************************
 LLDBGetRegisters.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetRegisters
#define _H_LLDBGetRegisters

#include "GetRegistersCmd.h"

class LLDBGetRegisters : public GetRegistersCmd
{
public:

	LLDBGetRegisters(RegistersDir* dir);

	virtual	~LLDBGetRegisters();

protected:

	void	HandleSuccess(const JString& data) override;
};

#endif
