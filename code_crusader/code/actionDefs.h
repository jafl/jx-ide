/******************************************************************************
 actionDefs.h

	Shared actions for use as menu item ID's and in keybinding tables.
	These preprocessor definitions allow them to be included in static
	menu definitions.

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_actionDefs
#define _H_actionDefs

#include <jx-af/jx/jXActionDefs.h>	// for convenience

// File menu

#define kNewTextFileAction         "CBNewTextFile"
#define kNewTextFileFromTmplAction "CBNewTextFileFromTmpl"
#define kNewProjectAction          "CBNewProject"
#define kOpenSomethingAction       "CBOpenSomething"
#define kSaveFileAction            "CBSaveFile"
#define kSaveFileAsAction          "CBSaveFileAs"
#define kSaveCopyAsAction          "CBSaveCopyAs"
#define kSaveAsTemplateAction      "CBSaveAsTemplate"
#define kRevertAction              "CBRevert"
#define kSaveAllTextFilesAction    "CBSaveAllTextFiles"
#define kPageSetupStyledTextAction "CBPageSetupStyledText"
#define kPrintStyledTextAction     "CBPrintStyledText"
#define kCloseAllTextFilesAction   "CBCloseAllTextFiles"
#define kShowInFileMgrAction       "CBShowInFileMgr"

#define kDiffFilesAction "CBDiffFiles"
#define kDiffSmartAction "CBDiffSmart"
#define kDiffVCSAction   "CBDiffVCS"

#define kUNIXTextFormatAction "CBUNIXTextFormat"
#define kMacTextFormatAction  "CBMacTextFormat"
#define kDOSTextFormatAction  "CBDOSTextFormat"

// Project menu

#define kMakeBinaryAction       "CBMakeBinary"
#define kCompileFileAction      "CBCompileFile"
#define kEditMakeSettingsAction "CBEditMakeSettings"
#define kRunProgramAction       "CBRunProgram"
#define kDebugFileAction        "CBDebugFile"
#define kEditRunSettingsAction  "CBEditRunSettings"

#define kAddToProjAbsoluteAction     "CBAddToProjAbsolute"
#define kAddToProjProjRelativeAction "CBAddToProjProjRelative"
#define kAddToProjHomeRelativeAction "CBAddToProjHomeRelative"

#define kNewProjectGroupAction           "CBNewProjectGroup"
#define kAddFilesToProjectAction         "CBAddFilesToProject"
#define kAddDirectoryTreeToProjectAction "CBAddDirectoryTreeToProject"
#define kOpenSelectedFilesAction         "CBOpenSelectedFiles"
#define kOpenSelectedFileLocationsAction "CBOpenSelectedFileLocations"
#define kOpenComplFilesAction            "CBOpenComplFiles"
#define kRemoveFilesAction               "CBRemoveFiles"
#define kEditFilePathAction              "CBEditFilePath"
#define kEditSubprojConfigAction         "CBEditSubprojConfig"
#define kEditMakeConfigAction            "CBEditMakeConfig"
#define kUpdateMakefileAction            "CBUpdateMakefile"
#define kShowSymbolBrowserAction         "CBShowSymbolBrowser"
#define kShowCPPClassTreeAction          "CBShowCPPClassTree"
#define kShowDClassTreeAction            "CBShowDClassTree"
#define kShowGoClassTreeAction           "CBShowGoClassTree"
#define kShowJavaClassTreeAction         "CBShowJavaClassTree"
#define kShowPHPClassTreeAction          "CBShowPHPClassTree"
#define kViewManPageAction               "CBViewManPage"
#define kEditSearchPathsAction           "CBEditSearchPaths"
#define kShowFileListAction              "CBShowFileList"
#define kFindFileAction                  "CBFindFile"
#define kSearchFilesAction               "CBSearchFiles"

// Search menu

#define kBalanceGroupingSymbolAction  "CBBalanceGroupingSymbol"
#define kPlaceBookmarkAction          "CBPlaceBookmark"
#define kGoToLineAction               "CBGoToLine"
#define kGoToColumnAction             "CBGoToColumn"
#define kOpenSelectionAsFileAction    "CBOpenSelectionAsFile"
#define kFindSelectionInProjectAction "CBFindSelectionInProject"
#define kFindSelInProjNoContextAction "CBFindSelectionInProjectNoContext"
#define kFindSelectionInManPageAction "CBFindSelectionInManPage"
#define kFindFileAction               "CBFindFile"
#define kOpenManPageAction            "CBOpenManPage"
#define kOpenPrevListItem				"CBOpenPrevListItem"
#define kOpenNextListItem				"CBOpenNextListItem"

// Symbol menu

#define kCopySymbolNameAction "CBCopySymbolName"
#define kCloseAllSymSRAction  "CBCloseAllSymSR"

// Tree menus

#define kPrintTreeEPSAction "CBPrintTreeEPS"

#define kConfigureCTreeCPPAction     "CBConfigureCTreeCPP"
#define kUpdateClassTreeAction       "CBUpdateClassTree"
#define kMinimizeMILinkLengthAction  "CBMinimizeMILinkLength"
#define kOpenClassFnListAction       "CBOpenClassFnList"
#define kCollapseClassesAction       "CBCollapseClasses"
#define kExpandClassesAction         "CBExpandClasses"
#define kExpandAllClassesAction      "CBExpandAllClasses"
#define kSelectParentClassAction     "CBSelectParentClass"
#define kSelectDescendantClassAction "CBSelectDescendantClass"
#define kCopyClassNameAction         "CBCopyClassName"
#define kCreateDerivedClassAction    "CBCreateDerivedClass"
#define kFindFunctionAction          "CBFindFunction"

// File List menu

#define kUseWildcardFilterAction "CBUseWildcardFilter"
#define kUseRegexFilterAction    "CBUseRegexFilter"

#endif
