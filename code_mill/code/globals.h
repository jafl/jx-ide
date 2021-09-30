/******************************************************************************
 globals.h

   Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_globals
#define _H_globals

#include <jx-af/jx/jXGlobals.h>

// we include these for convenience

#include "App.h"
#include "PrefsManager.h"
#include <jx-af/jx/JXDocumentManager.h>

class MDIServer;

App*				GetApplication();
PrefsManager*	GetPrefsManager();
MDIServer*		GetMDIServer();

const JString&		GetVersionNumberStr();
JString				GetVersionStr();

	// called by App

bool	CreateGlobals(App* app);
void		DeleteGlobals();
void		CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

#endif
