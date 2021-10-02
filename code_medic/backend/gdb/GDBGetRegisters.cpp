/******************************************************************************
 GDBGetRegisters.cpp

	BASE CLASS = GetRegisters

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GDBGetRegisters.h"
#include "RegistersDir.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetRegisters::GDBGetRegisters
	(
	RegistersDir* dir
	)
	:
	GetRegisters(JString("info all-registers", JString::kNoCopy), dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetRegisters::~GDBGetRegisters()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBGetRegisters::HandleSuccess
	(
	const JString& data
	)
{
	GetDirector()->Update(data);
}
