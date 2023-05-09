/******************************************************************************
 FileListDirector.cpp

	Window to display all files found by ParseFiles().

	BASE CLASS = JXWindowDirector

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "FileListDirector.h"
#include "FileListTable.h"
#include "ProjectDocument.h"
#include "SymbolDirector.h"
#include "CTreeDirector.h"
#include "DTreeDirector.h"
#include "GoTreeDirector.h"
#include "JavaTreeDirector.h"
#include "PHPTreeDirector.h"
#include "Tree.h"
#include "ViewManPageDialog.h"
#include "FindFileDialog.h"
#include "DiffFileDialog.h"
#include "SearchTextDialog.h"
#include "CommandMenu.h"
#include "DocumentMenu.h"
#include "FileHistoryMenu.h"
#include "actionDefs.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXFileListSet.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    New text file                  %k Meta-N       %i" kNewTextFileAction
	"  | New text file from template... %k Meta-Shift-N %i" kNewTextFileFromTmplAction
	"  | New project...                                 %i" kNewProjectAction
	"  | New shell...                                   %i" kNewShellAction
	"%l| Open...                        %k Meta-O       %i" kOpenSomethingAction
	"  | Recent projects"
	"  | Recent text files"
	"%l| Close                          %k Meta-W       %i" kJXCloseWindowAction
	"  | Quit                           %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewTextEditorCmd = 1, kNewTextTemplateCmd, kNewProjectCmd, kNewShellCmd,
	kOpenSomethingCmd,
	kRecentProjectMenuCmd, kRecentTextMenuCmd,
	kCloseCmd, kQuitCmd
};

// List menu

static const JUtf8Byte* kListMenuStr =
	"    Open selected files                 %k Return.     %i" kOpenSelectedFilesAction
	"  | Show selected files in file manager %k Meta-Return %i" kOpenSelectedFileLocationsAction
	"  | Update                              %k Meta-U      %i" kUpdateClassTreeAction
	"%l| Use wildcard filter %b                             %i" kUseWildcardFilterAction
	"  | Use regex filter    %b                             %i" kUseRegexFilterAction;

enum
{
	kOpenSelectionCmd = 1, kShowLocationCmd, kUpdateCmd,
	kUseWildcardCmd, kUseRegexCmd
};

// Project menu

static const JUtf8Byte* kProjectMenuStr =
	"    Show symbol browser %k Ctrl-F12     %i" kShowSymbolBrowserAction
	"  | Show C++ class tree                 %i" kShowCPPClassTreeAction
	"  | Show D class tree                   %i" kShowDClassTreeAction
	"  | Show Go struct/interface tree       %i" kShowGoClassTreeAction
	"  | Show Java class tree                %i" kShowJavaClassTreeAction
	"  | Show PHP class tree                 %i" kShowPHPClassTreeAction
	"  | Look up man page... %k Meta-I       %i" kViewManPageAction
	"%l| Find file...        %k Meta-D       %i" kFindFileAction
	"  | Search files...     %k Meta-F       %i" kSearchFilesAction
	"  | Compare files...                    %i" kDiffFilesAction
	"%l| Save all            %k Meta-Shift-S %i" kSaveAllTextFilesAction
	"  | Close all           %k Meta-Shift-W %i" kCloseAllTextFilesAction;

enum
{
	kShowSymbolBrowserCmd = 1,
	kShowCTreeCmd, kShowDTreeCmd, kShowGoTreeCmd, kShowJavaTreeCmd, kShowPHPTreeCmd,
	kViewManPageCmd,
	kFindFileCmd, kSearchFilesCmd, kDiffFilesCmd,
	kSaveAllTextCmd, kCloseAllTextCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Toolbar buttons..."
	"  | File types..."
	"  | External editors..."
	"  | File manager & web browser..."
	"  | Miscellaneous..."
	"%l| Save window size as default";

enum
{
	kToolBarPrefsCmd = 1,
	kEditFileTypesCmd, kChooseExtEditorsCmd,
	kShowLocationPrefsCmd, kMiscPrefsCmd,
	kSaveWindSizeCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

FileListDirector::FileListDirector
	(
	ProjectDocument* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	FileListDirectorX(supervisor);
}

FileListDirector::FileListDirector
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			setInput,
	const JFileVersion	setVers,
	std::istream*			symInput,
	const JFileVersion	symVers,
	ProjectDocument*	supervisor,
	const bool		subProject
	)
	:
	JXWindowDirector(supervisor)
{
	FileListDirectorX(supervisor);

	if (projVers >= 20)
	{
		const bool useProjData = setInput == nullptr || setVers < 71;
		if (projVers < 71)
		{
			if (useProjData)
			{
				GetWindow()->ReadGeometry(projInput);
			}
			else
			{
				JXWindow::SkipGeometry(projInput);
			}

			bool active = false;
			if (projVers >= 50)
			{
				projInput >> JBoolFromString(active);
			}
			if (useProjData && active && !subProject)
			{
				Activate();
			}

			itsFLSet->ReadSetup(projInput);		// no way to skip
		}

		if (!useProjData)
		{
			GetWindow()->ReadGeometry(*setInput);

			bool active;
			*setInput >> JBoolFromString(active);
			if (active && !subProject)
			{
				Activate();
			}

			itsFLSet->ReadSetup(*setInput);
		}

		itsFLTable->ReadSetup(projInput, projVers, symInput, symVers);
	}
}

// private

void
FileListDirector::FileListDirectorX
	(
	ProjectDocument* projDoc
	)
{
	itsProjDoc = projDoc;
	ListenTo(itsProjDoc, std::function([this](const JXFileDocument::NameChanged&)
	{
		AdjustWindowTitle();
	}));

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileListDirector::~FileListDirector()
{
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
FileListDirector::StreamOut
	(
	std::ostream& projOutput,
	std::ostream* setOutput,
	std::ostream* symOutput
	)
	const
{
	if (setOutput != nullptr)
	{
		*setOutput << ' ';
		GetWindow()->WriteGeometry(*setOutput);
		*setOutput << ' ' << JBoolToString(IsActive());
		*setOutput << ' ';
		itsFLSet->WriteSetup(*setOutput);
		*setOutput << ' ';
	}

	itsFLTable->WriteSetup(projOutput, symOutput);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "jcc_file_list_window.xpm"

#include <jx-af/image/jx/jx_file_new.xpm>
#include <jx-af/image/jx/jx_file_open.xpm>
#include <jx-af/image/jx/jx_filter_wildcard.xpm>
#include <jx-af/image/jx/jx_filter_regex.xpm>
#include "jcc_show_symbol_list.xpm"
#include "jcc_show_c_tree.xpm"
#include "jcc_show_d_tree.xpm"
#include "jcc_show_go_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"
#include "jcc_view_man_page.xpm"
#include "jcc_search_files.xpm"
#include "jcc_compare_files.xpm"
#include <jx-af/image/jx/jx_file_save_all.xpm>

void
FileListDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 340,450, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 340,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kFileListToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 340,420);
	assert( itsToolBar != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetWMClass(GetWMClassInstance(), GetFileListWindowClass());

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, jcc_file_list_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (GetPrefsManager()->GetWindowSize(kFileListWindSizeID, &desktopLoc, &w, &h))
	{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}

	itsFLSet =
		jnew JXFileListSet(itsToolBar->GetWidgetEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsFLSet != nullptr );
	itsFLSet->FitToEnclosure();

	JXScrollbarSet* scrollbarSet = itsFLSet->GetScrollbarSet();
	itsFLTable =
		jnew FileListTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsFLTable != nullptr );
	itsFLSet->SetTable(itsFLTable);

	ListenTo(itsFLTable, std::function([this](const JXFileListTable::ProcessSelection&)
	{
		OpenSelectedFiles();
	}));

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "FileListDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&FileListDirector::UpdateFileMenu,
		&FileListDirector::HandleFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);

	auto* recentProjectMenu =
		jnew FileHistoryMenu(DocumentManager::kProjectFileHistory,
							  itsFileMenu, kRecentProjectMenuCmd, menuBar);
	assert( recentProjectMenu != nullptr );

	auto* recentTextMenu =
		jnew FileHistoryMenu(DocumentManager::kTextFileHistory,
							  itsFileMenu, kRecentTextMenuCmd, menuBar);
	assert( recentTextMenu != nullptr );

	itsListMenu = menuBar->AppendTextMenu(JGetString("ListMenuTitle::FileListDirector"));
	itsListMenu->SetMenuItems(kListMenuStr, "FileListDirector");
	itsListMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsListMenu->AttachHandlers(this,
		&FileListDirector::UpdateListMenu,
		&FileListDirector::HandleListMenu);

	itsListMenu->SetItemImage(kUseWildcardCmd, jx_filter_wildcard);
	itsListMenu->SetItemImage(kUseRegexCmd,    jx_filter_regex);

	itsFLSet->AppendEditMenu(menuBar);

	itsProjectMenu = menuBar->AppendTextMenu(JGetString("ProjectMenuTitle::global"));
	itsProjectMenu->SetMenuItems(kProjectMenuStr, "FileListDirector");
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsProjectMenu->AttachHandlers(this,
		&FileListDirector::UpdateProjectMenu,
		&FileListDirector::HandleProjectMenu);

	itsProjectMenu->SetItemImage(kShowSymbolBrowserCmd, jcc_show_symbol_list);
	itsProjectMenu->SetItemImage(kShowCTreeCmd,         jcc_show_c_tree);
	itsProjectMenu->SetItemImage(kShowDTreeCmd,         jcc_show_d_tree);
	itsProjectMenu->SetItemImage(kShowGoTreeCmd,        jcc_show_go_tree);
	itsProjectMenu->SetItemImage(kShowJavaTreeCmd,      jcc_show_java_tree);
	itsProjectMenu->SetItemImage(kShowPHPTreeCmd,       jcc_show_php_tree);
	itsProjectMenu->SetItemImage(kViewManPageCmd,       jcc_view_man_page);
	itsProjectMenu->SetItemImage(kSearchFilesCmd,       jcc_search_files);
	itsProjectMenu->SetItemImage(kDiffFilesCmd,         jcc_compare_files);
	itsProjectMenu->SetItemImage(kSaveAllTextCmd,       jx_file_save_all);

	itsCmdMenu =
		jnew CommandMenu(itsProjDoc, nullptr, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != nullptr );
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	auto* fileListMenu =
		jnew DocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != nullptr );
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "FileListDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandlers(this,
		&FileListDirector::UpdatePrefsMenu,
		&FileListDirector::HandlePrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "FileListDirector", "FileListHelp");

	// must do this after creating menus

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsListMenu, kUseWildcardCmd);
		itsToolBar->AppendButton(itsListMenu, kUseRegexCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsProjectMenu, kSearchFilesCmd);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 AdjustWindowTitle (private)

 ******************************************************************************/

