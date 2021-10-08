/******************************************************************************
 TextDocument.cpp

	BASE CLASS = JXFileDocument, JPrefObject

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "TextDocument.h"
#include "TextEditor.h"
#include "EditTextPrefsDialog.h"
#include "TELineIndexInput.h"
#include "TEColIndexInput.h"
#include "FileDragSource.h"
#include "FileNameDisplay.h"
#include "FileHistoryMenu.h"
#include "TabWidthDialog.h"
#include "ProjectDocument.h"
#include "DiffFileDialog.h"
#include "StylerBase.h"
#include "PTPrinter.h"
#include "PSPrinter.h"
#include "DocumentMenu.h"
#include "CommandMenu.h"
#include "globals.h"
#include "sharedUtil.h"
#include "actionDefs.h"
#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXDockWidget.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXDownRect.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXSharedPrefsManager.h>
#include <jx-af/jx/jXEventUtil.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 2;
const JUtf8Byte kSetupDataEndDelimiter  = '\1';

	// version 1 includes itsBreakCodeCROnlyFlag
	// version 2 removes itsBreakCodeCROnlyFlag

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    New text file                  %k Meta-N       %i" kNewTextFileAction
	"  | New text file from template... %k Meta-Shift-N %i" kNewTextFileFromTmplAction
	"  | New project...                                 %i" kNewProjectAction
	"  | New shell...                                   %i" kNewShellAction
	"%l| Open...                        %k Meta-O       %i" kOpenSomethingAction
	"  | Recent projects"
	"  | Recent text files"
	"%l| Save                           %k Meta-S       %i" kSaveFileAction
	"  | Save as...                     %k Ctrl-S       %i" kSaveFileAsAction
	"  | Save a copy as...              %k Ctrl-Shift-S %i" kSaveCopyAsAction
	"  | Save as template...                            %i" kSaveAsTemplateAction
	"  | Revert to saved                                %i" kRevertAction
	"  | Save all                       %k Meta-Shift-S %i" kSaveAllTextFilesAction
	"%l| File format"
	"%l| Compare files...                               %i" kDiffFilesAction
	"  | Compare with...                                %i" kDiffSmartAction
	"  | Compare with version control                   %i" kDiffVCSAction
	"  | Compare"
	"%l| Show in file manager                           %i" kShowInFileMgrAction
	"%l| Page setup...                                  %i" kJXPageSetupAction
	"  | Print...                       %k Meta-P       %i" kJXPrintAction
	"%l| Page setup for styles...                       %i" kPageSetupStyledTextAction
	"  | Print with styles...           %k Meta-Shift-P %i" kPrintStyledTextAction
	"%l| Close                          %k Meta-W       %i" kJXCloseWindowAction
	"  | Close all                      %k Meta-Shift-W %i" kCloseAllTextFilesAction
	"%l| Quit                           %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewTextEditorCmd = 1, kNewTextTemplateCmd, kNewProjectCmd, kNewShellCmd,
	kOpenSomethingCmd, kRecentProjectMenuCmd, kRecentTextMenuCmd,
	kSaveFileCmd, kSaveFileAsCmd, kSaveCopyAsCmd, kSaveAsTemplateCmd,
	kRevertCmd, kSaveAllFilesCmd,
	kFileFormatIndex,
	kDiffFilesCmd, kDiffSmartCmd, kDiffVCSCmd, kDiffMenuIndex,
	kShowInFileMgrCmd,
	kPTPageSetupCmd, kPrintPTCmd,
	kPSPageSetupCmd, kPrintPSCmd,
	kCloseCmd, kCloseAllCmd,
	kQuitCmd
};

// File format menu

static const JUtf8Byte* kFileFormatMenuStr =
	"  UNIX                 %r %i" kUNIXTextFormatAction
	"| Macintosh (pre OS X) %r %i" kMacTextFormatAction
	"| DOS / Windows        %r %i" kDOSTextFormatAction;

enum
{
	kUNIXFmtCmd = 1, kMacFmtCmd, kDOSFmtCmd
};

// Diff menu

static const JUtf8Byte* kDiffMenuStr =
	"    Compare as file #1..."
	"  | Compare as file #2..."
	"%l| Compare with version control...";

enum
{
	kDiffAs1Cmd = 1,
	kDiffAs2Cmd,
	kDiffAsVCSCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Editor..."
	"  | Toolbar buttons..."
	"  | External editors..."
	"  | File manager & web browser..."
	"  | Miscellaneous..."
	"%l| File types..."
	"  | Syntax highlighting..."
	"  | Macros..."
	"  | Clean paragraph margins rules..."
	"%l| Save window size as default";

enum
{
	kEditorPrefsCmd = 1, kToolBarPrefsCmd,
	kChooseExtEditorsCmd, kWWWPrefsCmd, kMiscPrefsCmd,
	kEditFileTypesCmd, kEditStylesSubmenuIndex,
	kEditMacrosCmd, kEditCRMRuleListsCmd,
	kSaveWindSizeCmd
};

// Syntax highlighting menu

static const JUtf8Byte* kPrefsStylesMenuStr =
	"  Bourne shell"
	"| C shell"
	"| C / C++"
	"| C#"
	"| D"
	"| Eiffel"
	"| Go"
	"| HTML / PHP / JSP / XML"
	"| INI"
	"| Java"
	"| Java properties"
	"| JavaScript"
	"| Perl"
	"| Python"
	"| Ruby"
	"| SQL"
	"| TCL";

static const Language kMenuItemToLang[] =
{
	kBourneShellLang,
	kCShellLang,
	kCLang,
	kCSharpLang,
	kDLang,
	kEiffelLang,
	kGoLang,
	kHTMLLang,
	kINILang,
	kJavaLang,
	kPropertiesLang,
	kJavaScriptLang,
	kPerlLang,
	kPythonLang,
	kRubyLang,
	kSQLLang,
	kTCLLang
};

// Settings popup menu

static const JUtf8Byte* kSettingsMenuStr =
	"    Change spaces per tab..." 
	"%l| Auto-indent        %b"
	"  | Tab inserts spaces %b"
	"  | Show whitespace    %b"
	"  | Word wrap          %b"
	"  | Read only          %b";

enum
{
	kTabWidthCmd = 1,
	kToggleAutoIndentCmd,
	kToggleTabInsertsSpacesCmd,
	kToggleWhitespaceCmd,
	kToggleWordWrapCmd,
	kToggleReadOnlyCmd
};

// JBroadcaster message types

const JUtf8Byte* TextDocument::kSaved = "Saved::TextDocument";

/******************************************************************************
 Constructor

 ******************************************************************************/

TextDocument::TextDocument
	(
	const TextFileType	type,		// first to avoid conflict with fileName
	const JUtf8Byte*	helpSectionName,
	const bool			setWMClass,
	TextEditorCtorFn	teCtorFn
	)
	:
	JXFileDocument(GetApplication(),
				   GetDocumentManager()->GetNewFileName(),
				   false, true, ""),
	JPrefObject(GetPrefsManager(), kTextDocID),
	itsHelpSectionName(helpSectionName)
{
	TextDocumentX1(type);
	BuildWindow(setWMClass, teCtorFn);
	TextDocumentX2(true);
}

TextDocument::TextDocument
	(
	const JString&			fileName,
	const TextFileType	type,
	const bool			tmpl
	)
	:
	JXFileDocument(GetApplication(), fileName, !tmpl, true, ""),
	JPrefObject(GetPrefsManager(), kTextDocID),
	itsHelpSectionName("OverviewHelp")
{
	TextDocumentX1(type);
	BuildWindow(true, ConstructTextEditor);

	ReadFile(fileName, true);
	TextDocumentX2(true);

	if (tmpl)
	{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);
		FileChanged(name, false);
	}
}

