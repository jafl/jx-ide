/******************************************************************************
 globals.cpp

	Access to global objects and factories.

	Copyright © 1997-99 John Lindal.

 ******************************************************************************/

#include "globals.h"
#include "MDIServer.h"
#include "DockManager.h"
#include "SearchTextDialog.h"
#include "RunTEScriptDialog.h"
#include "ViewManPageDialog.h"
#include "FindFileDialog.h"
#include "DiffFileDialog.h"
#include "CommandManager.h"
#include "FnMenuUpdater.h"
#include "SymbolTypeList.h"
#include "PTPrinter.h"
#include "PSPrinter.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/jAssert.h>

static App*				theApplication       = nullptr;	// owns itself
static DocumentManager*	theDocManager        = nullptr;	// owned by JX
static PrefsManager*		thePrefsManager      = nullptr;
static MDIServer*			theMDIServer         = nullptr;	// owned by JX

static SearchTextDialog*	theSearchTextDialog  = nullptr;	// owned by JXGetPersistentWindowOwner()
static RunTEScriptDialog*	theRunTEScriptDialog = nullptr;	// owned by JXGetPersistentWindowOwner()
static ViewManPageDialog*	theViewManPageDialog = nullptr;	// owned by JXGetPersistentWindowOwner()
static FindFileDialog*	theFindFileDialog    = nullptr;	// owned by JXGetPersistentWindowOwner()
static DiffFileDialog*	theDiffFileDialog    = nullptr;	// owned by JXGetPersistentWindowOwner()

static CommandManager*	theCmdManager        = nullptr;
static FnMenuUpdater*		theFnMenuUpdater     = nullptr;
static SymbolTypeList*	theSymbolTypeList    = nullptr;

static PTPrinter*			thePTTextPrinter     = nullptr;
static PSPrinter*			thePSTextPrinter     = nullptr;

static const JUtf8Byte* kProjectWindowClass       = "Code_Crusader_Project";
static const JUtf8Byte* kSymbolWindowClass        = "Code_Crusader_Symbol_List";
static const JUtf8Byte* kTreeWindowClass          = "Code_Crusader_Tree";
static const JUtf8Byte* kFileListWindowClass      = "Code_Crusader_File_List";
static const JUtf8Byte* kEditorWindowClass        = "Code_Crusader_Editor";
static const JUtf8Byte* kExecOutputWindowClass    = "Code_Crusader_Editor_Exec_Output";
static const JUtf8Byte* kCompileOutputWindowClass = "Code_Crusader_Editor_Compile_Output";
static const JUtf8Byte* kSearchOutputWindowClass  = "Code_Crusader_Editor_Search_Output";
static const JUtf8Byte* kShellWindowClass         = "Code_Crusader_Editor_Shell";

// owned by JXImageCache
static JXImage* theFileIcon           = nullptr;
static JXImage* theWritableFileIcon   = nullptr;
static JXImage* theSourceIcon         = nullptr;
static JXImage* theWritableSourceIcon = nullptr;
static JXImage* theLibraryIcon        = nullptr;
static JXImage* theProjectIcon        = nullptr;
static JXImage* theActiveProjectIcon  = nullptr;
static JXImage* theActiveListIcon     = nullptr;

static bool theIsUpdateThread = false;

// private functions

void	CreateIcons();
void	DeleteIcons();

/******************************************************************************
 CreateGlobals

	Returns true if this is the first time the program is run.

 ******************************************************************************/

