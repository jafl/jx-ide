/******************************************************************************
 GDBGetStopLocationForLink.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationForLink
#define _H_GDBGetStopLocationForLink

#include "GDBGetStopLocation.h"

class GDBGetStopLocationForLink : public GDBGetStopLocation
{
public:

	GDBGetStopLocationForLink();

	virtual	~GDBGetStopLocationForLink();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