TextDocument::TextDocument
	(
	std::istream&		input,
	const JFileVersion	vers,
	bool*			keep
	)
	:
	JXFileDocument(GetApplication(), JString::empty, false, true, ""),
	JPrefObject(GetPrefsManager(), kTextDocID),
	itsHelpSectionName("OverviewHelp")
{
	TextDocumentX1(kUnknownFT);
	BuildWindow(true, ConstructTextEditor);
	*keep = ReadFromProject(input, vers);
	TextDocumentX2(false);

	JXDockWidget* dock;
	if (*keep && GetWindow()->GetDockWidget(&dock))
	{
		dock->Dock(GetWindow());	// sort to correct location
	}
}

// private

void
TextDocument::TextDocumentX1
	(
	const TextFileType type
	)
{
	ListenTo(this);

	itsOpenOverComplementFlag = false;
	itsFileFormat             = JStyledText::kUNIXText;
	itsUpdateFileTypeFlag     = false;

	itsFileType    = type;
	itsActionMgr   = nullptr;
	itsMacroMgr    = nullptr;
	itsCRMRuleList = nullptr;

	itsTabWidthDialog = nullptr;

	for (JUnsignedOffset i=0; i<kSettingCount; i++)
	{
		itsOverrideFlag[i] = false;
	}

	GetDocumentManager()->TextDocumentCreated(this);
}

void
TextDocument::TextDocumentX2
	(
	const bool setWindowSize
	)
{
	itsTextEditor->SetPTPrinter(GetPTTextPrinter());
	itsTextEditor->SetPSPrinter(GetPSTextPrinter());

	// must do this after reading file

	itsUpdateFileTypeFlag = true;
	UpdateFileType(true);

	// must do this after reading prefs and calculating file type

	if (setWindowSize)
	{
		JPoint desktopLoc;
		JCoordinate w,h;
		if (GetWindowSize(&desktopLoc, &w, &h))
		{
			JXWindow* window = GetWindow();
			window->Place(desktopLoc.x, desktopLoc.y);
			window->SetSize(w,h);
		}
	}
}

// static private

