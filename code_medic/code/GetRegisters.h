/******************************************************************************
 GetRegisters.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetRegisters
#define _H_GetRegisters

#include "Command.h"

class RegistersDir;

class GetRegisters : public Command
{
public:

	GetRegisters(const JString& cmd, RegistersDir* dir);

	virtual	~GetRegisters();

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
GetRegisters::GetDirector()
{
	return itsDir;
}

#endif
