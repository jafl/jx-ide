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

		bool active;
		*setInput >> JBoolFromString(active);
		if (active && !subProject)
		{
			Activate();
		}
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
	ListenTo(itsProjDoc, std::function([this](const JXFileDocument::NameChanged&)
	{
		AdjustWindowTitle();
	}));

	itsSymbolList = jnew SymbolList(projDoc);

	ListenTo(itsSymbolList, std::function([this](const SymbolList::UpdateFoundChanges&)
	{
		itsSymbolMenu->Deactivate();
		itsSymbolTable->Hide();
		CloseSymbolBrowsers();
	}));

	ListenTo(itsSymbolList, std::function([this](const class SymbolList::UpdateDone&)
	{
		itsSymbolMenu->Activate();
		itsSymbolTable->Show();
	}));

	itsSRList = jnew JPtrArray<SymbolSRDirector>(JPtrArrayT::kForgetAll);

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
	const JSize count = itsSRList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsSRList->GetLastItem()->Close();
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
	const bool			reparseAll,
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
	return itsSymbolList->UpdateFinished(deadFileList, pg);
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
					cnList.AppendItem(
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

			cnList.AppendItem(SymbolList::ContextNamespace(lang, list));
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

	if (symbolList.GetItemCount() == 1 && button != kJXRightButton)
	{
		ViewSymbol(symbolList.GetFirstItem());
		return true;
	}
	else if (foundSymbol)
	{
		auto* dir =
			jnew SymbolSRDirector(this, itsProjDoc, itsSymbolList,
									symbolList, name);
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
 ViewSymbol

	Displays all the symbols for the class and its ancestors.

 ******************************************************************************/

bool
SymbolDirector::ViewSymbol
	(
	const JIndex symbolIndex
	)
	const
{
	JIndex lineIndex;
	const JString& fileName = itsSymbolList->GetFile(symbolIndex, &lineIndex);

	TextDocument* doc = nullptr;
	if (GetDocumentManager()->OpenTextDocument(fileName, lineIndex, &doc))
	{
		Language lang;
		SymbolList::Type type;
		itsSymbolList->GetSymbol(symbolIndex, &lang, &type);

		if (SymbolList::ShouldSmartScroll(type))
		{
			doc->GetTextEditor()->ScrollForDefinition(lang);
		}
		return true;
	}
	else
	{
		return false;
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
#include "Generic-File.h"
#include "SymbolDirector-Symbol.h"
#include "SymbolDirector-Project.h"
#include "SymbolDirector-Preferences.h"

void
SymbolDirector::BuildWindow
	(
	SymbolList* symbolList
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,430, JString::empty);
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Crusader_Symbol_List");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kSymbolToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,400);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 400,400);
	assert( scrollbarSet != nullptr );

	itsSymbolTable =
		jnew SymbolTable(this, symbolList, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 400,400);

// end JXLayout

	AdjustWindowTitle();
	window->SetCloseAction(JXWindow::kDeactivateDirector);

	JXDisplay* display = GetDisplay();
	auto* icon         = jnew JXImage(display, jcc_symbol_window);
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (GetPrefsManager()->GetWindowSize(kSymbolWindSizeID, &desktopLoc, &w, &h))
	{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::Generic_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&SymbolDirector::UpdateFileMenu,
		&SymbolDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	jnew FileHistoryMenu(DocumentManager::kProjectFileHistory,
						  itsFileMenu, kRecentProjectMenuCmd, menuBar);

	jnew FileHistoryMenu(DocumentManager::kTextFileHistory,
						  itsFileMenu, kRecentTextMenuCmd, menuBar);

	itsSymbolMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::SymbolDirector_Symbol"));
	itsSymbolMenu->SetMenuItems(kSymbolMenuStr);
	itsSymbolMenu->AttachHandlers(this,
		&SymbolDirector::UpdateSymbolMenu,
		&SymbolDirector::HandleSymbolMenu);
	ConfigureSymbolMenu(itsSymbolMenu);

	itsProjectMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::SymbolDirector_Project"));
	itsProjectMenu->SetMenuItems(kProjectMenuStr);
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsProjectMenu->AttachHandlers(this,
		&SymbolDirector::UpdateProjectMenu,
		&SymbolDirector::HandleProjectMenu);
	ConfigureProjectMenu(itsProjectMenu);

	itsCmdMenu =
		jnew CommandMenu(itsProjDoc, nullptr, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	auto* fileListMenu =
		jnew DocumentMenu(menuBar, JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != nullptr );
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::SymbolDirector_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandlers(this,
		&SymbolDirector::UpdatePrefsMenu,
		&SymbolDirector::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "SymbolHelp");

	// must do this after creating menus

	auto f = std::function(UpgradeToolBarID);
	itsToolBar->LoadPrefs(&f);
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsSymbolMenu, kEditSearchPathsCmd);

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
SymbolDirector::UpgradeToolBarID
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
SymbolDirector::AdjustWindowTitle()
{
	const JString title = itsProjDoc->GetName() + JGetString("WindowTitleSuffix::SymbolDirector");
	GetWindow()->SetTitle(title);
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
