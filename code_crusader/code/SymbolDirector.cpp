/******************************************************************************
 SymbolDirector.cpp

	Window to display SymbolList.

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "SymbolDirector.h"
#include "SymbolSRDirector.h"
#include "SymbolTable.h"
#include "SymbolList.h"
#include "SymbolTypeList.h"
#include "EditSymbolPrefsDialog.h"
#include "FileHistoryMenu.h"
#include "ViewManPageDialog.h"
#include "FindFileDialog.h"
#include "DiffFileDialog.h"
#include "SearchTextDialog.h"
#include "ProjectDocument.h"
#include "FileListTable.h"
#include "CTreeDirector.h"
#include "DTreeDirector.h"
#include "GoTreeDirector.h"
#include "JavaTreeDirector.h"
#include "PHPTreeDirector.h"
#include "TreeWidget.h"
#include "Tree.h"
#include "Class.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "FileListDirector.h"
#include "CommandMenu.h"
#include "DocumentMenu.h"
#include "actionDefs.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentPrefsVersion = 0;

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

// Symbol menu

static const JUtf8Byte* kSymbolMenuStr =
	"    Edit search paths...                               %i" kEditSearchPathsAction
	"  | Update                 %k Meta-U                   %i" kUpdateClassTreeAction
	"%l| Open file              %k Left-dbl-click or Return %i" kOpenSelectedFilesAction
	"  | Copy selected names    %k Meta-C                   %i" kCopySymbolNameAction
//	"%l| Find symbol...         %k Meta-F                   %i" kFindFunctionAction
	"%l| Find selected symbols  %k Meta-dbl-click           %i" kFindSelectionInProjectAction
	"  | Close symbol browsers                              %i" kCloseAllSymSRAction;

enum
{
	kEditSearchPathsCmd = 1, kUpdateCurrentCmd,
	kOpenFileCmd, kCopySelNamesCmd,
//	kFindSymbolCmd,
	kFindSelectedSymbolCmd,
	kCloseAllSymSRCmd
};

// Project menu

static const JUtf8Byte* kProjectMenuStr =
	"    Show C++ class tree                 %i" kShowCPPClassTreeAction
	"  | Show D class tree                   %i" kShowDClassTreeAction
	"  | Show Go struct/interface tree       %i" kShowGoClassTreeAction
	"  | Show Java class tree                %i" kShowJavaClassTreeAction
	"  | Show PHP class tree                 %i" kShowPHPClassTreeAction
	"  | Look up man page... %k Meta-I       %i" kViewManPageAction
	"%l| Show file list      %k Meta-Shift-F %i" kShowFileListAction
	"  | Find file...        %k Meta-D       %i" kFindFileAction
	"  | Search files...     %k Meta-F       %i" kSearchFilesAction
	"  | Compare files...                    %i" kDiffFilesAction
	"%l| Save all            %k Meta-Shift-S %i" kSaveAllTextFilesAction
	"  | Close all           %k Meta-Shift-W %i" kCloseAllTextFilesAction;

enum
{
	kShowCTreeCmd = 1, kShowDTreeCmd, kShowGoTreeCmd, kShowJavaTreeCmd, kShowPHPTreeCmd,
	kViewManPageCmd,
	kShowFileListCmd, kFindFileCmd, kSearchFilesCmd, kDiffFilesCmd,
	kSaveAllTextCmd, kCloseAllTextCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Symbols..."
	"  | Toolbar buttons..."
	"  | File types..."
	"  | External editors..."
	"  | Miscellaneous..."
	"%l| Save window size as default";

enum
{
	kSymbolPrefsCmd = 1, kToolBarPrefsCmd,
	kEditFileTypesCmd, kChooseExtEditorsCmd, kMiscPrefsCmd,
	kSaveWindSizeCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

SymbolDirector::SymbolDirector
	(
	ProjectDocument* supervisor
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kSymbolDirectorID)
{
	SymbolDirectorX(supervisor);
}

SymbolDirector::SymbolDirector
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	ProjectDocument*	supervisor,
	const bool			subProject
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kSymbolDirectorID)
{
	SymbolDirectorX(supervisor);

	const bool useProjData = setInput == nullptr || setVers < 71;
	if (!useProjData)
	{
		GetWindow()->ReadGeometry(*setInput);

		// put SR windows on top of main window

		bool active;
		*setInput >> JBoolFromString(active);
		if (active && !subProject)
		{
			Activate();
		}

		// read and create SymbolSRDirectors here
	}

	if (projVers >= 41)
	{
		if (projVers < 71 && useProjData)
		{
			GetWindow()->ReadGeometry(projInput);
		}
		else if (projVers < 71)
		{
			JXWindow::SkipGeometry(projInput);
		}

		if (45 <= projVers && projVers < 74)
		{
			SymbolTypeList::SkipSetup(projInput, projVers);
		}

		itsSymbolList->ReadSetup(projInput, projVers, symInput, symVers);

		bool active = false;
		if (47 <= projVers && projVers < 71)
		{
			projInput >> JBoolFromString(active);
		}
		if (useProjData && active && !subProject)
		{
			Activate();
		}
	}
}

// private

void
SymbolDirector::SymbolDirectorX
	(
	ProjectDocument* projDoc
	)
{
	itsProjDoc = projDoc;
	ListenTo(itsProjDoc);

	itsSymbolList = jnew SymbolList(projDoc);
	assert( itsSymbolList != nullptr );

	itsSRList = jnew JPtrArray<SymbolSRDirector>(JPtrArrayT::kForgetAll);
	assert( itsSRList != nullptr );

	itsRaiseTreeOnRightClickFlag = false;

	BuildWindow(itsSymbolList);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SymbolDirector::~SymbolDirector()
{
	CloseSymbolBrowsers();

	jdelete itsSymbolList;
	jdelete itsSRList;			// objects deleted by JXDirector
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
SymbolDirector::ReadSetup
	(
	std::istream&		symInput,
	const JFileVersion	symVers
	)
{
	CloseSymbolBrowsers();
	itsSymbolList->ReadSetup(symInput, symVers);
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SymbolDirector::StreamOut
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
		*setOutput << ' ' << JBoolToString(IsActive()) << ' ';
	}

	itsSymbolList->WriteSetup(projOutput, symOutput);
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
SymbolDirector::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
	{
		return;
	}

	input >> JBoolFromString(itsRaiseTreeOnRightClickFlag);
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
SymbolDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;
	output << ' ' << JBoolToString(itsRaiseTreeOnRightClickFlag);
}

/******************************************************************************
 CloseSymbolBrowsers

 ******************************************************************************/

