/******************************************************************************
 GetAssemblyCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetAssembly
#define _H_GetAssembly

#include "Command.h"

class SourceDirector;

class GetAssemblyCmd : public Command
{
public:

	GetAssemblyCmd(SourceDirector* dir, const JString& cmd);

	virtual	~GetAssemblyCmd();

	SourceDirector*	GetDirector();

protected:

	virtual void	HandleFailure();

private:

	SourceDirector*	itsDir;
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline SourceDirector*
GetAssemblyCmd::GetDirector()
{
	return itsDir;
}

#endif