TextEditor*
TextDocument::ConstructTextEditor
	(
	TextDocument*		document,
	const JString&		fileName,
	JXMenuBar*			menuBar,
	TELineIndexInput*	lineInput,
	TEColIndexInput*	colInput,
	JXScrollbarSet*		scrollbarSet
	)
{
	auto* te =
		jnew TextEditor(document, fileName, menuBar, lineInput, colInput, false,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( te != nullptr );

	return te;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextDocument::~TextDocument()
{
	GetDocumentManager()->TextDocumentDeleted(this);
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
TextDocument::Activate()
{
	JXFileDocument::Activate();

	if (IsActive())
	{
		GetDocumentManager()->SetActiveTextDocument(this);
	}
}

/******************************************************************************
 ConvertSelectionToFullPath (virtual)

	Check if the file is relative to our file's path.
	ExecOutputDocument overrides this to try other tricks.

 ******************************************************************************/

void
TextDocument::ConvertSelectionToFullPath
	(
	JString* fileName
	)
	const
{
	if (JIsAbsolutePath(*fileName))
	{
		return;
	}

	if (ExistsOnDisk())
	{
		const JString testName = JCombinePathAndName(GetFilePath(), *fileName);
		if (JFileExists(testName))
		{
			*fileName = testName;
		}
	}
}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

bool
TextDocument::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	ExecOutputDocument* doc;
	*icon = GetTextFileIcon(GetDocumentManager()->GetActiveListDocument(&doc) &&
								   doc == const_cast<TextDocument*>(this));
	return true;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include <jx_file_new.xpm>
#include <jx_file_open.xpm>
#include <jx_file_save.xpm>
#include <jx_file_save_all.xpm>
#include <jx_file_revert_to_saved.xpm>
#include <jx_file_print.xpm>
#include <jx_edit_read_only.xpm>
#include <jx_show_whitespace.xpm>

#include "jcc_file_print_with_styles.xpm"
#include "jcc_file_save_copy_as.xpm"
#include "jcc_unix_format.xpm"
#include "jcc_mac_format.xpm"
#include "jcc_dos_format.xpm"
#include "jcc_compare_files.xpm"
#include "jcc_compare_files_1.xpm"
#include "jcc_compare_files_2.xpm"
#include "jcc_compare_backup.xpm"
#include "jcc_compare_vcs.xpm"
#include "jcc_compare_vcs_as.xpm"
#include "jcc_word_wrap.xpm"
#include "jcc_auto_indent.xpm"
#include "jcc_tab_inserts_spaces.xpm"

void
TextDocument::BuildWindow
	(
	const bool			setWMClass,
	TextEditorCtorFn	teCtorFn
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 550,550, JString::empty);
	assert( window != nullptr );

	itsFileDragSource =
		jnew FileDragSource(this, window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 150,530, 20,20);
	assert( itsFileDragSource != nullptr );

	itsMenuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 480,30);
	assert( itsMenuBar != nullptr );

	itsActionButton =
		jnew JXTextButton(JGetString("itsActionButton::TextDocument::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,0, 70,30);
	assert( itsActionButton != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kTEToolBarID, itsMenuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 550,500);
	assert( itsToolBar != nullptr );

	auto* lineBorder =
		jnew JXDownRect(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,530, 80,20);
	assert( lineBorder != nullptr );

	auto* lineLabel =
		jnew JXStaticText(JGetString("lineLabel::TextDocument::JXLayout"), lineBorder,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 28,16);
	assert( lineLabel != nullptr );
	lineLabel->SetToLabel();

	auto* lineInput =
		jnew TELineIndexInput(lineLabel, lineBorder,
					JXWidget::kHElastic, JXWidget::kFixedTop, 28,0, 48,16);
	assert( lineInput != nullptr );

	auto* colBorder =
		jnew JXDownRect(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,530, 70,20);
	assert( colBorder != nullptr );

	auto* colLabel =
		jnew JXStaticText(JGetString("colLabel::TextDocument::JXLayout"), colBorder,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 28,16);
	assert( colLabel != nullptr );
	colLabel->SetToLabel();

	auto* colInput =
		jnew TEColIndexInput(lineInput, colLabel, colBorder,
					JXWidget::kHElastic, JXWidget::kFixedTop, 28,0, 38,16);
	assert( colInput != nullptr );

	itsFileDisplay =
		jnew FileNameDisplay(this, itsFileDragSource, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 170,530, 295,20);
	assert( itsFileDisplay != nullptr );

	itsSettingsMenu =
		jnew JXTextMenu(JGetString("itsSettingsMenu::TextDocument::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 465,530, 85,20);
	assert( itsSettingsMenu != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetMinSize(300, 100);
	if (setWMClass)
	{
		window->SetWMClass(GetWMClassInstance(), GetEditorWindowClass());
	}
	window->ShouldFocusWhenShow(true);	// necessary for click-to-focus

	JPoint p = itsSettingsMenu->GetTitlePadding();
	p.y      = 0;
	itsSettingsMenu->SetTitlePadding(p);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	DisplayFileName(fullName);

	itsTextEditor = teCtorFn(this, fullName, itsMenuBar, lineInput, colInput, scrollbarSet);
	itsTextEditor->FitToEnclosure();

	UpdateReadOnlyDisplay();

	ListenTo(itsTextEditor);
	ListenTo(itsActionButton);
	itsActionButton->SetHint(JGetString("ConfigButtonHint::TextDocument"));

	itsFileDisplay->SetTE(itsTextEditor);
	itsFileDisplay->ShareEditMenu(itsTextEditor);

	itsSettingsMenu->SetPopupArrowDirection(JXMenu::kArrowPointsUp);
	itsSettingsMenu->AdjustSize(
		window->GetFrameGlobal().right - itsSettingsMenu->GetFrameGlobal().right, 0);
	itsSettingsMenu->SetMenuItems(kSettingsMenuStr, "TextDocument");
	itsSettingsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsSettingsMenu);

	itsSettingsMenu->SetItemImage(kToggleAutoIndentCmd,       jcc_auto_indent);
	itsSettingsMenu->SetItemImage(kToggleTabInsertsSpacesCmd, jcc_tab_inserts_spaces);
	itsSettingsMenu->SetItemImage(kToggleWhitespaceCmd,       jx_show_whitespace);
	itsSettingsMenu->SetItemImage(kToggleWordWrapCmd,         jcc_word_wrap);
	itsSettingsMenu->SetItemImage(kToggleReadOnlyCmd,         jx_edit_read_only);

	itsFileMenu = itsMenuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "TextDocument");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);
	itsFileMenu->SetItemImage(kSaveFileCmd,      jx_file_save);
	itsFileMenu->SetItemImage(kSaveCopyAsCmd,    jcc_file_save_copy_as);
	itsFileMenu->SetItemImage(kRevertCmd,        jx_file_revert_to_saved);
	itsFileMenu->SetItemImage(kSaveAllFilesCmd,  jx_file_save_all);
	itsFileMenu->SetItemImage(kDiffFilesCmd,     jcc_compare_files);
	itsFileMenu->SetItemImage(kDiffSmartCmd,     jcc_compare_backup);
	itsFileMenu->SetItemImage(kDiffVCSCmd,       jcc_compare_vcs);
	itsFileMenu->SetItemImage(kPrintPTCmd,       jx_file_print);
	itsFileMenu->SetItemImage(kPrintPSCmd,       jcc_file_print_with_styles);

	auto* recentProjectMenu =
		jnew FileHistoryMenu(DocumentManager::kProjectFileHistory,
							  itsFileMenu, kRecentProjectMenuCmd, itsMenuBar);
	assert( recentProjectMenu != nullptr );

	auto* recentTextMenu =
		jnew FileHistoryMenu(DocumentManager::kTextFileHistory,
							  itsFileMenu, kRecentTextMenuCmd, itsMenuBar);
	assert( recentTextMenu != nullptr );

	itsFileFormatMenu = jnew JXTextMenu(itsFileMenu, kFileFormatIndex, itsMenuBar);
	assert( itsFileFormatMenu != nullptr );
	itsFileFormatMenu->SetMenuItems(kFileFormatMenuStr, "TextDocument");
	itsFileFormatMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileFormatMenu);

	itsFileFormatMenu->SetItemImage(kUNIXFmtCmd, jcc_unix_format);
	itsFileFormatMenu->SetItemImage(kMacFmtCmd,  jcc_mac_format);
	itsFileFormatMenu->SetItemImage(kDOSFmtCmd,  jcc_dos_format);

	itsDiffMenu = jnew JXTextMenu(itsFileMenu, kDiffMenuIndex, itsMenuBar);
	assert( itsDiffMenu != nullptr );
	itsDiffMenu->SetMenuItems(kDiffMenuStr, "TextDocument");
	ListenTo(itsDiffMenu);

	itsDiffMenu->SetItemImage(kDiffAs1Cmd,   jcc_compare_files_1);
	itsDiffMenu->SetItemImage(kDiffAs2Cmd,   jcc_compare_files_2);
	itsDiffMenu->SetItemImage(kDiffAsVCSCmd, jcc_compare_vcs_as);

	itsCmdMenu =
		jnew CommandMenu(nullptr, this, itsMenuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != nullptr );
	itsMenuBar->AppendMenu(itsCmdMenu);

	itsWindowMenu =
		jnew DocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), itsMenuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsWindowMenu != nullptr );
	itsMenuBar->AppendMenu(itsWindowMenu);

	itsPrefsMenu = itsMenuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "TextDocument");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsPrefsStylesMenu = jnew JXTextMenu(itsPrefsMenu, kEditStylesSubmenuIndex, itsMenuBar);
	assert( itsPrefsStylesMenu != nullptr );
	itsPrefsStylesMenu->SetMenuItems(kPrefsStylesMenuStr, "TextDocument");
	itsPrefsStylesMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsStylesMenu);

	itsHelpMenu = GetApplication()->CreateHelpMenu(itsMenuBar, "TextDocument");
	ListenTo(itsHelpMenu);

	// must do this after creating widgets

	JPrefObject::ReadPrefs();

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		JXTextMenu* editMenu;
		bool ok = itsTextEditor->GetEditMenu(&editMenu);
		assert( ok );

		JIndex undo, redo, cut, copy, paste, left, right;
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kUndoCmd, &undo);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kRedoCmd, &redo);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kCutCmd, &cut);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &copy);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kPasteCmd, &paste);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kShiftSelLeftCmd, &left);
		assert( ok );
		ok = itsTextEditor->EditMenuCmdToIndex(JTextEditor::kShiftSelRightCmd, &right);
		assert( ok );

		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kSaveFileCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveCopyAsCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveAllFilesCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kPrintPTCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(editMenu, undo);
		itsToolBar->AppendButton(editMenu, redo);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(editMenu, cut);
		itsToolBar->AppendButton(editMenu, copy);
		itsToolBar->AppendButton(editMenu, paste);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(editMenu, left);
		itsToolBar->AppendButton(editMenu, right);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, itsHelpMenu);
	}
}

/******************************************************************************
 InsertTextMenu

 ******************************************************************************/

JXTextMenu*
TextDocument::InsertTextMenu
	(
	const JString& title
	)
{
	JIndex i;
	const bool found = itsMenuBar->FindMenu(itsWindowMenu, &i);
	assert( found );
	return itsMenuBar->InsertTextMenu(i, title);
}

/******************************************************************************
 GetFileDisplayInfo

 ******************************************************************************/

JRect
TextDocument::GetFileDisplayInfo
	(
	JXWidget::HSizingOption* hSizing,
	JXWidget::VSizingOption* vSizing
	)
	const
{
	*hSizing = itsFileDisplay->GetHSizing();
	*vSizing = itsFileDisplay->GetVSizing();

	return JCovering(itsFileDragSource->GetFrame(), itsFileDisplay->GetFrame());
}

/******************************************************************************
 SetFileDisplayVisible

 ******************************************************************************/

void
TextDocument::SetFileDisplayVisible
	(
	const bool show
	)
{
	itsFileDragSource->SetVisible(show);
	itsFileDisplay->SetVisible(show);
}

/******************************************************************************
 DisplayFileName (private)

	The file name is displayed at the bottom of the window.

 ******************************************************************************/

void
TextDocument::DisplayFileName
	(
	const JString& name
	)
{
	itsFileDisplay->GetText()->SetText(ExistsOnDisk() ? name : JString::empty);
	itsFileDisplay->DiskCopyIsModified(false);
}

/******************************************************************************
 HandleFileModifiedByOthers (virtual protected)

	Display the file name in red.

 ******************************************************************************/

