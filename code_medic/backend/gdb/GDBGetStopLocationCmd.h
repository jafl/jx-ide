/******************************************************************************
 GDBGetStopLocationCmd.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStopLocationCmd
#define _H_GDBGetStopLocationCmd

#include "Command.h"

class Location;

namespace gdb {

class GetStopLocationCmd : public Command
{
public:

	GetStopLocationCmd();

	virtual	~GetStopLocationCmd();

protected:

	Location	GetLocation() const;
};

};

#endif