void
SymbolDirector::CloseSymbolBrowsers()
{
	const JSize count = itsSRList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsSRList->GetLastElement()->Close();
	}
}

/*****************************************************************************
 SRDirectorClosed

 ******************************************************************************/

void
SymbolDirector::SRDirectorClosed
	(
	SymbolSRDirector* dir
	)
{
	itsSRList->Remove(dir);
}

/******************************************************************************
 FileTypesChanged

 ******************************************************************************/

void
SymbolDirector::FileTypesChanged
	(
	const PrefsManager::FileTypesChanged& info
	)
{
	itsSymbolList->FileTypesChanged(info);
}

/******************************************************************************
 PrepareForListUpdate

 ******************************************************************************/

void
SymbolDirector::PrepareForListUpdate
	(
	const bool		reparseAll,
	JProgressDisplay&	pg
	)
{
	itsSymbolList->PrepareForUpdate(reparseAll, pg);
}

/******************************************************************************
 ListUpdateFinished

 ******************************************************************************/

bool
SymbolDirector::ListUpdateFinished
	(
	const JArray<JFAID_t>&	deadFileList,
	JProgressDisplay&		pg
	)
{
	const bool ok = itsSymbolList->UpdateFinished(deadFileList, pg);
	if (ok && !InUpdateThread())
	{
		CloseSymbolBrowsers();
	}
	return ok;
}