void
TextDocument::HandleFileModifiedByOthers
	(
	const bool modTimeChanged,
	const bool permsChanged
	)
{
	JXFileDocument::HandleFileModifiedByOthers(modTimeChanged, permsChanged);

	if (modTimeChanged)
	{
		itsFileDisplay->DiskCopyIsModified(true);
	}

	// if file becomes writable, update "Read only" setting
	// (Never automatically go the other way, because that is annoying!)

	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk && JFileWritable(fullName))
	{
		itsTextEditor->UpdateWritable(fullName);
	}
}

/******************************************************************************
 RefreshVCSStatus

 ******************************************************************************/

void
TextDocument::RefreshVCSStatus()
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
	{
		itsTextEditor->UpdateWritable(fullName);
	}
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
TextDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateFileMenu();
	}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
	}

	else if (sender == itsFileFormatMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateFileFormatMenu();
	}
	else if (sender == itsFileFormatMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileFormatMenu(selection->GetIndex());
	}

	else if (sender == itsDiffMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateDiffMenu();
	}
	else if (sender == itsDiffMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleDiffMenu(selection->GetIndex());
	}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdatePrefsMenu();
	}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsMenu(selection->GetIndex());
	}

	else if (sender == itsPrefsStylesMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdatePrefsStylesMenu();
	}
	else if (sender == itsPrefsStylesMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsStylesMenu(selection->GetIndex());
	}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		GetApplication()->UpdateHelpMenu(itsHelpMenu);
	}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		GetApplication()->HandleHelpMenu(itsHelpMenu, itsHelpSectionName,
											 selection->GetIndex());
	}

	else if (sender == itsSettingsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateSettingsMenu();
	}
	else if (sender == itsSettingsMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSettingsMenu(selection->GetIndex());
	}

	else if (sender == itsActionButton && message.Is(JXButton::kPushed))
	{
		HandleActionButton();
	}

	else if (sender == itsTextEditor &&
			 message.Is(JStyledText::kTextChanged))
	{
		if (itsTextEditor->GetText()->IsAtLastSaveLocation())
		{
			DataReverted(true);
		}
		else
		{
			DataModified();
		}
		GetDocumentManager()->TextDocumentNeedsSave();
	}

	else if (sender == itsTextEditor && message.Is(JTextEditor::kTypeChanged))
	{
		UpdateReadOnlyDisplay();
	}

	else if (sender == itsTabWidthDialog && message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			OverrideTabWidth(itsTabWidthDialog->GetTabCharCount());
		}
		itsTabWidthDialog = nullptr;
	}

	else
	{
		if (sender == this && message.Is(JXFileDocument::kNameChanged))
		{
			const auto* info =
				dynamic_cast<const JXFileDocument::NameChanged*>(&message);
			assert( info != nullptr );
			DisplayFileName(info->GetFullName());
			UpdateFileType();
		}

		JXFileDocument::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
TextDocument::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kSaveFileCmd, NeedsSave());
	itsFileMenu->SetItemEnable(kRevertCmd, CanRevert());
	itsFileMenu->SetItemEnable(kSaveAllFilesCmd,
							   GetDocumentManager()->TextDocumentsNeedSave());

	bool enable, onDisk;
	const JString fullName = GetFullName(&onDisk);
	itsFileMenu->SetItemText(kDiffSmartCmd,
		(GetDiffFileDialog())->GetSmartDiffItemText(onDisk, fullName, &enable));
	itsFileMenu->SetItemEnable(kDiffSmartCmd, enable);

	enable = false;
	const JString* s;
	if (onDisk)
	{
		const JVCSType type = JGetVCSType(GetFilePath());
		if (type == kJCVSType)
		{
			enable = true;
			s = & JGetString("DiffCVSMenuText::TextDocument");
		}
		else if (type == kJSVNType)
		{
			enable = true;
			s = & JGetString("DiffSVNMenuText::TextDocument");
		}
		else if (type == kJGitType)
		{
			enable = true;
			s = & JGetString("DiffGitMenuText::TextDocument");
		}
		else
		{
			enable = true;
			s = & JGetString("DiffVCSMenuText::TextDocument");
		}
	}
	else
	{
		s = & JGetString("DiffVCSMenuText::TextDocument");
	}
	itsFileMenu->SetItemText(kDiffVCSCmd, *s);
	itsFileMenu->SetItemEnable(kDiffVCSCmd, enable);

	const bool hasText = !itsTextEditor->GetText()->IsEmpty();
	itsFileMenu->SetItemEnable(kPrintPTCmd, hasText);

	const bool isStyled = itsTextEditor->GetText()->GetStyles().GetRunCount() > 1;
	itsFileMenu->SetItemEnable(kPSPageSetupCmd, isStyled);
	itsFileMenu->SetItemEnable(kPrintPSCmd, hasText && isStyled);
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
TextDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kNewTextEditorCmd)
	{
		GetDocumentManager()->NewTextDocument();
	}
	else if (index == kNewTextTemplateCmd)
	{
		GetDocumentManager()->NewTextDocumentFromTemplate();
	}
	else if (index == kNewProjectCmd)
	{
		GetDocumentManager()->NewProjectDocument();
	}
	else if (index == kNewShellCmd)
	{
		GetDocumentManager()->NewShellDocument();
	}
	else if (index == kOpenSomethingCmd)
	{
		if (OpenSomething())
		{
			return;		// we closed ourselves
		}
	}
	else if (index == kSaveFileCmd)
	{
		Save();
	}
	else if (index == kSaveFileAsCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		GetDocumentManager()->
			AddToFileHistoryMenu(DocumentManager::kTextFileHistory, fullName);

		SaveInNewFile();
	}
	else if (index == kSaveCopyAsCmd)
	{
		JString fullName;
		if (SaveCopyInNewFile(JString::empty, &fullName))
		{
			GetDocumentManager()->
				AddToFileHistoryMenu(DocumentManager::kTextFileHistory, fullName);
		}
	}
	else if (index == kSaveAsTemplateCmd)
	{
		JString origName;
		if (GetDocumentManager()->GetTextTemplateDirectory(true, &origName))
		{
			origName = JCombinePathAndName(origName, GetFileName());
			SaveCopyInNewFile(origName);
		}
	}
	else if (index == kRevertCmd)
	{
		RevertToSaved();
	}
	else if (index == kSaveAllFilesCmd)
	{
		GetDocumentManager()->SaveTextDocuments(true);
	}

	else if (index == kDiffFilesCmd)
	{
		(GetDiffFileDialog())->Activate();
	}
	else if (index == kDiffSmartCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
		{
			Save();
			(GetDiffFileDialog())->ViewDiffs(onDisk, fullName);
		}
	}
	else if (index == kDiffVCSCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
		{
			Save();
			(GetDiffFileDialog())->ViewVCSDiffs(fullName);
		}
	}

	else if (index == kShowInFileMgrCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
		{
			(JXGetWebBrowser())->ShowFileLocation(fullName);
		}
	}

	else if (index == kPTPageSetupCmd)
	{
		itsTextEditor->HandlePTPageSetup();
	}
	else if (index == kPrintPTCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		PTPrinter* p = GetPTTextPrinter();
		p->SetHeaderName(fullName);
		p->SetTabWidth(itsTextEditor->GetTabCharCount());
		itsTextEditor->PrintPT();
	}

	else if (index == kPSPageSetupCmd)
	{
		itsTextEditor->HandlePSPageSetup();
	}
	else if (index == kPrintPSCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		(GetPSTextPrinter())->SetPrintInfo(itsTextEditor, fullName);
		itsTextEditor->PrintPS();
	}

	else if (index == kCloseCmd)
	{
		Close();
	}
	else if (index == kCloseAllCmd)
	{
		GetDocumentManager()->CloseTextDocuments();
	}

	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 OpenSomething (private)

	If "Open..." is invoked from an empty, unmodified document that is
	not on disk, and the user opens a text document, then we close ourselves.

	Returns true if we closed ourselves.

 ******************************************************************************/

