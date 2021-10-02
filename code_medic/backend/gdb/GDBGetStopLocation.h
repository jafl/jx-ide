/******************************************************************************
 GDBGetStopLocation.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocation
#define _H_GDBGetStopLocation

#include "Command.h"

class Location;

class GDBGetStopLocation : public Command
{
public:

	GDBGetStopLocation();

	virtual	~GDBGetStopLocation();

protected:

	Location	GetLocation() const;
};

#endif
