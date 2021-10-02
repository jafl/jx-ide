/******************************************************************************
 GetMemory.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GetMemory
#define _H_GetMemory

#include "Command.h"

class MemoryDir;

class GetMemory : public Command
{
public:

	GetMemory(MemoryDir* dir);

	virtual	~GetMemory();

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
GetMemory::GetDirector()
{
	return itsDir;
}

#endif