bool
TextDocument::OpenSomething()
{
	DocumentManager* docMgr = GetDocumentManager();
	const JSize textDocCount  = docMgr->GetTextDocumentCount();

	docMgr->OpenSomething();

	if (docMgr->GetTextDocumentCount() > textDocCount &&
		itsTextEditor->GetText()->IsEmpty() &&
		!NeedsSave() && !ExistsOnDisk())
	{
		Close();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 DiscardChanges (virtual protected)

 ******************************************************************************/

void
TextDocument::DiscardChanges()
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
	{
		// save location of caret and scrollbars

		JIndex caretIndex = itsTextEditor->GetInsertionCharIndex();
		const JRect ap    = itsTextEditor->GetAperture();

		// re-read file

		DataReverted();			// must come first since file may need cleaning
		ReadFile(fullName, false);
		itsTextEditor->UpdateWritable(fullName);

		// restore location of caret and scrollbars

		itsTextEditor->ScrollTo(ap.topLeft());	// this first to avoid 2 redraws

		const JSize textLength = itsTextEditor->GetText()->GetText().GetCharacterCount();
		if (caretIndex > textLength + 1)
		{
			caretIndex = textLength + 1;
		}
		itsTextEditor->SetCaretLocation(caretIndex);
	}
	else
	{
		itsTextEditor->GetText()->SetText(JString::empty);
		DataReverted();
	}

	itsFileDisplay->DiskCopyIsModified(false);
}

/******************************************************************************
 OpenAsBinaryFile (static)

	Returns true if the file contains illegal characters and the user
	wants to open it as a binary file.

 ******************************************************************************/

bool
TextDocument::OpenAsBinaryFile
	(
	const JString& fileName
	)
{
	JString s;
	JReadFile(fileName, &s);

	if (JStyledText::ContainsIllegalChars(s))
	{
		const JUtf8Byte* map[] =
		{
			"name", fileName.GetBytes()
		};
		const JString msg = JGetString("OpenBinaryFileMessage::TextDocument", map, sizeof(map));
		return !JGetUserNotification()->AskUserNo(msg);
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 ReadFile (protected)

 ******************************************************************************/

void
TextDocument::ReadFile
	(
	const JString&	fileName,
	const bool		firstTime
	)
{
	if (!itsTextEditor->GetText()->ReadPlainText(fileName, &itsFileFormat))
	{
		// Don't let them overwrite it without thinking about it.

		const JString name = JGetString("BinaryFilePrefix::TextDocument") + GetFileName();
		FileChanged(name, false);

		// Display the full path of the original file.
		// (FileChanged() changes the display.)

		JString fullName;
		const bool ok = JGetTrueName(fileName, &fullName);
		assert( ok );
		DisplayFileName(fullName);
		itsTextEditor->UpdateWritable(fullName);
	}

	bool setTabWidth, setTabMode, tabInsertsSpaces, setAutoIndent, autoIndent;
	JSize tabWidth;
	ParseEditorOptions(fileName, itsTextEditor->GetText()->GetText(), &setTabWidth, &tabWidth,
					   &setTabMode, &tabInsertsSpaces, &setAutoIndent, &autoIndent);

	if (setTabWidth)
	{
		OverrideTabWidth(tabWidth);
	}
	if (setTabMode)
	{
		OverrideTabInsertsSpaces(tabInsertsSpaces);
	}
	if (setAutoIndent)
	{
		OverrideAutoIndent(autoIndent);
	}

	if (firstTime)
	{
		const bool spaces = itsTextEditor->GetText()->TabInsertsSpaces();

		JSize tabWidth = itsTextEditor->GetTabCharCount();
		itsTextEditor->AnalyzeWhitespace(&tabWidth);

		if (setTabMode)
		{
			itsTextEditor->GetText()->TabShouldInsertSpaces(spaces);
		}

		if (!setTabWidth)
		{
			itsTextEditor->SetTabCharCount(tabWidth);
		}

		JPtrArray<JString> safetyFilesToOpen(JPtrArrayT::kDeleteAll);
		if (CheckForSafetySaveFiles(fileName, &safetyFilesToOpen))
		{
			const JSize count = safetyFilesToOpen.GetElementCount();
			for (JIndex i=1; i<=count; i++)
			{
				GetDocumentManager()->
					OpenTextDocument(*(safetyFilesToOpen.GetElement(i)));
			}
		}
	}

	UpdateFileType();
	itsTextEditor->GetText()->SetLastSaveLocation();
}

/******************************************************************************
 WriteTextFile (virtual protected)

 ******************************************************************************/

void
TextDocument::WriteTextFile
	(
	std::ostream&	output,
	const bool	safetySave
	)
	const
{
	itsTextEditor->GetText()->WritePlainText(output, itsFileFormat);
	if (!safetySave)
	{
		itsTextEditor->GetText()->DeactivateCurrentUndo();

		if (output.good())
		{
			itsTextEditor->GetText()->SetLastSaveLocation();

			const_cast<TextDocument*>(this)->UpdateFileType();

			itsFileDisplay->DiskCopyIsModified(false);

			const_cast<TextDocument*>(this)->Broadcast(Saved());
		}
	}
}

/******************************************************************************
 ReadFromProject (private)

	Returns false if it cannot find the file.

 ******************************************************************************/

bool
TextDocument::ReadFromProject
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	bool onDisk;
	input >> JBoolFromString(onDisk);
	if (!onDisk)
	{
		return false;
	}

	JString fullName, ptPrintName, psPrintName;
	input >> fullName;
	if (vers >= 29)
	{
		input >> ptPrintName >> psPrintName;
	}

	GetWindow()->ReadGeometry(input);

	JIndex insertionIndex;
	input >> insertionIndex;

	bool autoIndent, tabInsertsSpaces, showWS, wordWrap, readOnly;
	JSize tabCharCount;
	if (vers >= 68)
	{
		input >> JBoolFromString(itsOverrideFlag[ kAutoIndentIndex ])     >> JBoolFromString(autoIndent);
		input >> JBoolFromString(itsOverrideFlag[ kShowWhitespaceIndex ]) >> JBoolFromString(showWS);
		input >> JBoolFromString(itsOverrideFlag[ kWordWrapIndex ])       >> JBoolFromString(wordWrap);
		input >> JBoolFromString(itsOverrideFlag[ kReadOnlyIndex ])       >> JBoolFromString(readOnly);
	}
	if (vers >= 69)
	{
		input >> JBoolFromString(itsOverrideFlag[ kTabWidthIndex ]) >> tabCharCount;
	}
	if (70 <= vers && vers < 84)
	{
		bool override;
		JString charSet;
		input >> JBoolFromString(override) >> charSet;
	}
	if (vers >= 82)
	{
		input >> JBoolFromString(itsOverrideFlag[ kTabInsertsSpacesIndex ]) >> JBoolFromString(tabInsertsSpaces);
	}

	JXFileDocument* existingDoc;
	if (JFileReadable(fullName) &&
		!(JXGetDocumentManager())->FileDocumentIsOpen(fullName, &existingDoc))
	{
		FileChanged(fullName, true);
		ReadFile(fullName, true);

		itsTextEditor->SetPTPrintFileName(ptPrintName);
		itsTextEditor->SetPSPrintFileName(psPrintName);

		if (itsOverrideFlag[ kAutoIndentIndex ])
		{
			itsTextEditor->GetText()->ShouldAutoIndent(autoIndent);
		}
		if (itsOverrideFlag[ kTabInsertsSpacesIndex ])
		{
			itsTextEditor->GetText()->TabShouldInsertSpaces(tabInsertsSpaces);
		}
		if (itsOverrideFlag[ kShowWhitespaceIndex ])
		{
			itsTextEditor->ShouldShowWhitespace(showWS);
		}
		if (itsOverrideFlag[ kWordWrapIndex ])
		{
			itsTextEditor->SetBreakCROnly(wordWrap);
		}
		if (itsOverrideFlag[ kReadOnlyIndex ])
		{
			itsTextEditor->SetWritable(!readOnly);
		}
		if (itsOverrideFlag[ kTabWidthIndex ])
		{
			itsTextEditor->SetTabCharCount(tabCharCount);
		}

		if (vers >= 22)
		{
			itsTextEditor->ReadScrollSetup(input);
		}
		else if (vers >= 18)
		{
			JXScrollbar *hScrollbar, *vScrollbar;
			const bool ok = itsTextEditor->GetScrollbars(&hScrollbar, &vScrollbar);
			assert( ok );
			vScrollbar->ReadSetup(input);
		}

		const JIndex lineIndex = itsTextEditor->GetLineForChar(insertionIndex);
		if (lineIndex > 1)
		{
			itsTextEditor->GoToLine(lineIndex);
		}
		return true;
	}
	else
	{
		if (vers >= 22)
		{
			JXScrollableWidget::SkipScrollSetup(input);
		}
		else if (vers >= 18)
		{
			JXScrollbar::SkipSetup(input);
		}

		return false;
	}
}

/******************************************************************************
 WriteForProject

 ******************************************************************************/

void
TextDocument::WriteForProject
	(
	std::ostream& output
	)
	const
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	output << JBoolToString(onDisk);

	if (onDisk)
	{
		output << ' ' << fullName;
		output << ' ' << itsTextEditor->GetPTPrintFileName();
		output << ' ' << itsTextEditor->GetPSPrintFileName();

		output << ' ';
		GetWindow()->WriteGeometry(output);

		output << ' ' << itsTextEditor->GetInsertionCharIndex();

		output << ' ' << JBoolToString(itsOverrideFlag[ kAutoIndentIndex ]);
		output << JBoolToString(itsTextEditor->GetText()->WillAutoIndent());

		output << JBoolToString(itsOverrideFlag[ kShowWhitespaceIndex ]);
		output << JBoolToString(itsTextEditor->WillShowWhitespace());

		output << JBoolToString(itsOverrideFlag[ kWordWrapIndex ]);
		output << JBoolToString(itsTextEditor->WillBreakCROnly());

		output << JBoolToString(itsOverrideFlag[ kReadOnlyIndex ]);
		output << JBoolToString(itsTextEditor->IsReadOnly());

		output << JBoolToString(itsOverrideFlag[ kTabWidthIndex ]);
		output << ' ' << itsTextEditor->GetTabCharCount();

		output << ' ' << JBoolToString(itsOverrideFlag[ kTabInsertsSpacesIndex ]);
		output << JBoolToString(itsTextEditor->GetText()->TabInsertsSpaces());

		output << ' ';
		itsTextEditor->WriteScrollSetup(output);
	}
}

/******************************************************************************
 ReadStaticGlobalPrefs (static)

 ******************************************************************************/

void
TextDocument::ReadStaticGlobalPrefs
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	if (vers < 25)
	{
		bool copyWhenSelect;
		input >> JBoolFromString(copyWhenSelect);

		if (JXGetSharedPrefsManager()->WasNew())
		{
			JTextEditor::ShouldCopyWhenSelect(copyWhenSelect);
		}
	}

	if (21 <= vers && vers < 25)
	{
		bool windowsHomeEnd, scrollCaret;
		input >> JBoolFromString(windowsHomeEnd) >> JBoolFromString(scrollCaret);

		if (JXGetSharedPrefsManager()->WasNew())
		{
			JXTEBase::ShouldUseWindowsHomeEnd(windowsHomeEnd);
			JXTEBase::CaretShouldFollowScroll(scrollCaret);
		}
	}

	if (vers >= 17)
	{
		bool askOKToClose;
		input >> JBoolFromString(askOKToClose);
		JXFileDocument::ShouldAskOKToClose(askOKToClose);
	}
}

