/******************************************************************************
 CMDisplaySourceForMain.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMDisplaySourceForMain
#define _H_CMDisplaySourceForMain

#include "CMCommand.h"
#include <jx-af/jcore/JBroadcaster.h>

class CMSourceDirector;

class CMDisplaySourceForMain : public CMCommand, virtual public JBroadcaster
{
public:

	CMDisplaySourceForMain(CMSourceDirector* sourceDir, const JString& cmd);

	virtual	~CMDisplaySourceForMain();

	CMSourceDirector*	GetSourceDir();

private:

	CMSourceDirector*	itsSourceDir;		// not owned
};


/******************************************************************************
 GetSourceDir

 ******************************************************************************/

inline CMSourceDirector*
CMDisplaySourceForMain::GetSourceDir()
{
	return itsSourceDir;
}

#endif
