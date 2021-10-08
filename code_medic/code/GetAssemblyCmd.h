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

	~GetAssemblyCmd() override;

	SourceDirector*	GetDirector();

protected:

	void	HandleFailure() override;

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
