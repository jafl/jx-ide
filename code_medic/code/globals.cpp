/******************************************************************************
 globals.h

	Copyright (C) 1997-02 by Glenn W. Bach.

 ******************************************************************************/

#include "globals.h"
#include "DockManager.h"
#include "FnMenuUpdater.h"
#include "gdb/Link.h"
#include "lldb/Link.h"
#include "jvm/Link.h"
#include "xdebug/Link.h"
#include "CommandDirector.h"
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/j2dplot/JX2DPlotEPSPrinter.h>
#include <jx-af/jx/JXPTPrinter.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

static App*					theApplication		= nullptr;		// owned by JX
static bool					theShutdownFlag     = false;
static PrefsManager*		thePrefsManager		= nullptr;
static JXPTPrinter*			theTextPrinter		= nullptr;
static JXPSPrinter*			thePSPrinter		= nullptr;
static JX2DPlotEPSPrinter*	thePlotEPSPrinter	= nullptr;
static FnMenuUpdater*		theFnMenuUpdater	= nullptr;

static Link*			theLink   = nullptr;
static CommandDirector*	theCmdDir = nullptr;

// owned by JXImageCache
static JXImage* theCommandLineIcon   = nullptr;
static JXImage* theCurrentSourceIcon = nullptr;
static JXImage* theCurrentAsmIcon    = nullptr;
static JXImage* theThreadsIcon       = nullptr;
static JXImage* theStackTraceIcon    = nullptr;
static JXImage* theBreakpointsIcon   = nullptr;
static JXImage* theVariablesIcon     = nullptr;
static JXImage* theLocalVarsIcon     = nullptr;
static JXImage* theFileListIcon      = nullptr;
static JXImage* theSourceFileIcon    = nullptr;
static JXImage* theAsmSourceIcon     = nullptr;
static JXImage* theArray1DIcon       = nullptr;
static JXImage* theArray2DIcon       = nullptr;
static JXImage* thePlot2DIcon        = nullptr;
static JXImage* theMemoryIcon        = nullptr;
static JXImage* theRegistersIcon     = nullptr;

// private functions

void	CreateIcons();
void	DeleteIcons();

/******************************************************************************
 CreateGlobals

 ******************************************************************************/

bool
CreateGlobals
	(
	App* app
	)
{
	theApplication = app;

	JXDisplay* display = app->GetCurrentDisplay();

	CreateIcons();

	JString oldPrefsFile, newPrefsFile;
	if (JGetPrefsDirectory(&oldPrefsFile))
	{
		oldPrefsFile = JCombinePathAndName(oldPrefsFile, ".gMedic.pref");
		if (JFileExists(oldPrefsFile) &&
			(JPrefsFile::GetFullName(app->GetSignature(), &newPrefsFile)).OK() &&
			!JFileExists(newPrefsFile))
		{
			JRenameFile(oldPrefsFile, newPrefsFile);
		}
	}

	bool isNew;
	thePrefsManager = jnew PrefsManager(&isNew);

	JXInitHelp();

	auto* wdMgr = jnew JXWDManager(display, true);

	auto* dockManager = jnew DockManager;
	dockManager->JPrefObject::ReadPrefs();

	theTextPrinter = jnew JXPTPrinter;
	thePrefsManager->ReadPrinterSetup(theTextPrinter);

	thePSPrinter = jnew JXPSPrinter(display);
	thePrefsManager->ReadPrinterSetup(thePSPrinter);

	thePlotEPSPrinter = jnew JX2DPlotEPSPrinter(display);
	thePrefsManager->ReadPrinterSetup(thePlotEPSPrinter);

	theFnMenuUpdater = jnew FnMenuUpdater;

	thePrefsManager->LoadSearchPrefs();
	thePrefsManager->SyncWithCodeCrusader();	// after creating search dialog

	lldb::SBDebugger::Initialize();
	StartDebugger();

	return isNew;
}

/******************************************************************************
 CreateCommandDirector

 ******************************************************************************/