void
FileListDirector::AdjustWindowTitle()
{
	const JString title = itsProjDoc->GetName() + JGetString("WindowTitleSuffix::FileListDirector");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 OpenSelectedFiles (private)

 ******************************************************************************/

void
FileListDirector::OpenSelectedFiles()
	const
{
	JPtrArray<JString> fileList(JPtrArrayT::kDeleteAll);
	if (itsFLTable->GetSelection(&fileList))
	{
		GetDocumentManager()->OpenSomething(fileList);
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
FileListDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kCloseCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
FileListDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(itsProjDoc);

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
		GetDocumentManager()->OpenSomething();
	}

	else if (index == kCloseCmd)
	{
		GetWindow()->Close();
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 UpdateListMenu (private)

 ******************************************************************************/

void
FileListDirector::UpdateListMenu()
{
	const bool allowOpen =
		itsFLTable->HasFocus() && itsFLTable->HasSelection();
	itsListMenu->SetItemEnabled(kOpenSelectionCmd, allowOpen);
	itsListMenu->SetItemEnabled(kShowLocationCmd,  allowOpen);

	const JXFileListSet::FilterType type = itsFLSet->GetFilterType();
	if (type == JXFileListSet::kWildcardFilter)
	{
		itsListMenu->CheckItem(kUseWildcardCmd);
	}
	else if (type == JXFileListSet::kRegexFilter)
	{
		itsListMenu->CheckItem(kUseRegexCmd);
	}
}

/******************************************************************************
 HandleListMenu (private)

 ******************************************************************************/

void
FileListDirector::HandleListMenu
	(
	const JIndex index
	)
{
	if (index == kOpenSelectionCmd)
	{
		OpenSelectedFiles();
	}
	else if (index == kShowLocationCmd)
	{
		itsFLTable->ShowSelectedFileLocations();
	}
	else if (index == kUpdateCmd)
	{
		itsProjDoc->UpdateSymbolDatabase();
	}

	else if (index == kUseWildcardCmd)
	{
		itsFLSet->ToggleWildcardFilter();
	}
	else if (index == kUseRegexCmd)
	{
		itsFLSet->ToggleRegexFilter();
	}
}

/******************************************************************************
 UpdateProjectMenu (private)

 ******************************************************************************/

void
FileListDirector::UpdateProjectMenu()
{
	itsProjectMenu->SetItemEnabled(kShowCTreeCmd,
		!itsProjDoc->GetCTreeDirector()->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnabled(kShowDTreeCmd,
		!itsProjDoc->GetDTreeDirector()->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnabled(kShowGoTreeCmd,
		!itsProjDoc->GetGoTreeDirector()->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnabled(kShowJavaTreeCmd,
		!itsProjDoc->GetJavaTreeDirector()->GetTree()->IsEmpty());
	itsProjectMenu->SetItemEnabled(kShowPHPTreeCmd,
		!itsProjDoc->GetPHPTreeDirector()->GetTree()->IsEmpty());

	itsProjectMenu->SetItemEnabled(kCloseAllTextCmd,
								  GetDocumentManager()->HasTextDocuments());
	itsProjectMenu->SetItemEnabled(kSaveAllTextCmd,
								  GetDocumentManager()->TextDocumentsNeedSave());
}

/******************************************************************************
 HandleProjectMenu (private)

 ******************************************************************************/

void
FileListDirector::HandleProjectMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(itsProjDoc);

	if (index == kShowSymbolBrowserCmd)
	{
		itsProjDoc->GetSymbolDirector()->Activate();
	}
	else if (index == kShowCTreeCmd)
	{
		itsProjDoc->GetCTreeDirector()->Activate();
	}
	else if (index == kShowDTreeCmd)
	{
		itsProjDoc->GetDTreeDirector()->Activate();
	}
	else if (index == kShowGoTreeCmd)
	{
		itsProjDoc->GetGoTreeDirector()->Activate();
	}
	else if (index == kShowJavaTreeCmd)
	{
		itsProjDoc->GetJavaTreeDirector()->Activate();
	}
	else if (index == kShowPHPTreeCmd)
	{
		itsProjDoc->GetPHPTreeDirector()->Activate();
	}
	else if (index == kViewManPageCmd)
	{
		GetViewManPageDialog()->Activate();
	}

	else if (index == kFindFileCmd)
	{
		GetFindFileDialog()->Activate();
	}
	else if (index == kSearchFilesCmd)
	{
		GetSearchTextDialog()->Activate();
	}
	else if (index == kDiffFilesCmd)
	{
		GetDiffFileDialog()->Activate();
	}

	else if (index == kSaveAllTextCmd)
	{
		GetDocumentManager()->SaveTextDocuments(true);
	}
	else if (index == kCloseAllTextCmd)
	{
		GetDocumentManager()->CloseTextDocuments();
	}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
FileListDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
FileListDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kToolBarPrefsCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kEditFileTypesCmd)
	{
		GetPrefsManager()->EditFileTypes();
	}
	else if (index == kChooseExtEditorsCmd)
	{
		GetDocumentManager()->ChooseEditors();
	}
	else if (index == kShowLocationPrefsCmd)
	{
		(JXGetWebBrowser())->EditPrefs();
	}
	else if (index == kMiscPrefsCmd)
	{
		GetApplication()->EditMiscPrefs();
	}

	else if (index == kSaveWindSizeCmd)
	{
		GetPrefsManager()->SaveWindowSize(kFileListWindSizeID, GetWindow());
	}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
FileListDirector::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsCmdMenu && message->Is(CommandMenu::kGetTargetInfo))
	{
		auto* info = dynamic_cast<CommandMenu::GetTargetInfo*>(message);
		assert( info != nullptr );
		itsFLTable->GetSelection(info->GetFileList());
	}
	else
	{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
	}
}