bool
CreateGlobals
	(
	App*			app,
	const bool	useMDI
	)
{
	if (JAssertBase::GetAction() == JAssertBase::kAskUser)
	{
		JAssertBase::SetAction(JAssertBase::kAbort);
	}

	theApplication = app;

	CreateIcons();

	// prefs must be created first so everybody can read from it

	bool isNew;
	thePrefsManager = jnew PrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theDocManager = jnew DocumentManager;
	assert( theDocManager != nullptr );

	if (useMDI)
	{
		auto* dockManager = jnew DockManager();
		assert( dockManager != nullptr );
		dockManager->JPrefObject::ReadPrefs();

		theMDIServer = jnew MDIServer;
		assert( theMDIServer != nullptr );
	}

	theSearchTextDialog = SearchTextDialog::Create();
	assert( theSearchTextDialog != nullptr );

	theRunTEScriptDialog = jnew RunTEScriptDialog(JXGetPersistentWindowOwner());
	assert( theRunTEScriptDialog != nullptr );

	theViewManPageDialog = jnew ViewManPageDialog(JXGetPersistentWindowOwner());
	assert( theViewManPageDialog != nullptr );

	theFindFileDialog = jnew FindFileDialog(JXGetPersistentWindowOwner());
	assert( theFindFileDialog != nullptr );

	theDiffFileDialog = jnew DiffFileDialog(JXGetPersistentWindowOwner());
	assert( theDiffFileDialog != nullptr );

	// widgets hidden in permanent window

	JXWindow* permWindow = theSearchTextDialog->GetWindow();

	theDocManager->CreateFileHistoryMenus(permWindow);

	// global commands

	theCmdManager = jnew CommandManager(theDocManager);
	assert( theCmdManager != nullptr );
	theCmdManager->JPrefObject::ReadPrefs();

	// fn menu updater

	theFnMenuUpdater = jnew FnMenuUpdater;
	assert( theFnMenuUpdater != nullptr );

	// symbol type list

	theSymbolTypeList = jnew SymbolTypeList(permWindow->GetDisplay());
	assert( theSymbolTypeList != nullptr );

	// printers

	thePTTextPrinter = jnew PTPrinter;
	assert( thePTTextPrinter != nullptr );

	thePSTextPrinter = jnew PSPrinter(permWindow->GetDisplay());
	assert( thePSTextPrinter != nullptr );

	return isNew;
}

/******************************************************************************
 DeleteGlobals

 ******************************************************************************/

void
DeleteGlobals()
{
	theDocManager->JPrefObject::WritePrefs();

	JXDockManager* dockMgr;
	if (JXGetDockManager(&dockMgr))
	{
		dockMgr->JPrefObject::WritePrefs();
	}

	if (theMDIServer != nullptr)
	{
		theMDIServer->JPrefObject::WritePrefs();
	}

	DeleteIcons();

	theApplication = nullptr;
	theDocManager  = nullptr;
	theMDIServer   = nullptr;

	theCmdManager->JPrefObject::WritePrefs();
	jdelete theCmdManager;
	theCmdManager = nullptr;

	jdelete theFnMenuUpdater;
	theFnMenuUpdater = nullptr;

	jdelete theSymbolTypeList;
	theSymbolTypeList = nullptr;

	// printers

	jdelete thePTTextPrinter;
	thePTTextPrinter = nullptr;

	jdelete thePSTextPrinter;
	thePSTextPrinter = nullptr;

	ShutdownCompleters();
	ShutdownStylers();

	// windows closed by JXGetPersistentWindowOwner()

	theSearchTextDialog->JPrefObject::WritePrefs();
	theRunTEScriptDialog->JPrefObject::WritePrefs();
	theViewManPageDialog->JPrefObject::WritePrefs();
	theFindFileDialog->JPrefObject::WritePrefs();
	theDiffFileDialog->JPrefObject::WritePrefs();

	theSearchTextDialog  = nullptr;
	theRunTEScriptDialog = nullptr;
	theViewManPageDialog = nullptr;
	theFindFileDialog    = nullptr;
	theDiffFileDialog    = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
}

/******************************************************************************
 CleanUpBeforeSuddenDeath

	This must be the last one called by App so we can save
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
		theSearchTextDialog->JPrefObject::WritePrefs();
		theRunTEScriptDialog->JPrefObject::WritePrefs();
		theViewManPageDialog->JPrefObject::WritePrefs();
		theFindFileDialog->JPrefObject::WritePrefs();
		theDiffFileDialog->JPrefObject::WritePrefs();

		theDocManager->JPrefObject::WritePrefs();

		if (theMDIServer != nullptr)
		{
			theMDIServer->JPrefObject::WritePrefs();
		}

		ShutdownCompleters();
		ShutdownStylers();
	}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
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
 InUpdateThread

 ******************************************************************************/

bool
InUpdateThread()
{
	return theIsUpdateThread;
}

/******************************************************************************
 SetUpdateThread

 ******************************************************************************/

void
SetUpdateThread()
{
	theIsUpdateThread = true;
}

/******************************************************************************
 GetDocumentManager

 ******************************************************************************/

