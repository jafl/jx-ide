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

	~GetMemoryCmd() override;

	MemoryDir*	GetDirector();

protected:

	void	HandleFailure() override;

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
