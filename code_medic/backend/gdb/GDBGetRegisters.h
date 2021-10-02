/******************************************************************************
 GDBGetRegisters.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetRegisters
#define _H_GDBGetRegisters

#include "GetRegisters.h"

class GDBGetRegisters : public GetRegisters
{
public:

	GDBGetRegisters(RegistersDir* dir);

	virtual	~GDBGetRegisters();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