/******************************************************************************
 WriteStaticGlobalPrefs (static)

 ******************************************************************************/

void
TextDocument::WriteStaticGlobalPrefs
	(
	std::ostream& output
	)
{
	output << ' ' << JBoolToString(JXFileDocument::WillAskOKToClose());
}

/******************************************************************************
 UpdateFileFormatMenu (private)

 ******************************************************************************/

void
TextDocument::UpdateFileFormatMenu()
{
	if (itsFileFormat == JStyledText::kUNIXText)
	{
		itsFileFormatMenu->CheckItem(kUNIXFmtCmd);
	}
	else if (itsFileFormat == JStyledText::kMacintoshText)
	{
		itsFileFormatMenu->CheckItem(kMacFmtCmd);
	}
	else if (itsFileFormat == JStyledText::kDOSText)
	{
		itsFileFormatMenu->CheckItem(kDOSFmtCmd);
	}
}

/******************************************************************************
 HandleFileFormatMenu (private)

 ******************************************************************************/

void
TextDocument::HandleFileFormatMenu
	(
	const JIndex index
	)
{
	const JStyledText::PlainTextFormat origFormat = itsFileFormat;

	if (index == kUNIXFmtCmd)
	{
		itsFileFormat = JStyledText::kUNIXText;
	}
	else if (index == kMacFmtCmd)
	{
		itsFileFormat = JStyledText::kMacintoshText;
	}
	else if (index == kDOSFmtCmd)
	{
		itsFileFormat = JStyledText::kDOSText;
	}

	if (itsFileFormat != origFormat)
	{
		itsTextEditor->GetText()->ClearLastSaveLocation();
		DataModified();
	}
}

/******************************************************************************
 UpdateDiffMenu (private)

 ******************************************************************************/

void
TextDocument::UpdateDiffMenu()
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);

	bool enable = false;
	JString s("...");
	if (onDisk)
	{
		itsDiffMenu->EnableItem(kDiffAs1Cmd);
		itsDiffMenu->EnableItem(kDiffAs2Cmd);

		const JVCSType type = JGetVCSType(GetFilePath());
		if (type == kJCVSType)
		{
			enable = true;
			s.Prepend(JGetString("DiffCVSMenuText::TextDocument"));
		}
		else if (type == kJSVNType)
		{
			enable = true;
			s.Prepend(JGetString("DiffSVNMenuText::TextDocument"));
		}
		else if (type == kJGitType)
		{
			enable = true;
			s.Prepend(JGetString("DiffGitMenuText::TextDocument"));
		}
		else
		{
			s.Prepend(JGetString("DiffVCSMenuText::TextDocument"));
		}
	}
	else
	{
		s.Prepend(JGetString("DiffVCSMenuText::TextDocument"));
	}

	itsDiffMenu->SetItemText(kDiffAsVCSCmd, s);
	itsDiffMenu->SetItemEnable(kDiffAsVCSCmd, enable);
}

