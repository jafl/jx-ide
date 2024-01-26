/******************************************************************************
 globals.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_globals
#define _H_globals

#include <jx-af/jx/jXGlobals.h>
#include "App.h"
#include "PrefsManager.h"
#include "Link.h"		// for convenience

class JXPSPrinter;
class JX2DPlotEPSPrinter;
class JXPTPrinter;
class CommandDirector;
class FnMenuUpdater;

App*				GetApplication();
bool				HasPrefsManager();
PrefsManager*		GetPrefsManager();
void				ForgetPrefsManager();
JXPTPrinter*		GetPTPrinter();
JXPSPrinter*		GetPSPrinter();
JX2DPlotEPSPrinter*	GetPlotEPSPrinter();
FnMenuUpdater*		GetFnMenuUpdater();

void				StartDebugger();
Link*				GetLink();
CommandDirector*	GetCommandDirector();

const JString&		GetVersionNumberStr();
JString				GetVersionStr();

bool				IsShuttingDown();

	// icons

const JXImage*	GetCommandLineIcon();
const JXImage*	GetCurrentSourceIcon();
const JXImage*	GetCurrentAsmIcon();
const JXImage*	GetThreadsIcon();
const JXImage*	GetStackTraceIcon();
const JXImage*	GetBreakpointsIcon();
const JXImage*	GetVariablesIcon();
const JXImage*	GetLocalVarsIcon();
const JXImage*	GetFileListIcon();
const JXImage*	GetSourceFileIcon();
const JXImage*	GetAsmSourceIcon();
const JXImage*	GetArray1DIcon();
const JXImage*	GetArray2DIcon();
const JXImage*	GetPlot2DIcon();
const JXImage*	GetMemoryIcon();
const JXImage*	GetRegistersIcon();

	// called by App

bool	CreateGlobals(App* app);
void	CreateCommandDirector();
void	DeleteGlobals();
void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

// CtagsUser

bool	InUpdateThread();

#endif