/******************************************************************************
 FindSymbol

	If fileName is not empty, it is used for context.

	If there is a single match, button determines what to open:

		kJXLeftButton:   definition
		kJXMiddleButton: declaration
		kJXRightButton:  both

 ******************************************************************************/

bool
SymbolDirector::FindSymbol
	(
	const JString&		name,
	const JString&		fileName,
	const JXMouseButton	button
	)
{
	JXGetApplication()->DisplayBusyCursor();

	JPtrArray<Tree> treeList(JPtrArrayT::kForgetAll);
	for (auto* director : itsProjDoc->GetTreeDirectorList())
	{
		treeList.Append(director->GetTree());
	}

	JFAID_t contextFileID = JFAID::kInvalidID;
	JArray<SymbolList::ContextNamespace> cnList;
	if (!fileName.IsEmpty())
	{
		itsProjDoc->GetAllFileList()->GetFileID(fileName, &contextFileID);

		JString path, name1, className, suffix;
		JSplitPathAndName(fileName, &path, &name1);
		JSplitRootAndSuffix(name1, &className, &suffix);

		const Class* theClass;
		for (auto* tree : treeList)
		{
			if (tree->IsUniqueClassName(className, &theClass))
			{
				auto* list = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
				assert( list != nullptr );

				theClass->GetAncestorList(list);
				if (!list->IsEmpty())
				{
					cnList.AppendElement(
						SymbolList::ContextNamespace(tree->GetLanguage(), list));
				}
				else
				{
					jdelete list;
				}
			}
		}

		Language lang;
		if (cnList.IsEmpty() &&
			itsSymbolList->IsUniqueClassName(className, &lang))
		{
			auto* list = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
			assert( list != nullptr );
			list->Append(className);

			cnList.AppendElement(SymbolList::ContextNamespace(lang, list));
		}
	}

	JArray<JIndex> symbolList;
	const bool foundSymbol =
		itsSymbolList->FindSymbol(name, contextFileID, cnList,
			button == kJXMiddleButton || button == kJXRightButton,
			button == kJXLeftButton   || button == kJXRightButton,
			&symbolList);

	for (auto cns : cnList)
	{
		cns.CleanOut();
	}
	cnList.RemoveAll();

	const bool raiseTree =
		!foundSymbol || (button == kJXRightButton && itsRaiseTreeOnRightClickFlag);

	bool foundInAnyTree = false;
	for (auto* director : itsProjDoc->GetTreeDirectorList())
	{
		auto* treeWidget = director->GetTreeWidget();
		foundInAnyTree   = treeWidget->FindClass(name, button, raiseTree, false, !foundSymbol, true) || foundInAnyTree;
		foundInAnyTree   = treeWidget->FindFunction(symbolList, button, raiseTree, false, !foundSymbol, false) || foundInAnyTree;
	}

	if (symbolList.GetElementCount() == 1 && button != kJXRightButton)
	{
		const JIndex symbolIndex = symbolList.GetFirstElement();

		Language lang;
		SymbolList::Type type;
		itsSymbolList->GetSymbol(symbolIndex, &lang, &type);

		JIndex lineIndex;
		const JString& fileName1 =
			itsSymbolList->GetFile(symbolIndex, &lineIndex);

		TextDocument* doc = nullptr;
		if (GetDocumentManager()->OpenTextDocument(fileName1, lineIndex, &doc) &&
			SymbolList::ShouldSmartScroll(type))
		{
			doc->GetTextEditor()->ScrollForDefinition(lang);
		}
		return true;
	}
	else if (foundSymbol)
	{
		auto* dir =
			jnew SymbolSRDirector(this, itsProjDoc, itsSymbolList,
									symbolList, name);
		assert( dir != nullptr );
		dir->Activate();
		itsSRList->Append(dir);
		return true;
	}
	else
	{
		return foundInAnyTree;
	}
}

/******************************************************************************
 FindAllSymbols

	Displays all the symbols for the class and its ancestors.

 ******************************************************************************/