DocumentManager*
GetDocumentManager()
{
	assert( theDocManager != nullptr );
	return theDocManager;
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
 GetMDIServer

 ******************************************************************************/

bool
GetMDIServer
	(
	MDIServer** mdi
	)
{
	*mdi = theMDIServer;
	return theMDIServer != nullptr;
}

/******************************************************************************
 GetSearchTextDialog

 ******************************************************************************/

SearchTextDialog*
GetSearchTextDialog()
{
	assert( theSearchTextDialog != nullptr );
	return theSearchTextDialog;
}

/******************************************************************************
 GetRunTEScriptDialog

 ******************************************************************************/

RunTEScriptDialog*
GetRunTEScriptDialog()
{
	assert( theRunTEScriptDialog != nullptr );
	return theRunTEScriptDialog;
}

/******************************************************************************
 GetViewManPageDialog

 ******************************************************************************/

ViewManPageDialog*
GetViewManPageDialog()
{
	assert( theViewManPageDialog != nullptr );
	return theViewManPageDialog;
}

/******************************************************************************
 GetFindFileDialog

 ******************************************************************************/

FindFileDialog*
GetFindFileDialog()
{
	assert( theFindFileDialog != nullptr );
	return theFindFileDialog;
}

/******************************************************************************
 GetDiffFileDialog

 ******************************************************************************/

DiffFileDialog*
GetDiffFileDialog()
{
	assert( theDiffFileDialog != nullptr );
	return theDiffFileDialog;
}

/******************************************************************************
 GetCommandManager

 ******************************************************************************/

CommandManager*
GetCommandManager()
{
	assert( theCmdManager != nullptr );
	return theCmdManager;
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
 GetSymbolTypeList

 ******************************************************************************/

SymbolTypeList*
GetSymbolTypeList()
{
	assert( theSymbolTypeList != nullptr );
	return theSymbolTypeList;
}

/******************************************************************************
 GetPTTextPrinter

 ******************************************************************************/

PTPrinter*
GetPTTextPrinter()
{
	assert( thePTTextPrinter != nullptr );
	return thePTTextPrinter;
}

/******************************************************************************
 GetPSTextPrinter

 ******************************************************************************/

PSPrinter*
GetPSTextPrinter()
{
	assert( thePSTextPrinter != nullptr );
	return thePSTextPrinter;
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
GetProjectWindowClass()
{
	return kProjectWindowClass;
}

const JUtf8Byte*
GetSymbolWindowClass()
{
	return kSymbolWindowClass;
}

const JUtf8Byte*
GetTreeWindowClass()
{
	return kTreeWindowClass;
}

const JUtf8Byte*
GetFileListWindowClass()
{
	return kFileListWindowClass;
}

const JUtf8Byte*
GetEditorWindowClass()
{
	return kEditorWindowClass;
}

const JUtf8Byte*
GetExecOutputWindowClass()
{
	return kExecOutputWindowClass;
}

const JUtf8Byte*
GetCompileOutputWindowClass()
{
	return kCompileOutputWindowClass;
}

const JUtf8Byte*
GetSearchOutputWindowClass()
{
	return kSearchOutputWindowClass;
}

const JUtf8Byte*
GetShellWindowClass()
{
	return kShellWindowClass;
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
	return JGetString("Description::global", map, sizeof(map));
}

/******************************************************************************
 Icons

 ******************************************************************************/

#include "jcc_plain_file_small.xpm"
#include "jcc_plain_file_writable_small.xpm"
#include "jcc_source_file_small.xpm"
#include "jcc_source_file_writable_small.xpm"
#include "jcc_library_file_small.xpm"

#include "jcc_project_file.xpm"
#include "jcc_active_project_file.xpm"
#include "jcc_active_list_file.xpm"

void
CreateIcons()
{
	JXImageCache* c = theApplication->GetDisplay(1)->GetImageCache();

	theFileIcon           = c->GetImage(jcc_plain_file_small);
	theWritableFileIcon   = c->GetImage(jcc_plain_file_writable_small);
	theSourceIcon         = c->GetImage(jcc_source_file_small);
	theWritableSourceIcon = c->GetImage(jcc_source_file_writable_small);
	theLibraryIcon        = c->GetImage(jcc_library_file_small);

	theProjectIcon        = c->GetImage(jcc_project_file);
	theActiveProjectIcon  = c->GetImage(jcc_active_project_file);

	theActiveListIcon     = c->GetImage(jcc_active_list_file);
}

void
DeleteIcons()
{
}

const JXImage*
GetPlainFileIcon()
{
	return theFileIcon;
}

const JXImage*
GetWritablePlainFileIcon()
{
	return theWritableFileIcon;
}

const JXImage*
GetSourceFileIcon()
{
	return theSourceIcon;
}

const JXImage*
GetWritableSourceFileIcon()
{
	return theWritableSourceIcon;
}

const JXImage*
GetLibraryFileIcon()
{
	return theLibraryIcon;
}

const JXImage*
GetProjectFileIcon
	(
	const bool active
	)
{
	return (active ? theActiveProjectIcon : theProjectIcon);
}

const JXImage*
GetTextFileIcon
	(
	const bool active
	)
{
	return (active ? theActiveListIcon : theDocManager->GetDefaultMenuIcon());
}
