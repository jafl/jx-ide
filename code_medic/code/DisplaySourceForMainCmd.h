/******************************************************************************
 DisplaySourceForMainCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_DisplaySourceForMain
#define _H_DisplaySourceForMain

#include "Command.h"
#include <jx-af/jcore/JBroadcaster.h>

class SourceDirector;

class DisplaySourceForMainCmd : public Command, virtual public JBroadcaster
{
public:

	DisplaySourceForMainCmd(SourceDirector* sourceDir, const JString& cmd);

	virtual	~DisplaySourceForMainCmd();

	SourceDirector*	GetSourceDir();

private:

	SourceDirector*	itsSourceDir;		// not owned
};


/******************************************************************************
 GetSourceDir

 ******************************************************************************/

inline SourceDirector*
DisplaySourceForMainCmd::GetSourceDir()
{
	return itsSourceDir;
}

#endif
