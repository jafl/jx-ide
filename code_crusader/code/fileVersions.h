/******************************************************************************
 fileVersions.h

	Version information for unstructured data files

	Copyright © 1996-2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_cbFileVersions
#define _H_cbFileVersions

#include <jx-af/jcore/jTypes.h>

const JFileVersion kCurrentPrefsFileVersion = 68;

// version 68:
//	Added support for D & Go source files.
// version 67:
//	Added support for Java properties files.
// version 66:
//	Added support for INI files.
// version 65:
//	Simplified sort of FileTypeInfo.
//	Added "core.*" file type to execute "medic -c $f".
// version 64:
//	Switches to TrueType monospace font.
// version 63:
//	Upgrade to ctags 5.8.
// version 62:
//	Added external png file type.
// version 61:
//	Update to SearchTextDialog prefs (73).
// version 60:
//	TextDocument saves kTabInsertsSpacesIndex setting override.
// version 59:
//	Added new file types and symbols supported by ctags 5.7.
// version 58:
//	Added XML to TextFileType.
// version 57:
//	Default character action for XML: xml-auto-close
// version 56:
//	Added JSP to TextFileType.
// version 55:
//	Removed DocumentManager::itsUpdateSymbolDBAfterBuildFlag.
// version 54:
//	Added ctags "PHP interface" type.
// version 53:
//	Re-sorted file type list so longer items are checked first.
// version 52:
// 	Added Ant to TextFileType.
// version 51:
//	Added ProjectDocument::warnOpenOldVersion to kStaticGlobalID.
// version 50:
// 	Added JavaScript to TextFileType.
// version 49:
//	Added C#, Erlang, SML to TextFileType.
//	Added ctags "ASP variable" type.
//	Added ctags "Vim autocommand group" type.
//	Added CSharpStyler, PythonStyler, EiffelStyler.
// version 48:
//	Added ctags "PHP define" type.
// version 47:
//	Added AddFilesFilterDialog::itsFilterStr to kStaticGlobalID.
// version 46:
//	Added BuildManager::ShouldRebuildMakefileDaily() to kStaticGlobalID.
// version 45:
//	Cleaned out data for id 14 and 15, since they were unused.
// version 44:
//	Added Beta, Lua, SLang, SQL, Vera, Verilog to TextFileType.
// version 43:
//	Removed RaiseBefore* and BeepAfter* from kStaticGlobalID.
// version 42:
//	Added DoubleSpaceCompilerOutput to kStaticGlobalID.
// version 41:
//	Added Bison to TextFileType.
// version 40:
//	Added scriptPath to FileTypeInfo.
// version 39:
//	Added C shell to TextFileType.
// version 38:
//	Added Lex to TextFileType.
// version 37:
//	Added RaiseBeforeMake/Compile/Run to kStaticGlobalID.
//	Added kRightMarginColorIndex to PrefsManager color list.
// version 36:
//	Added REXX and Ruby to TextFileType.
// version 35:
//	Added Make to TextFileType.
// version 34:
//	Added ASP to TextFileType.
//	Added various symbol types to CtagsUser::Type.
// version 33:
//	Added PHP to TextFileType.
// version 32:
//	Added Java Archive to TextFileType.
// version 31:
//	Added new file types supported by ctags 4.0.
// version 30:
//	Added Modula-2 and Modula-3 to TextFileType.
// version 29:
//	Pref ID zero is no longer valid.
// version 28:
//	HTMLStyler adds ?php and JavaScript styles, if not already defined.
// version 27:
//	Moved kViewHTMLCmdsID (18) to JXWebBrowser in JX shared prefs.
//	Converted kViewInHTMLBrowserSuffixID (19) to External file types.
//	Removed kURLPrefixID (20).
// version 26:
//	Added editCmd to FileTypeInfo.
// version 25:
//	JXHelpManager (kHelpID = 27) moved to JX shared prefs.
//	JTextEditor::CopyWhenSelect  moved to JX shared prefs. (kStaticGlobalID)
//	JXTEBase::UseWindowsHomeEnd  moved to JX shared prefs. (kStaticGlobalID)
//	JXTEBase::CaretFollowsScroll moved to JX shared prefs. (kStaticGlobalID)
// version 24:
//	Added file types .s .S .r .p
// version 23:
//	Replaced kSymbolDialogID (47) with kSymbolWindSizeID (47).
// version 22:
//	Incremented CStyler version.
// version 21:
//	Adds JXTEBase::windowsHomeEnd and TextEditor::scrollCaret to kStaticGlobalID.
// version 20:
//	Adds CompileDialog::beepAfterMake,beepAfterCompile to kStaticGlobalID.
// version 19:
//	Renamed kGlobalTextID to kStaticGlobalID.
//	Adds ProjectDocument::reopenFiles, ProjectTable::dropAction to kStaticGlobalID.
// version 18:
//	Adds file suffixes for Eiffel, FORTRAN, and Java.
// version 17:
//	Stores JXFileDocument::itsAskOKToCloseFlag in kGlobalTextID.
// version 16:
//	JXHelpManager setup version incremented.
// version 15:
//	Converted variable marker in HTML commands from % to $
// version 14:
//	Resorted FileTypeInfo to put content regexes first.
// version 13:
//	Added C, C++, UNIX script, E-mail, and Outline CRM rule lists.
//	Added CRM id and complement suffix to FileTypeInfo.
//	Added content regexes to file type list.
// version 12:
//	Moved 20000 -> 0, 20004 -> 1
//	Converted from suffix lists.
//		Check PrefsManager::ConvertFromSuffixLists() for details.
// version 11:
//	Incremented to keep new SearchTextDialog prefs from being
//		destroyed by older version of Code Crusader.
// version 10:
//	Incremented version to avoid incompatibility with Code Medic.
// version 9:
//	Added misc file type info
//	Moved 10003 -> 10004, 10002 -> 10003, 20003 -> 20004
// version 8:
//	Removed id=1 (PS print setup for tree)
//	Removed id=8 (EPS print setup for tree)
// version 7:
//	Moved id=4 (print plain text) to id=16 to make space for
//		recreation of external editor cmds
// version 6:
//	Collected suffixes (2,3,16,17) to (10000,10001,10002,10003)
// version 5:
//	Added default font (kDefFontID)
// version 4:
//	Upgrades EPS setup to XEPS setup
//	Added print plain text settings for TextEditor
// version 3:
//	Removed id=4 (cmds for using external editor)
// version 2:
//	Removed '&' from external editor commands
// version 1:
//	Stores "local text editing" flag in kViewFileCmdID

const JFileVersion kCurrentProjectFileVersion = 94;

// version 94:
//	Fixes bug in symbol list sorting which broke lookup.
// version 93:
//	Stores partial paths for file scoped symbols.
// version 92:
//	Parse \\ in output from ctags (PHP).
// version 91:
//	Update parser for C++ tree to use properties instead of implementation
// version 90:
//	Added HTML anchor type, lex regex type.
// version 89:
//	Added Java method prototype (from interface).
// version 88:
//	Added support for D & Go.
//	Removed FnListDirector.
//	Removed functions from Class.
// version 87:
//	Removed show flags from Tree, because all loners moved to the bottom.
// version 86:
//	JavaTreeScanner:
//		* If parent not found in import list, assume package.
//		* Detect inner classes.
// version 85:
//	Added PHPTreeDirector.
// version 84:
//	TextDocument: removed kCharSetIndex setting.
// version 83:
//	Upgrade to ctags 5.8.
// version 82:
//	TextDocument saves kTabInsertsSpacesIndex setting override.
// version 81:
//	Added new file types and symbols supported by ctags 5.7.
// version 80:
//	FileListTable stores itsLastUniqueID.
// version 79:
//	Added ctags "PHP interface" type.
// version 78:
//	Fixed bug in ctags PHP parser.
// version 77:
// 	Parses JavaScript functions inside PHP files.
// version 76:
// 	Added Ant file type.
// version 75:
//	Fixed Java superclass parsing.
// version 74:
//	Moved SymbolTypeList to prefs file.
// version 73:
// 	Added JavaScript file type.
// version 72:
//	Converted item counts into "keepGoing" booleans, to allow CVS merge.
// version 71:
//	ProjectDocument saves non-essential information in .jup and .jst, for CVS users.
// version 70:
//	TextDocument saves kCharSetIndex setting override.
// version 69:
//	TextDocument saves kTabWidthIndex setting override.
// version 68:
//	TextDocument saves settings overrides.
// version 67:
//	Added support for CMake.
// version 66:
//	Added C#, Erlang, SML file types.
//	Added ctags "ASP variable" type.
//	Added ctags "Vim autocommand group" type.
// version 65:
//	Added ctags "PHP define" type.
// version 64:
//	Added BuildManager::itsSubProjectBuildCmd.
// version 63:
//	Added SymbolInfo::signature.
//	Added Beta, Lua, SLang, SQL, Vera, Verilog file types.
// version 62:
//	BuildManager replaces CompileDialog and RunDialog.
//	Added CommandManager.
// version 61:
//	Added Bison file type.
// version 60:
//	Added C shell file type.
// version 59:
//	Added Lex file type.
// version 58:
//	C++ inheritance parser correctly handles declarations within namespaces.
// version 57:
//	Converted ProjectDocument to use MakefileMethod.
//	CompileDialog stores qmake file mod times.
// version 56:
//	Added REXX and Ruby file types.
// version 55:
//	All symbols from Makefiles have file scope.
// version 54:
//	Added flag for marking fully qualified, file scoped symbols.
// version 53:
//	Added new file types and symbols supported by ctags 4.0.
// version 52:
//	Added Java and Qt inheritance and access types to Class.
//	Added StreamOut() to JavaClass.
// version 51:
//	Changed sorting function for SymbolList.
// version 50:
//	FileListDirector stores itsActiveFlag.
// version 49:
//	Class stores itsIsTemplateFlag.
// version 48:
//	Added JavaTreeDirector.
// version 47:
//	SymbolDirector stores itsActiveFlag.
// version 46:
//	Reparse all to remove "::name" and add "file:name".
// version 45:
//	Added SymbolTypeList.
// version 44:
//	Includes C function prototypes and class members in symbol database.
// version 43:
//	Tree::itsClassesByFull is sorted case sensitive.
// version 42:
//	Moved file list, symbol list, and C++ class tree to separate .jst file.
// version 41:
//	Added SymbolDirector.
// version 40:
//	Tree no longer stores file names and mod times.
//	Class stores file ID instead of file name.
//	Added FileListTable.
// version 39:
//	RunDialog stores useOutputDoc flag.
//	LibraryNode stores itsIncludeInDepListFlag.
//	ProjectDocument stores itsDepListExpr.
// version 38:
//	LibraryNode stores itsShouldBuildFlag.
// version 37:
//	ProjectDocument stores previous file path.
// version 36:
//	ProjectDocument stores itsPrintName;
// version 35:
//	ProjectDocument stores its*WriteMakeFilesFlag and itsTargetName.
// version 34:
//	Stores ProjectTable setup.
//	GroupNode stores open flag.
// version 33:
//	ProjectDocument stores window geometry.
// version 32:
//	ProjectDocument stores file tree.
// version 31:
//	TreeDirector stores IsActive().
// version 30:
//	Removed type value (=1) from data written by Class.
// version 29:
//	CompileDialog stores "double space output" flag.
//	TextDocument saves PT & PS printing file names.
//	JXHelpManager setup version incremented.
// version 28:
//	ProjectDocument stores CPreprocessor.
// version 27:
//	CompileDialog stores makemake file info.
// version 26:
//	Converted variable marker in Compile and Run dialogs from % to $
// version 25:
//	C++ parser strips non-ID characters (e.g. *) from function names,
//		so everything needs to be reparsed.
// version 24:
//	ProjectDocument stores its window geometry.
//	FnListDirector stores scrollbar setup.
// version 23:
//	Compile/RunDialog stores window geometry in setup.
// version 22:
//	TextDocument and TreeWidget use Read/WriteScrollSetup().
// version 21:
//	Tree saves recurse flag for each directory.
// version 20:
//	ProjectDocument saves FileListDirector setup.
// version 19:
//	Tree saves itsNeedsMinimizeMILinksFlag.
// version 18:
//	Tree saves itsVisibleByGeom (as indicies).
//	TextDocument and TreeWidget save scrolltabs.
// version 17:
//	ProjectDocument saves setup of each FnListDirector.
//	ProjectDocument saves printer setup in project file.
// version 16:
//	Changes debugging command to "medic -f +%l %f"
// version 15:
//	C++ parser correctly handles inline constructors with initializer
//		lists, so everything needs to be reparsed.
// version 14:
//	CompileDialog stores make depend settings.
// version 13:
//	Tree stores itsShowLoneStructsFlag.
// version 12:
//	Tree stores itsShowLoneClassesFlag.
// version 11:
//	Class stores itsCollapsedFlag in place of itsWasVisibleFlag.
//	Class stores itsHas*ChildrenFlag.
// version 10:
//	TreeWidget stores scroll position.
//	Tree stores itsShowEnumsFlag.
//	Class stores itsVisibleFlag, itsWasVisibleFlag, itsIsSelectedFlag.
// version 9:
//	XTree stores copy of header suffix list.
// version 8:
//	Stores path history menus from CompileDialog and RunDialog
// version 7:
//	Eliminated Tree::itsClassNamesSortedFlag
// version 6:
//	Stores Class::itsNameStem, Class::itsFrame
//	All files in Tree must be reparsed
// version 5:
//	Stores settings from RunDialog
// version 4:
//	All files in Tree must be reparsed
// version 3:
//	Stores settings from CompileDialog
// version 2:
//	Stores configuration of open text editors
// version 1:
//	Stores Class::itsDeclType.
//	No longer stores Class::itsFirstFoundParent

const JFileVersion kCurrentProjectTmplVersion = 5;

// version 5:
//	Adjustments for non-essential data moved to .jup and .jst (projVers=71).
// version 4:
//	Added support for CMake.
// version 3:
//	Moved itsMakefileMethod,itsTargetName,itsDepListExpr to BuildManager::WriteTemplate()
//	Removed RunDialog.
// version 2:
//	Converted to ProjectDocument::MakefileMethod.
//	CompileDialog saves qmake files.
//	CompileDialog saves Makefile if nothing else exists.
// version 1:
//	Doesn't save Make.files, Make.header if they don't exist.

const JFileVersion kCurrentProjectWizardVersion = 0;

#endif