bool
SymbolDirector::FindAllSymbols
	(
	const Class*	theClass,
	const bool		findDeclaration,
	const bool		findDefinition
	)
{
	JArray<JIndex> symbolList;
	if (itsSymbolList->FindAllSymbols(theClass, findDeclaration, findDefinition, &symbolList))
	{
		auto* dir =
			jnew SymbolSRDirector(this, itsProjDoc, itsSymbolList,
									symbolList, theClass->GetFullName());
		assert( dir != nullptr );
		dir->Activate();
		itsSRList->Append(dir);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "jcc_symbol_window.xpm"

#include <jx-af/image/jx/jx_file_new.xpm>
#include <jx-af/image/jx/jx_file_open.xpm>
#include "jcc_show_c_tree.xpm"
#include "jcc_show_d_tree.xpm"
#include "jcc_show_go_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"
#include "jcc_view_man_page.xpm"
#include "jcc_show_file_list.xpm"
#include "jcc_search_files.xpm"
#include "jcc_compare_files.xpm"
#include <jx-af/image/jx/jx_file_save_all.xpm>

void
SymbolDirector::BuildWindow
	(
	SymbolList* symbolList
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,430, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kSymbolToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,400);
	assert( itsToolBar != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetMinSize(150, 150);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetWMClass(GetWMClassInstance(), GetSymbolWindowClass());

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	itsSymbolTable =
		jnew SymbolTable(this, symbolList,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsSymbolTable != nullptr );
	itsSymbolTable->FitToEnclosure();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, jcc_symbol_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (GetPrefsManager()->GetWindowSize(kSymbolWindSizeID, &desktopLoc, &w, &h))
	{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "SymbolDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

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

	itsSymbolMenu = menuBar->AppendTextMenu(JGetString("SymbolMenuTitle::SymbolDirector"));
	itsSymbolMenu->SetMenuItems(kSymbolMenuStr, "SymbolDirector");
	ListenTo(itsSymbolMenu);

	itsProjectMenu = menuBar->AppendTextMenu(JGetString("ProjectMenuTitle::global"));
	itsProjectMenu->SetMenuItems(kProjectMenuStr, "SymbolDirector");
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsProjectMenu);

	itsProjectMenu->SetItemImage(kShowCTreeCmd,    jcc_show_c_tree);
	itsProjectMenu->SetItemImage(kShowDTreeCmd,    jcc_show_d_tree);
	itsProjectMenu->SetItemImage(kShowGoTreeCmd,   jcc_show_go_tree);
	itsProjectMenu->SetItemImage(kShowJavaTreeCmd, jcc_show_java_tree);
	itsProjectMenu->SetItemImage(kShowPHPTreeCmd,  jcc_show_php_tree);
	itsProjectMenu->SetItemImage(kViewManPageCmd,  jcc_view_man_page);
	itsProjectMenu->SetItemImage(kShowFileListCmd, jcc_show_file_list);
	itsProjectMenu->SetItemImage(kSearchFilesCmd,  jcc_search_files);
	itsProjectMenu->SetItemImage(kDiffFilesCmd,    jcc_compare_files);
	itsProjectMenu->SetItemImage(kSaveAllTextCmd,  jx_file_save_all);

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
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "SymbolDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = GetApplication()->CreateHelpMenu(menuBar, "SymbolDirector");
	ListenTo(itsHelpMenu);

	// must do this after creating menus

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsSymbolMenu, kEditSearchPathsCmd);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, itsHelpMenu);
	}
}

/******************************************************************************
 AdjustWindowTitle (private)

 ******************************************************************************/