/******************************************************************************
 HandleDiffMenu (private)

 ******************************************************************************/

void
TextDocument::HandleDiffMenu
	(
	const JIndex index
	)
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (!onDisk)
	{
		return;
	}

	Save();

	if (index == kDiffAs1Cmd)
	{
		(GetDiffFileDialog())->SetFile1(fullName);
	}
	else if (index == kDiffAs2Cmd)
	{
		(GetDiffFileDialog())->SetFile2(fullName);
	}

	else if (index == kDiffAsVCSCmd)
	{
		const JVCSType type = JGetVCSType(GetFilePath());
		if (type == kJCVSType)
		{
			(GetDiffFileDialog())->SetCVSFile(fullName);
		}
		else if (type == kJSVNType)
		{
			(GetDiffFileDialog())->SetSVNFile(fullName);
		}
		else if (type == kJGitType)
		{
			(GetDiffFileDialog())->SetGitFile(fullName);
		}
	}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
TextDocument::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
TextDocument::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditorPrefsCmd)
	{
		EditPrefs();
	}
	else if (index == kToolBarPrefsCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kChooseExtEditorsCmd)
	{
		GetDocumentManager()->ChooseEditors();
	}
	else if (index == kWWWPrefsCmd)
	{
		(JXGetWebBrowser())->EditPrefs();
	}
	else if (index == kMiscPrefsCmd)
	{
		GetApplication()->EditMiscPrefs();
	}

	else if (index == kEditFileTypesCmd)
	{
		GetPrefsManager()->EditFileTypes();
	}
	else if (index == kEditMacrosCmd)
	{
		GetPrefsManager()->EditMacros(itsMacroMgr);
	}
	else if (index == kEditCRMRuleListsCmd)
	{
		GetPrefsManager()->EditCRMRuleLists(itsCRMRuleList);
	}

	else if (index == kSaveWindSizeCmd)
	{
		SaveWindowSize();
	}
}

/******************************************************************************
 UpdatePrefsStylesMenu (private)

 ******************************************************************************/

void
TextDocument::UpdatePrefsStylesMenu()
{
}

/******************************************************************************
 HandlePrefsStylesMenu (private)

 ******************************************************************************/

void
TextDocument::HandlePrefsStylesMenu
	(
	const JIndex index
	)
{
	StylerBase* styler;
	if (::GetStyler(kMenuItemToLang[index-1], &styler))
	{
		styler->EditStyles();
	}
}

/******************************************************************************
 UpdateSettingsMenu (private)

 ******************************************************************************/

void
TextDocument::UpdateSettingsMenu()
{
	const JString tabWidth(itsTextEditor->GetTabCharCount(), JString::kBase10);

	const JUtf8Byte* map[] =
{
		"n", tabWidth.GetBytes()
};
	const JString text = JGetString("ChangeSpacesPerTabMenuItem::TextDocument", map, sizeof(map));
	itsSettingsMenu->SetItemText(kTabWidthCmd, text);

	if (itsTextEditor->GetText()->WillAutoIndent())
	{
		itsSettingsMenu->CheckItem(kToggleAutoIndentCmd);
	}

	if (itsTextEditor->GetText()->TabInsertsSpaces())
	{
		itsSettingsMenu->CheckItem(kToggleTabInsertsSpacesCmd);
	}

	if (itsTextEditor->WillShowWhitespace())
	{
		itsSettingsMenu->CheckItem(kToggleWhitespaceCmd);
	}

	if (!itsTextEditor->WillBreakCROnly())
	{
		itsSettingsMenu->CheckItem(kToggleWordWrapCmd);
	}

	if (itsTextEditor->IsReadOnly())
	{
		itsSettingsMenu->CheckItem(kToggleReadOnlyCmd);
	}
}

/******************************************************************************
 HandleSettingsMenu (private)

 ******************************************************************************/

void
TextDocument::HandleSettingsMenu
	(
	const JIndex index
	)
{
	if (index == kTabWidthCmd)
	{
		assert( itsTabWidthDialog == nullptr );
		itsTabWidthDialog = jnew TabWidthDialog(this, itsTextEditor->GetTabCharCount());
		assert( itsTabWidthDialog != nullptr );
		itsTabWidthDialog->BeginDialog();
		ListenTo(itsTabWidthDialog);
	}

	else if (index == kToggleAutoIndentCmd)
	{
		OverrideAutoIndent( !itsTextEditor->GetText()->WillAutoIndent() );
	}
	else if (index == kToggleTabInsertsSpacesCmd)
	{
		OverrideTabInsertsSpaces( !itsTextEditor->GetText()->TabInsertsSpaces() );
	}
	else if (index == kToggleWhitespaceCmd)
	{
		OverrideShowWhitespace( !itsTextEditor->WillShowWhitespace() );
	}
	else if (index == kToggleWordWrapCmd)
	{
		OverrideBreakCROnly( !itsTextEditor->WillBreakCROnly() );
	}
	else if (index == kToggleReadOnlyCmd)
	{
		OverrideReadOnly( !itsTextEditor->IsReadOnly() );
	}
}

/******************************************************************************
 OverrideTabWidth

 ******************************************************************************/

void
TextDocument::OverrideTabWidth
	(
	const JSize tabWidth
	)
{
	itsOverrideFlag[ kTabWidthIndex ] = true;
	itsTextEditor->SetTabCharCount(tabWidth);
}

/******************************************************************************
 OverrideAutoIndent

 ******************************************************************************/

void
TextDocument::OverrideAutoIndent
	(
	const bool autoIndent
	)
{
	itsOverrideFlag[ kAutoIndentIndex ] = true;
	itsTextEditor->GetText()->ShouldAutoIndent(autoIndent);
}

/******************************************************************************
 OverrideTabInsertsSpaces

 ******************************************************************************/

void
TextDocument::OverrideTabInsertsSpaces
	(
	const bool insertSpaces
	)
{
	itsOverrideFlag[ kTabInsertsSpacesIndex ] = true;
	itsTextEditor->GetText()->TabShouldInsertSpaces(insertSpaces);
}

/******************************************************************************
 OverrideShowWhitespace

 ******************************************************************************/

void
TextDocument::OverrideShowWhitespace
	(
	const bool showWhitespace
	)
{
	itsOverrideFlag[ kShowWhitespaceIndex ] = true;
	itsTextEditor->ShouldShowWhitespace(showWhitespace);
}

/******************************************************************************
 OverrideBreakCROnly

 ******************************************************************************/

void
TextDocument::OverrideBreakCROnly
	(
	const bool breakCROnly
	)
{
	itsOverrideFlag[ kWordWrapIndex ] = true;
	itsTextEditor->SetBreakCROnly(breakCROnly);
}

/******************************************************************************
 OverrideReadOnly

 ******************************************************************************/

void
TextDocument::OverrideReadOnly
	(
	const bool readOnly
	)
{
	itsOverrideFlag[ kReadOnlyIndex ] = true;
	itsTextEditor->SetWritable(!readOnly);
}

/******************************************************************************
 GoToLine

 ******************************************************************************/

void
TextDocument::GoToLine
	(
	const JIndex lineIndex
	)
	const
{
	itsTextEditor->GoToLine(lineIndex);
}

/******************************************************************************
 SelectLines

 ******************************************************************************/

void
TextDocument::SelectLines
	(
	const JIndexRange& range
	)
	const
{
	::SelectLines(itsTextEditor, range);
}

/******************************************************************************
 UpdateFileType

 ******************************************************************************/

