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
#include "Generic-File.h"
#include "FileListDirector-List.h"
#include "FileListDirector-Project.h"
#include "FileListDirector-Preferences.h"

void
FileListDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 340,450, JString::empty);
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Crusader_File_List");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 340,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kFileListToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 340,420);

	itsFLSet =
		jnew JXFileListSet(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 340,420);

// end JXLayout

	AdjustWindowTitle();
	window->ShouldFocusWhenShow(true);
	window->SetCloseAction(JXWindow::kDeactivateDirector);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, jcc_file_list_window);
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (GetPrefsManager()->GetWindowSize(kFileListWindSizeID, &desktopLoc, &w, &h))
	{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}

	JXScrollbarSet* scrollbarSet = itsFLSet->GetScrollbarSet();
	itsFLTable =
		jnew FileListTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	itsFLSet->SetTable(itsFLTable);

	ListenTo(itsFLTable, std::function([this](const JXFileListTable::ProcessSelection&)
	{
		OpenSelectedFiles();
	}));

	ListenTo(itsFLTable, std::function([this](const FileListTable::UpdateFoundChanges&)
	{
		itsFLTable->ClearSelection();
		SetActiveDuringUpdate(false);
	}));

	ListenTo(itsFLTable, std::function([this](const FileListTable::UpdateDone&)
	{
		SetActiveDuringUpdate(true);
	}));

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::Generic_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&FileListDirector::UpdateFileMenu,
		&FileListDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	jnew FileHistoryMenu(DocumentManager::kProjectFileHistory,
						  itsFileMenu, kRecentProjectMenuCmd, menuBar);

	jnew FileHistoryMenu(DocumentManager::kTextFileHistory,
						  itsFileMenu, kRecentTextMenuCmd, menuBar);

	itsListMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::FileListDirector_List"));
	itsListMenu->SetMenuItems(kListMenuStr);
	itsListMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsListMenu->AttachHandlers(this,
		&FileListDirector::UpdateListMenu,
		&FileListDirector::HandleListMenu);
	ConfigureListMenu(itsListMenu);

	itsFLSet->AppendEditMenu(menuBar);

	itsProjectMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::FileListDirector_Project"));
	itsProjectMenu->SetMenuItems(kProjectMenuStr);
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsProjectMenu->AttachHandlers(this,
		&FileListDirector::UpdateProjectMenu,
		&FileListDirector::HandleProjectMenu);
	ConfigureProjectMenu(itsProjectMenu);

	itsCmdMenu =
		jnew CommandMenu(itsProjDoc, nullptr, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	auto* fileListMenu =
		jnew DocumentMenu(menuBar, JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::FileListDirector_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandlers(this,
		&FileListDirector::UpdatePrefsMenu,
		&FileListDirector::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "FileListHelp");

	// must do this after creating menus

	auto f = std::function(UpgradeToolBarID);
	itsToolBar->LoadPrefs(&f);
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
 UpgradeToolBarID (static private)

 ******************************************************************************/

static const JUtf8Byte* kToolbarIDMap[] =
{
	// File

	"CBNewTextFile",			"NewTextFile::Generic",
	"CBNewTextFileFromTmpl",	"NewTextFileFromTmpl::Generic",
	"CBNewProject",				"NewProject::Generic",
	"CBOpenSomething",			"OpenSomething::Generic",
};

const JSize kToolbarIDMapCount = sizeof(kToolbarIDMap) / sizeof(JUtf8Byte*);

void
FileListDirector::UpgradeToolBarID
	(
	JString* s
	)
{
	if (!s->StartsWith("CB") || CommandMenu::UpgradeToolBarID(s))
	{
		return;
	}

	for (JUnsignedOffset i=0; i<kToolbarIDMapCount; i+=2)
	{
		if (*s == kToolbarIDMap[i])
		{
			*s = kToolbarIDMap[i+1];
			return;
		}
	}

	JStringIterator iter(s);
	iter.SkipNext(2);
	iter.RemoveAllPrev();
	*s += "::FileListDirector";
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
 SetActiveDuringUpdate (private)

 ******************************************************************************/

void
FileListDirector::SetActiveDuringUpdate
	(
	const bool active
	)
{
	itsListMenu->SetActive(active);

	JXTEBase* te;
	JXTextMenu* menu;
	if (itsFLTable->GetEditMenuProvider(&te) &&
		te->GetEditMenu(&menu))
	{
		menu->SetActive(active);
	}

	itsFLSet->SetVisible(active);
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
		auto& info = dynamic_cast<CommandMenu::GetTargetInfo&>(*message);
		itsFLTable->GetSelection(info.GetFileList());
	}
	else
	{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
	}
}
