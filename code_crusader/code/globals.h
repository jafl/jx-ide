/******************************************************************************
 globals.h

	Copyright © 1997-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_globals
#define _H_globals

// we include these for convenience

#include <jx-af/jx/jXGlobals.h>
#include "App.h"
#include "DocumentManager.h"
#include "PrefsManager.h"

class MDIServer;
class SearchTextDialog;
class RunTEScriptDialog;
class ViewManPageDialog;
class FindFileDialog;
class DiffFileDialog;
class CommandManager;
class FnMenuUpdater;
class SymbolTypeList;
class PTPrinter;
class PSPrinter;

App*				GetApplication();
DocumentManager*	GetDocumentManager();
bool				HasPrefsManager();
PrefsManager*		GetPrefsManager();
void				ForgetPrefsManager();
bool				GetMDIServer(MDIServer** mdi);

SearchTextDialog*	GetSearchTextDialog();
RunTEScriptDialog*	GetRunTEScriptDialog();
ViewManPageDialog*	GetViewManPageDialog();
FindFileDialog*		GetFindFileDialog();
DiffFileDialog*		GetDiffFileDialog();

CommandManager*		GetCommandManager();
FnMenuUpdater*		GetFnMenuUpdater();
SymbolTypeList*		GetSymbolTypeList();

PTPrinter*			GetPTTextPrinter();
PSPrinter*			GetPSTextPrinter();

const JString&		GetVersionNumberStr();
JString				GetVersionStr();

	// icons

const JXImage*	GetPlainFileIcon();
const JXImage*	GetWritablePlainFileIcon();
const JXImage*	GetSourceFileIcon();
const JXImage*	GetWritableSourceFileIcon();
const JXImage*	GetLibraryFileIcon();
const JXImage*	GetProjectFileIcon(const bool active);
const JXImage*	GetTextFileIcon(const bool active);

	// called by App

bool	CreateGlobals(App* app, const bool useMDI);
void	DeleteGlobals();
void	CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JUtf8Byte*	GetWMClassInstance();
const JUtf8Byte*	GetProjectWindowClass();

#endif