void
CreateCommandDirector()
{
	theCmdDir = jnew CommandDirector(theApplication);
	theCmdDir->Activate();
}

/******************************************************************************
 DeleteGlobals

 ******************************************************************************/

void
DeleteGlobals()
{
	theShutdownFlag = true;

	JXGetDockManager()->JPrefObject::WritePrefs();

	if (thePrefsManager != nullptr)
	{
		thePrefsManager->WritePrinterSetup(theTextPrinter);
	}
	jdelete theTextPrinter;
	theTextPrinter = nullptr;

	if (thePrefsManager != nullptr)
	{
		thePrefsManager->WritePrinterSetup(thePSPrinter);
	}
	jdelete thePSPrinter;
	thePSPrinter = nullptr;

	if (thePrefsManager != nullptr)
	{
		thePrefsManager->WritePrinterSetup(thePlotEPSPrinter);
	}
	jdelete thePlotEPSPrinter;
	thePlotEPSPrinter = nullptr;

	jdelete theFnMenuUpdater;
	theFnMenuUpdater = nullptr;

	jdelete theLink;
	theLink = nullptr;
	lldb::SBDebugger::Terminate();

	DeleteIcons();
	ShutdownStylers();

	theApplication = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 CleanUpBeforeSuddenDeath

	This must be the last one called by CBApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
	{
		ShutdownStylers();
	}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 StartDebugger

 ******************************************************************************/

void
StartDebugger()
{
	Link* origLink = theLink;
	theLink        = nullptr;

	PrefsManager::DebuggerType type = GetPrefsManager()->GetDebuggerType();
	if (type == PrefsManager::kGDBType)
	{
		theLink = jnew gdb::Link;
	}
	else if (type == PrefsManager::kLLDBType)
	{
		theLink = jnew lldb::Link;
	}
	else if (type == PrefsManager::kJavaType)
	{
		theLink = jnew jvm::Link;
	}
	else if (type == PrefsManager::kXdebugType)
	{
		theLink = jnew xdebug::Link;
	}
	assert( theLink != nullptr );

	// original must be deleted *last* so listeners can call GetLink() to
	// get the new one

	jdelete origLink;

	if (theCmdDir != nullptr)
	{
		theCmdDir->InitializeCommandOutput();
	}
}

/******************************************************************************
 IsShuttingDown

 ******************************************************************************/

bool
IsShuttingDown()
{
	return theShutdownFlag;
}

/******************************************************************************
 GetApplication

 ******************************************************************************/

App*
GetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 HasPrefsManager

 ******************************************************************************/

bool
HasPrefsManager()
{
	return thePrefsManager != nullptr;
}

/******************************************************************************
 GetPrefsManager

 ******************************************************************************/

PrefsManager*
GetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 ForgetPrefsManager

	Called when license is not accepted, to avoid writing prefs file.

 ******************************************************************************/

void
ForgetPrefsManager()
{
	thePrefsManager = nullptr;
}

/******************************************************************************
 GetPTPrinter

 ******************************************************************************/

JXPTPrinter*
GetPTPrinter()
{
	assert( theTextPrinter != nullptr );
	return theTextPrinter;
}

/******************************************************************************
 GetPSPrinter

 ******************************************************************************/

JXPSPrinter*
GetPSPrinter()
{
	assert( thePSPrinter != nullptr );
	return thePSPrinter;
}

/******************************************************************************
 GetPlotEPSPrinter

 ******************************************************************************/

JX2DPlotEPSPrinter*
GetPlotEPSPrinter()
{
	assert( thePlotEPSPrinter != nullptr );
	return thePlotEPSPrinter;
}

/******************************************************************************
 GetFnMenuUpdater

 ******************************************************************************/

FnMenuUpdater*
GetFnMenuUpdater()
{
	assert( theFnMenuUpdater != nullptr );
	return theFnMenuUpdater;
}

/******************************************************************************
 GetLink

 *****************************************************************************/

Link*
GetLink()
{
	assert( theLink != nullptr );
	return theLink;
}

/******************************************************************************
 GetCommandDirector

 ******************************************************************************/

CommandDirector*
GetCommandDirector()
{
	assert( theCmdDir != nullptr );
	return theCmdDir;
}

/******************************************************************************
 GetVersionNumberStr

 ******************************************************************************/

const JString&
GetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 GetVersionStr

 ******************************************************************************/

JString
GetVersionStr()
{
	const JUtf8Byte* map[] =
	{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
	};
	return JGetString("Description", map, sizeof(map));
}

/******************************************************************************
 CBInUpdateThread

 ******************************************************************************/

bool
InUpdateThread()
{
	return false;
}

/******************************************************************************
 Icons

 ******************************************************************************/

#include "medic_show_command_line.xpm"
#include "medic_show_current_source.xpm"
#include "medic_show_current_asm.xpm"
#include "medic_show_threads.xpm"
#include "medic_show_stack_trace.xpm"
#include "medic_show_breakpoints.xpm"
#include "medic_show_variables.xpm"
#include "medic_show_local_variables.xpm"
#include "medic_show_file_list.xpm"
#include <jx-af/image/jx/jx_source_file_small.xpm>
#include <jx-af/image/jx/jx_binary_file_small.xpm>
#include "medic_show_1d_array.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_memory.xpm"
#include "medic_show_registers.xpm"

void
CreateIcons()
{
	JXImageCache* c = theApplication->GetCurrentDisplay()->GetImageCache();

	theCommandLineIcon   = c->GetImage(medic_show_command_line);
	theCurrentSourceIcon = c->GetImage(medic_show_current_source);
	theCurrentAsmIcon    = c->GetImage(medic_show_current_asm);
	theThreadsIcon       = c->GetImage(medic_show_threads);
	theStackTraceIcon    = c->GetImage(medic_show_stack_trace);
	theBreakpointsIcon   = c->GetImage(medic_show_breakpoints);
	theVariablesIcon     = c->GetImage(medic_show_variables);
	theLocalVarsIcon     = c->GetImage(medic_show_local_variables);
	theFileListIcon      = c->GetImage(medic_show_file_list);
	theSourceFileIcon    = c->GetImage(jx_source_file_small);
	theAsmSourceIcon     = c->GetImage(jx_binary_file_small);
	theArray1DIcon       = c->GetImage(medic_show_1d_array);
	theArray2DIcon       = c->GetImage(medic_show_2d_array);
	thePlot2DIcon        = c->GetImage(medic_show_2d_plot);
	theMemoryIcon        = c->GetImage(medic_show_memory);
	theRegistersIcon     = c->GetImage(medic_show_registers);
}

void
DeleteIcons()
{
}

const JXImage*
GetCommandLineIcon()
{
	return theCommandLineIcon;
}

const JXImage*
GetCurrentSourceIcon()
{
	return theCurrentSourceIcon;
}

const JXImage*
GetCurrentAsmIcon()
{
	return theCurrentAsmIcon;
}

const JXImage*
GetThreadsIcon()
{
	return theThreadsIcon;
}

const JXImage*
GetStackTraceIcon()
{
	return theStackTraceIcon;
}

const JXImage*
GetBreakpointsIcon()
{
	return theBreakpointsIcon;
}

const JXImage*
GetVariablesIcon()
{
	return theVariablesIcon;
}

const JXImage*
GetLocalVarsIcon()
{
	return theLocalVarsIcon;
}

const JXImage*
GetFileListIcon()
{
	return theFileListIcon;
}

const JXImage*
GetSourceFileIcon()
{
	return theSourceFileIcon;
}

const JXImage*
GetAsmSourceIcon()
{
	return theAsmSourceIcon;
}

const JXImage*
GetArray1DIcon()
{
	return theArray1DIcon;
}

const JXImage*
GetArray2DIcon()
{
	return theArray2DIcon;
}

const JXImage*
GetPlot2DIcon()
{
	return thePlot2DIcon;
}

const JXImage*
GetMemoryIcon()
{
	return theMemoryIcon;
}

const JXImage*
GetRegistersIcon()
{
	return theRegistersIcon;
}
