/******************************************************************************
 GetMemoryCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetMemoryCmd
#define _H_GetMemoryCmd

#include "Command.h"

class MemoryDir;

class GetMemoryCmd : public Command
{
public:

	GetMemoryCmd(MemoryDir* dir);

	virtual	~GetMemoryCmd();

	MemoryDir*	GetDirector();

protected:

	virtual void	HandleFailure();

private:

	MemoryDir*	itsDir;
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline MemoryDir*
GetMemoryCmd::GetDirector()
{
	return itsDir;
}

#endif
