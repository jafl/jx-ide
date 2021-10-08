/******************************************************************************
 globals.h

	Copyright (C) 1997-02 by Glenn W. Bach.

 ******************************************************************************/

#include "globals.h"
#include "DockManager.h"
#include "FnMenuUpdater.h"
#include "gdb/GDBLink.h"
#include "lldb/LLDBLink.h"
#include "jvm/JVMLink.h"
#include "xdebug/XDLink.h"
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

static Link*				theLink   = nullptr;
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

static const JUtf8Byte* kCommandWindowClass     = "Code_Medic_Command_Line";
static const JUtf8Byte* kSourceWindowClass      = "Code_Medic_Source";
static const JUtf8Byte* kAsmWindowClass         = "Code_Medic_Disassembly";
static const JUtf8Byte* kMainSourceWindowClass  = "Code_Medic_Source_Main";
static const JUtf8Byte* kMainAsmWindowClass     = "Code_Medic_Disassembly_Main";
static const JUtf8Byte* kThreadWindowClass      = "Code_Medic_Threads";
static const JUtf8Byte* kStackWindowClass       = "Code_Medic_Stack";
static const JUtf8Byte* kBreakpointsWindowClass = "Code_Medic_Breakpoints";
static const JUtf8Byte* kVariableWindowClass    = "Code_Medic_Variables";
static const JUtf8Byte* kLocalVarsWindowClass   = "Code_Medic_Variables_Local";
static const JUtf8Byte* kArray1DWindowClass     = "Code_Medic_Variables_Array_1D";
static const JUtf8Byte* kArray2DWindowClass     = "Code_Medic_Variables_Array_2D";
static const JUtf8Byte* kPlot2DWindowClass      = "Code_Medic_Variables_Plot_2D";
static const JUtf8Byte* kFileListWindowClass    = "Code_Medic_File_List";
static const JUtf8Byte* kMemoryWindowClass      = "Code_Medic_Memory";
static const JUtf8Byte* kRegistersWindowClass   = "Code_Medic_Registers";
static const JUtf8Byte* kDebugWindowClass       = "Code_Medic_Debug";

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
		oldPrefsFile = JCombinePathAndName(oldPrefsFile, JString(".gMedic.pref", JString::kNoCopy));
		if (JFileExists(oldPrefsFile) &&
			(JPrefsFile::GetFullName(app->GetSignature(), &newPrefsFile)).OK() &&
			!JFileExists(newPrefsFile))
		{
			JRenameFile(oldPrefsFile, newPrefsFile);
		}
	}

	bool isNew;
	thePrefsManager = jnew PrefsManager(&isNew);
	assert(thePrefsManager != nullptr);

	JXInitHelp();

	auto* wdMgr = jnew JXWDManager(display, true);
	assert( wdMgr != nullptr );

	auto* dockManager = jnew DockManager;
	assert( dockManager != nullptr );
	dockManager->JPrefObject::ReadPrefs();

	theTextPrinter = jnew JXPTPrinter;
	assert( theTextPrinter != nullptr );
	thePrefsManager->ReadPrinterSetup(theTextPrinter);

	thePSPrinter = jnew JXPSPrinter(display);
	assert( thePSPrinter != nullptr );
	thePrefsManager->ReadPrinterSetup(thePSPrinter);

	thePlotEPSPrinter = jnew JX2DPlotEPSPrinter(display);
	assert( thePlotEPSPrinter != nullptr );
	thePrefsManager->ReadPrinterSetup(thePlotEPSPrinter);

	theFnMenuUpdater = jnew FnMenuUpdater;
	assert( theFnMenuUpdater != nullptr );

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
	assert( theCmdDir != nullptr );
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

	thePrefsManager->WritePrinterSetup(theTextPrinter);
	jdelete theTextPrinter;
	theTextPrinter = nullptr;

	thePrefsManager->WritePrinterSetup(thePSPrinter);
	jdelete thePSPrinter;
	thePSPrinter = nullptr;

	thePrefsManager->WritePrinterSetup(thePlotEPSPrinter);
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
	theLink          = nullptr;

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
	assert (theLink != nullptr);

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
 GetPrefsManager

 ******************************************************************************/

PrefsManager*
GetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
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
 GetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
GetWMClassInstance()
{
	return JGetString("Name").GetBytes();
}

const JUtf8Byte*
GetCommandWindowClass()
{
	return kCommandWindowClass;
}

const JUtf8Byte*
GetSourceViewWindowClass()
{
	return kSourceWindowClass;
}

const JUtf8Byte*
GetAsmViewWindowClass()
{
	return kAsmWindowClass;
}

const JUtf8Byte*
GetMainSourceWindowClass()
{
	return kMainSourceWindowClass;
}

const JUtf8Byte*
GetMainAsmWindowClass()
{
	return kMainAsmWindowClass;
}

const JUtf8Byte*
GetThreadWindowClass()
{
	return kThreadWindowClass;
}

const JUtf8Byte*
GetStackWindowClass()
{
	return kStackWindowClass;
}

const JUtf8Byte*
GetBreakpointsWindowClass()
{
	return kBreakpointsWindowClass;
}

const JUtf8Byte*
GetVariableWindowClass()
{
	return kVariableWindowClass;
}

const JUtf8Byte*
GetLocalVariableWindowClass()
{
	return kLocalVarsWindowClass;
}

const JUtf8Byte*
GetArray1DWindowClass()
{
	return kArray1DWindowClass;
}

const JUtf8Byte*
GetArray2DWindowClass()
{
	return kArray2DWindowClass;
}

const JUtf8Byte*
GetPlot2DWindowClass()
{
	return kPlot2DWindowClass;
}

const JUtf8Byte*
GetFileListWindowClass()
{
	return kFileListWindowClass;
}

const JUtf8Byte*
GetMemoryWindowClass()
{
	return kMemoryWindowClass;
}

const JUtf8Byte*
GetRegistersWindowClass()
{
	return kRegistersWindowClass;
}

const JUtf8Byte*
GetDebugWindowClass()		// required so Enlightenment will allow it to resize
{
	return kDebugWindowClass;
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
#include <jx_source_file_small.xpm>
#include <jx_binary_file_small.xpm>
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
