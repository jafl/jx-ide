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
static PrefsManager*	thePrefsManager      = nullptr;
static MDIServer*		theMDIServer         = nullptr;	// owned by JX

static SearchTextDialog*	theSearchTextDialog  = nullptr;	// owned by JXGetPersistentWindowOwner()
static RunTEScriptDialog*	theRunTEScriptDialog = nullptr;	// owned by JXGetPersistentWindowOwner()
static ViewManPageDialog*	theViewManPageDialog = nullptr;	// owned by JXGetPersistentWindowOwner()
static FindFileDialog*		theFindFileDialog    = nullptr;	// owned by JXGetPersistentWindowOwner()
static DiffFileDialog*		theDiffFileDialog    = nullptr;	// owned by JXGetPersistentWindowOwner()

static CommandManager*	theCmdManager        = nullptr;
static FnMenuUpdater*	theFnMenuUpdater     = nullptr;
static SymbolTypeList*	theSymbolTypeList    = nullptr;

static PTPrinter*	thePTTextPrinter     = nullptr;
static PSPrinter*	thePSTextPrinter     = nullptr;

// owned by JXImageCache
static JXImage* theFileIcon           = nullptr;
static JXImage* theWritableFileIcon   = nullptr;
static JXImage* theSourceIcon         = nullptr;
static JXImage* theWritableSourceIcon = nullptr;
static JXImage* theLibraryIcon        = nullptr;
static JXImage* theProjectIcon        = nullptr;
static JXImage* theActiveProjectIcon  = nullptr;
static JXImage* theActiveListIcon     = nullptr;

static std::atomic_int theSymbolUpdateRefCount    = 0;
static std::atomic_int theCompleterUpdateRefCount = 0;

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
	App*		app,
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

	JXInitHelp();

	theDocManager = jnew DocumentManager;

	if (useMDI)
	{
		auto* dockManager = jnew DockManager();
		dockManager->JPrefObject::ReadPrefs();

		theMDIServer = jnew MDIServer;
	}

	theSearchTextDialog  = SearchTextDialog::Create();
	theRunTEScriptDialog = jnew RunTEScriptDialog(JXGetPersistentWindowOwner());
	theViewManPageDialog = jnew ViewManPageDialog(JXGetPersistentWindowOwner());
	theFindFileDialog    = jnew FindFileDialog(JXGetPersistentWindowOwner());
	theDiffFileDialog    = jnew DiffFileDialog(JXGetPersistentWindowOwner());

	// widgets hidden in permanent window

	JXWindow* permWindow = theSearchTextDialog->GetWindow();

	theDocManager->CreateFileHistoryMenus(permWindow);

	// global commands

	theCmdManager = jnew CommandManager(theDocManager);
	theCmdManager->JPrefObject::ReadPrefs();

	// fn menu updater

	theFnMenuUpdater = jnew FnMenuUpdater;

	// symbol type list

	theSymbolTypeList = jnew SymbolTypeList(permWindow->GetDisplay());

	// printers

	thePTTextPrinter = jnew PTPrinter;
	thePSTextPrinter = jnew PSPrinter(permWindow->GetDisplay());

	return isNew;
}

/******************************************************************************
 DeleteGlobals

 ******************************************************************************/

void
DeleteGlobals()
{
	if (thePrefsManager != nullptr)
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
	}

	DeleteIcons();

	theApplication = nullptr;
	theDocManager  = nullptr;
	theMDIServer   = nullptr;

	if (thePrefsManager != nullptr)
	{
		theCmdManager->JPrefObject::WritePrefs();
		theFnMenuUpdater->JPrefObject::WritePrefs();
		theSymbolTypeList->JPrefObject::WritePrefs();
		thePTTextPrinter->JPrefObject::WritePrefs();
		thePSTextPrinter->JPrefObject::WritePrefs();
	}
	jdelete theCmdManager;
	theCmdManager = nullptr;

	jdelete theFnMenuUpdater;
	theFnMenuUpdater = nullptr;

	jdelete theSymbolTypeList;
	theSymbolTypeList = nullptr;

	jdelete thePTTextPrinter;
	thePTTextPrinter = nullptr;

	jdelete thePSTextPrinter;
	thePSTextPrinter = nullptr;

	if (thePrefsManager != nullptr)
	{
		ShutdownCompleters();
		ShutdownStylers();
	}

	// windows closed by JXGetPersistentWindowOwner()

	if (thePrefsManager != nullptr)
	{
		theSearchTextDialog->JPrefObject::WritePrefs();
		theRunTEScriptDialog->JPrefObject::WritePrefs();
		theViewManPageDialog->JPrefObject::WritePrefs();
		theFindFileDialog->JPrefObject::WritePrefs();
		theDiffFileDialog->JPrefObject::WritePrefs();
	}

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
 GetDocumentManager

 ******************************************************************************/

DocumentManager*
GetDocumentManager()
{
	assert( theDocManager != nullptr );
	return theDocManager;
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

/******************************************************************************
 Thread management

 ******************************************************************************/

void
SymbolUpdateStarted()
{
	theSymbolUpdateRefCount++;
}

bool
SymbolUpdateRunning()
{
	return theSymbolUpdateRefCount > 0;
}

void
SymbolUpdateFinished()
{
	assert( theSymbolUpdateRefCount > 0 );
	theSymbolUpdateRefCount--;
}

void
CompleterUpdateStarted()
{
	theCompleterUpdateRefCount++;
}

bool
CompleterUpdateRunning()
{
	return theCompleterUpdateRefCount > 0;
}

void
CompleterUpdateFinished()
{
	assert( theCompleterUpdateRefCount > 0 );
	theCompleterUpdateRefCount--;
}