void
TextDocument::UpdateFileType
	(
	const bool init
	)
{
	if (!itsUpdateFileTypeFlag)
	{
		return;
	}

	const TextFileType origType = itsFileType;

	StylerBase* origStyler;
	::GetStyler(itsFileType, &origStyler);

	JString scriptPath;
	bool wordWrap;
	itsFileType =
		GetPrefsManager()->GetFileType(*this, &itsActionMgr, &itsMacroMgr,
										 &itsCRMRuleList, &scriptPath, &wordWrap);

	if ((!init && itsFileType != origType) || !itsOverrideFlag[ kWordWrapIndex ])
	{
		itsTextEditor->SetBreakCROnly(!wordWrap);
	}
	itsTextEditor->GetText()->SetCRMRuleList(itsCRMRuleList, false);
	itsTextEditor->SetScriptPath(scriptPath);

	StylerBase* newStyler;
	::GetStyler(itsFileType, &newStyler);
	if (newStyler != origStyler)
	{
		itsTextEditor->RecalcStyles();
	}

	itsTextEditor->FileTypeChanged(itsFileType);

	// action button

	if (HasComplementType(itsFileType) ||
		itsFileType == kHTMLFT         ||
		itsFileType == kXMLFT)
	{
		if (!itsActionButton->WouldBeVisible())
		{
			itsActionButton->Show();
			itsMenuBar->AdjustSize(-itsActionButton->GetFrameWidth(), 0);
		}

		if (itsFileType == kHTMLFT || itsFileType == kXMLFT)
		{
			itsActionButton->SetLabel(JGetString("HTMLButton::TextDocument"));
		}
		else
		{
			itsActionButton->SetLabel(GetComplementFileTypeName(itsFileType));
		}
	}

	else if (itsActionButton->WouldBeVisible())
	{
		itsActionButton->Hide();
		itsMenuBar->AdjustSize(itsActionButton->GetFrameWidth(), 0);
	}
}

/******************************************************************************
 StylerChanged

 ******************************************************************************/

void
TextDocument::StylerChanged
	(
	JSTStyler* styler
	)
{
	StylerBase* myStyler;
	::GetStyler(itsFileType, &myStyler);
	if (styler == myStyler)
	{
		itsTextEditor->RecalcStyles();
	}
}

/******************************************************************************
 GetStyler

	Returns the styler to be used for this document, if any.  Not inline
	to avoid including JSTStyler.h.

 ******************************************************************************/

bool
TextDocument::GetStyler
	(
	StylerBase** styler
	)
	const
{
	return ::GetStyler(itsFileType, styler) && (**styler).IsActive();
}

/******************************************************************************
 UpdateReadOnlyDisplay (private)

 ******************************************************************************/

void
TextDocument::UpdateReadOnlyDisplay()
{
	itsFileDragSource->SetBackColor(
		itsTextEditor->GetType() == JTextEditor::kFullEditor ?
		JColorManager::GetDefaultBackColor() :
		JColorManager::GetDarkRedColor());
}

/******************************************************************************
 HandleActionButton

 ******************************************************************************/

void
TextDocument::HandleActionButton()
{
	if (itsActionButton->IsVisible())
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);

		if ((itsFileType == kHTMLFT || itsFileType == kXMLFT) && Save())
		{
			JXGetWebBrowser()->ShowFileContent(fullName);
		}
		else
		{
			GetDocumentManager()->OpenComplementFile(fullName, itsFileType);
		}
	}
}

/******************************************************************************
 GetWindowSize

	This cannot be virtual because it is called by the constructor.

 ******************************************************************************/

bool
TextDocument::GetWindowSize
	(
	JPoint*			desktopLoc,
	JCoordinate*	width,
	JCoordinate*	height
	)
	const
{
	PrefsManager* prefsMgr = GetPrefsManager();
	if (itsFileType == kExecOutputFT || itsFileType == kShellOutputFT)
	{
		return prefsMgr->GetWindowSize(kExecOutputWindSizeID, desktopLoc, width, height);
	}
	else if (itsFileType == kSearchOutputFT)
	{
		return prefsMgr->GetWindowSize(kSearchOutputWindSizeID, desktopLoc, width, height);
	}

	bool onDisk;
	const JString thisFile = GetFullName(&onDisk);

	DocumentManager* docMgr = GetDocumentManager();

	JXFileDocument* doc;
	if (itsOpenOverComplementFlag &&
		docMgr->GetOpenComplementFile(thisFile, itsFileType, &doc))
	{
		JXWindow* window = doc->GetWindow();
		*desktopLoc      = window->GetDesktopLocation();
		*width           = window->GetFrameWidth();
		*height          = window->GetFrameHeight();
		return true;
	}

	else if (UseCSourceWindowSize(itsFileType))
	{
		return prefsMgr->GetWindowSize(kSourceWindSizeID, desktopLoc, width, height);
	}
	else if (IsHeaderType(itsFileType))
	{
		return prefsMgr->GetWindowSize(kHeaderWindSizeID, desktopLoc, width, height);
	}
	else
	{
		return prefsMgr->GetWindowSize(kOtherTextWindSizeID, desktopLoc, width, height);
	}
}

/******************************************************************************
 SaveWindowSize

 ******************************************************************************/

void
TextDocument::SaveWindowSize()
	const
{
	JXWindow* window = GetWindow();
	assert( window != nullptr );

	PrefsManager* prefsMgr = GetPrefsManager();

	if (itsFileType == kExecOutputFT || itsFileType == kShellOutputFT)
	{
		prefsMgr->SaveWindowSize(kExecOutputWindSizeID, window);
	}
	else if (itsFileType == kSearchOutputFT)
	{
		prefsMgr->SaveWindowSize(kSearchOutputWindSizeID, window);
	}
	else if (UseCSourceWindowSize(itsFileType))
	{
		prefsMgr->SaveWindowSize(kSourceWindSizeID, window);
	}
	else if (IsHeaderType(itsFileType))
	{
		prefsMgr->SaveWindowSize(kHeaderWindSizeID, window);
	}
	else
	{
		prefsMgr->SaveWindowSize(kOtherTextWindSizeID, window);
	}
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
TextDocument::EditPrefs()
{
	auto* dlog = jnew EditTextPrefsDialog(this);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
TextDocument::ReadPrefs
	(
	std::istream& input
	)
{
	assert( itsTextEditor != nullptr );

	JFileVersion vers;
	input >> vers;

	if (vers == 0)
	{
		bool openOverComplement;
		input >> JBoolFromString(openOverComplement);
		ShouldOpenComplFileOnTop(openOverComplement);
	}
	else if (vers == 1)
	{
		bool breakCodeCROnly, openOverComplement;
		input >> JBoolFromString(breakCodeCROnly) >> JBoolFromString(openOverComplement);
		ShouldOpenComplFileOnTop(openOverComplement);
	}
	else if (vers <= kCurrentSetupVersion)
	{
		bool openOverComplement;
		input >> JBoolFromString(openOverComplement);
		ShouldOpenComplFileOnTop(openOverComplement);
	}

	JIgnoreUntil(input, kSetupDataEndDelimiter);

	ReadJXFDSetup(input);
	itsTextEditor->ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
TextDocument::WritePrefs
	(
	std::ostream& output
	)
	const
{
	assert( itsTextEditor != nullptr );

	output << kCurrentSetupVersion;
	output << ' ' << JBoolToString(itsOpenOverComplementFlag);
	output << kSetupDataEndDelimiter;

	WriteJXFDSetup(output);
	itsTextEditor->WriteSetup(output);
}
