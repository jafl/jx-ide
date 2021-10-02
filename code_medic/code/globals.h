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
PrefsManager*		GetPrefsManager();
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

	// called by Directors

const JUtf8Byte*	GetWMClassInstance();
const JUtf8Byte*	GetCommandWindowClass();
const JUtf8Byte*	GetSourceViewWindowClass();
const JUtf8Byte*	GetAsmViewWindowClass();
const JUtf8Byte*	GetMainSourceWindowClass();
const JUtf8Byte*	GetMainAsmWindowClass();
const JUtf8Byte*	GetThreadWindowClass();
const JUtf8Byte*	GetStackWindowClass();
const JUtf8Byte*	GetBreakpointsWindowClass();
const JUtf8Byte*	GetVariableWindowClass();
const JUtf8Byte*	GetLocalVariableWindowClass();
const JUtf8Byte*	GetArray1DWindowClass();
const JUtf8Byte*	GetArray2DWindowClass();
const JUtf8Byte*	GetPlot2DWindowClass();
const JUtf8Byte*	GetFileListWindowClass();
const JUtf8Byte*	GetMemoryWindowClass();
const JUtf8Byte*	GetRegistersWindowClass();
const JUtf8Byte*	GetDebugWindowClass();

// CtagsUser

bool	InUpdateThread();

#endif