void
SymbolDirector::AdjustWindowTitle()
{
	const JString title = itsProjDoc->GetName() + JGetString("WindowTitleSuffix::SymbolDirector");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SymbolDirector::Receive
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

	else if (sender == itsSymbolMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateSymbolMenu();
	}
	else if (sender == itsSymbolMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSymbolMenu(selection->GetIndex());
	}

	else if (sender == itsProjectMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateProjectMenu();
	}
	else if (sender == itsProjectMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleProjectMenu(selection->GetIndex());
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

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		GetApplication()->UpdateHelpMenu(itsHelpMenu);
	}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		GetApplication()->HandleHelpMenu(itsHelpMenu, "SymbolHelp",
											 selection->GetIndex());
	}

	else if (sender == itsProjDoc && message.Is(JXFileDocument::kNameChanged))
	{
		AdjustWindowTitle();
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
SymbolDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kCloseCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
SymbolDirector::HandleFileMenu
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
 UpdateSymbolMenu (private)

 ******************************************************************************/

void
SymbolDirector::UpdateSymbolMenu()
{
	itsSymbolMenu->EnableItem(kEditSearchPathsCmd);
	itsSymbolMenu->EnableItem(kUpdateCurrentCmd);

	if (!itsSymbolList->IsEmpty())
	{
//		itsSymbolMenu->EnableItem(kFindSymbolCmd);
	}

	if (itsSymbolTable->HasSelection())
	{
		itsSymbolMenu->EnableItem(kOpenFileCmd);
		itsSymbolMenu->EnableItem(kCopySelNamesCmd);
		itsSymbolMenu->EnableItem(kFindSelectedSymbolCmd);
	}

	if (HasSymbolBrowsers())
	{
		itsSymbolMenu->EnableItem(kCloseAllSymSRCmd);
	}
}

/******************************************************************************
 HandleSymbolMenu (private)

 ******************************************************************************/

void
SymbolDirector::HandleSymbolMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(itsProjDoc);

	if (index == kEditSearchPathsCmd)
	{
		itsProjDoc->EditSearchPaths();
	}
	else if (index == kUpdateCurrentCmd)
	{
		itsProjDoc->UpdateSymbolDatabase();
	}

	else if (index == kOpenFileCmd)
	{
		itsSymbolTable->DisplaySelectedSymbols();
	}
	else if (index == kCopySelNamesCmd)
	{
		itsSymbolTable->CopySelectedSymbolNames();
	}

//	else if (index == kFindSymbolCmd)
//		{
//		}
	else if (index == kFindSelectedSymbolCmd)
	{
		itsSymbolTable->FindSelectedSymbols(kJXRightButton);
	}
	else if (index == kCloseAllSymSRCmd)
	{
		CloseSymbolBrowsers();
	}
}

/******************************************************************************
 UpdateProjectMenu (private)

 ******************************************************************************/

void
SymbolDirector::UpdateProjectMenu()
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
SymbolDirector::HandleProjectMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(itsProjDoc);

	if (index == kShowCTreeCmd)
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

	else if (index == kShowFileListCmd)
	{
		itsProjDoc->GetFileListDirector()->Activate();
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
SymbolDirector::UpdatePrefsMenu()
{
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
SymbolDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kSymbolPrefsCmd)
	{
		EditPrefs();
	}
	else if (index == kToolBarPrefsCmd)
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
	else if (index == kMiscPrefsCmd)
	{
		GetApplication()->EditMiscPrefs();
	}

	else if (index == kSaveWindSizeCmd)
	{
		GetPrefsManager()->SaveWindowSize(kSymbolWindSizeID, GetWindow());
	}
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
SymbolDirector::EditPrefs()
{
	auto* dlog = jnew EditSymbolPrefsDialog(itsRaiseTreeOnRightClickFlag);
	assert( dlog != nullptr );

	if (dlog->DoDialog())
	{
		dlog->UpdateSettings();
	}
}

/******************************************************************************
 SetPrefs

 ******************************************************************************/

void
SymbolDirector::SetPrefs
	(
	const bool raiseTreeOnRightClick,
	const bool writePrefs
	)
{
	itsRaiseTreeOnRightClickFlag = raiseTreeOnRightClick;

	if (writePrefs)
	{
		JPrefObject::WritePrefs();
	}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
SymbolDirector::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsCmdMenu && message->Is(CommandMenu::kGetTargetInfo))
	{
		auto* info =
			dynamic_cast<CommandMenu::GetTargetInfo*>(message);
		assert( info != nullptr );
		itsSymbolTable->GetFileNamesForSelection(info->GetFileList(),
												 info->GetLineIndexList());
	}
	else
	{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
	}
}
