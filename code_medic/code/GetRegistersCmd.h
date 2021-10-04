/******************************************************************************
 GetRegistersCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetRegistersCmd
#define _H_GetRegistersCmd

#include "Command.h"

class RegistersDir;

class GetRegistersCmd : public Command
{
public:

	GetRegistersCmd(const JString& cmd, RegistersDir* dir);

	virtual	~GetRegistersCmd();

	RegistersDir*	GetDirector();

protected:

	virtual void	HandleFailure();

private:

	RegistersDir*	itsDir;
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline RegistersDir*
GetRegistersCmd::GetDirector()
{
	return itsDir;
}

#endif