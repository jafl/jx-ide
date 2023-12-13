/******************************************************************************
 TreeDirector.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright © 1996-99 by John Lindal.

 ******************************************************************************/

#include "TreeDirector.h"
#include "TreeWidget.h"
#include "Tree.h"
#include "Class.h"
#include "FileHistoryMenu.h"
#include "EditTreePrefsDialog.h"
#include "ViewManPageDialog.h"
#include "FindFileDialog.h"
#include "DiffFileDialog.h"
#include "SearchTextDialog.h"
#include "EditCPPMacroDialog.h"
#include "ProjectDocument.h"
#include "SymbolDirector.h"
#include "FileListDirector.h"
#include "CommandMenu.h"
#include "DocumentMenu.h"
#include "globals.h"
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/JXEPSPrinter.h>
#include <jx-af/jx/JXGetStringDialog.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/jAssert.h>

// setup information

const JFileVersion kCurrentPrefsVersion = 3;

	// version 1 saves autoMinMILinks, drawMILinksOnTop
	// version 2 saves TreeWidget::itsRaiseWhenSingleMatchFlag
	// version 3 removed itsShowInheritedFnsFlag

/******************************************************************************
 Constructor

	In the 2nd version, d1 and d2 can be nullptr.

 ******************************************************************************/

TreeDirector::TreeDirector
	(
	ProjectDocument*	supervisor,
	TreeCreateFn		createTreeFn,
	const JUtf8Byte*	windowTitleSuffixID,
	const JUtf8Byte*	windowHelpName,
	const JXPM&			windowIcon,
	const JUtf8Byte*	treeMenuItems,
	const JIndex		toolBarPrefID,
	TreeInitToolBarFn	initToolBarFn
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kTreeSetupID),
	itsWindowTitleSuffix( JGetString(windowTitleSuffixID) ),
	itsWindowHelpName( windowHelpName )
{
	JXScrollbarSet* scrollbarSet =
		TreeDirectorX(supervisor, windowIcon, treeMenuItems,
					  toolBarPrefID, initToolBarFn);

	itsTree = createTreeFn(this, TreeWidget::kBorderWidth);

	itsTreeWidget =
		jnew TreeWidget(this, itsTree, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 100,100);
	assert( itsTreeWidget != nullptr );
	itsTreeWidget->FitToEnclosure();

	JPrefObject::ReadPrefs();
}

static void skipFnListDirector
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	JSize fnListCount;
	input >> fnListCount;

	JString className;
	bool showInheritedFns;
	for (JIndex i=1; i<=fnListCount; i++)
	{
		JXWindow::SkipGeometry(input);
		input >> className >> JBoolFromString(showInheritedFns);
		if (vers >= 24)
		{
			JXScrollableWidget::SkipScrollSetup(input);
		}
	}
}

TreeDirector::TreeDirector
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symStream,
	const JFileVersion	origSymVers,
	ProjectDocument*	supervisor,
	const bool			subProject,
	TreeStreamInFn		streamInTreeFn,
	const JUtf8Byte*	windowTitleSuffixID,
	const JUtf8Byte*	windowHelpName,
	const JXPM&			windowIcon,
	const JUtf8Byte*	treeMenuItems,
	const JIndex		toolBarPrefID,
	TreeInitToolBarFn	initToolBarFn,
	DirList*			dirList,
	const bool			readCompileRunDialogs
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kTreeSetupID),
	itsWindowTitleSuffix( JGetString(windowTitleSuffixID) ),
	itsWindowHelpName( windowHelpName )
{
	JXScrollbarSet* scrollbarSet =
		TreeDirectorX(supervisor, windowIcon, treeMenuItems,
					  toolBarPrefID, initToolBarFn);

	itsTree = streamInTreeFn(projInput, projVers,
							 setInput, setVers, symStream, origSymVers,
							 this, TreeWidget::kBorderWidth, dirList);

	std::istream* symInput             = (projVers <= 41 ? &projInput : symStream);
	const JFileVersion symVers    = (projVers <= 41 ? projVers   : origSymVers);
	const bool useSetProjData = setInput == nullptr || setVers < 71;

/* settings file */

	if (!useSetProjData)
	{
		GetWindow()->ReadGeometry(*setInput);
	}
	if (24 <= projVers && projVers < 71 && useSetProjData)
	{
		GetWindow()->ReadGeometry(projInput);
	}
	else if (24 <= projVers && projVers < 71)
	{
		JXWindow::SkipGeometry(projInput);
	}

	itsTreeWidget =
		jnew TreeWidget(this, itsTree, scrollbarSet,
						 scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 100,100);
	assert( itsTreeWidget != nullptr );
	itsTreeWidget->FitToEnclosure();

	if (projVers < 71)
	{
		itsTreeWidget->ReadSetup(projInput, projVers);
	}
	if (!useSetProjData)	// overwrite
	{
		itsTreeWidget->ReadSetup(*setInput, setVers);
	}

	JPrefObject::ReadPrefs();		// set tree parameters before updating

	if (readCompileRunDialogs)
	{
		supervisor->ConvertCompileRunDialogs(projInput, projVers);
	}

	if (17 <= projVers && projVers < 71)
	{
		itsPSPrinter->ReadXPSSetup(projInput);
		itsEPSPrinter->ReadXEPSSetup(projInput);
		itsFnListPrinter->ReadXPSSetup(projInput);
	}
	if (!useSetProjData)	// overwrite
	{
		itsPSPrinter->ReadXPSSetup(*setInput);
		itsEPSPrinter->ReadXEPSSetup(*setInput);
		itsFnListPrinter->ReadXPSSetup(*setInput);
	}

	// put fn list windows on top of tree window

	bool active = false;
	if (31 <= projVers && projVers < 71)
	{
		projInput >> JBoolFromString(active);
	}
	if (!useSetProjData)	// overwrite
	{
		*setInput >> JBoolFromString(active);
	}
	if (active && !subProject)
	{
		Activate();
	}

/* symbol file */

	if (17 <= projVers && projVers < 71)
	{
		skipFnListDirector(projInput, projVers);
	}
	else if (symInput != nullptr && 71 <= symVers && symVers < 88)
	{
		skipFnListDirector(*symInput, symVers);
	}
}

// private

JXScrollbarSet*
TreeDirector::TreeDirectorX
	(
	ProjectDocument*	doc,
	const JXPM&			windowIcon,
	const JUtf8Byte*	treeMenuItems,
	const JIndex		toolBarPrefID,
	TreeInitToolBarFn	initToolBarFn
	)
{
	itsProjDoc = doc;
	ListenTo(itsProjDoc, std::function([this](const JXFileDocument::NameChanged&)
	{
		AdjustWindowTitle();
	}));

	JXScrollbarSet* sbarSet = BuildWindow(windowIcon, treeMenuItems,
										  toolBarPrefID, initToolBarFn);

	// can't call GetWindow() until window is created

	itsPSPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsPSPrinter != nullptr );

	itsEPSPrinter = jnew JXEPSPrinter(GetDisplay());
	assert( itsEPSPrinter != nullptr );

	itsFnListPrinter = jnew JXPSPrinter(GetDisplay());
	assert( itsFnListPrinter != nullptr );

	return sbarSet;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TreeDirector::~TreeDirector()
{
	jdelete itsTree;
	jdelete itsPSPrinter;
	jdelete itsEPSPrinter;
	jdelete itsFnListPrinter;
}

/******************************************************************************
 ReloadSetup (virtual)

 ******************************************************************************/

void
TreeDirector::ReloadSetup
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	itsTree->ReloadSetup(input, vers);
}

/******************************************************************************
 StreamOut (virtual)

	dirList, d1, and d2 can be nullptr

 ******************************************************************************/

void
TreeDirector::StreamOut
	(
	std::ostream&		projOutput,
	std::ostream*		setOutput,
	std::ostream*		symOutput,
	const DirList*	dirList
	)
	const
{
	itsTree->StreamOut(projOutput, setOutput, symOutput, dirList);

/* settings file */

	if (setOutput != nullptr)
	{
		*setOutput << ' ';
		GetWindow()->WriteGeometry(*setOutput);

		*setOutput << ' ';
		itsTreeWidget->WriteSetup(*setOutput);

		*setOutput << ' ';
		itsPSPrinter->WriteXPSSetup(*setOutput);

		*setOutput << ' ';
		itsEPSPrinter->WriteXEPSSetup(*setOutput);

		*setOutput << ' ';
		itsFnListPrinter->WriteXPSSetup(*setOutput);

		*setOutput << ' ' << JBoolToString(IsActive());

		*setOutput << ' ';
	}
}

/******************************************************************************
 FileTypesChanged

 ******************************************************************************/

void
TreeDirector::FileTypesChanged
	(
	const PrefsManager::FileTypesChanged& info
	)
{
	itsTree->FileTypesChanged(info);
}

/******************************************************************************
 PrepareForTreeUpdate

 ******************************************************************************/

void
TreeDirector::PrepareForTreeUpdate
	(
	const bool reparseAll
	)
{
	itsTree->PrepareForUpdate(reparseAll);
}

/******************************************************************************
 TreeUpdateFinished

 ******************************************************************************/

bool
TreeDirector::TreeUpdateFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	return itsTree->UpdateFinished(deadFileList);
}

/******************************************************************************
 FindFunction

 ******************************************************************************/

void
TreeDirector::FindFunction()
{
	auto* dlog =
		jnew JXGetStringDialog(JGetString("FindFunctionTitle::TreeDirector"),
							   JGetString("FindFunctionPrompt::TreeDirector"));
	assert( dlog != nullptr );

	if (dlog->DoDialog())
	{
		itsTreeWidget->FindFunction(dlog->GetString(), kJXLeftButton);
	}
}

/******************************************************************************
 ViewFunctionList

 ******************************************************************************/

void
TreeDirector::ViewFunctionList
	(
	const Class* theClass
	)
{
	itsProjDoc->GetSymbolDirector()->FindAllSymbols(theClass, false, true);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "TreeDirector-File.h"
#include "TreeDirector-Project.h"
#include "TreeDirector-Preferences.h"

JXScrollbarSet*
TreeDirector::BuildWindow
	(
	const JXPM&			windowIcon,
	const JUtf8Byte*	treeMenuItems,
	const JIndex		toolBarPrefID,
	TreeInitToolBarFn	initToolBarFn
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,430, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), toolBarPrefID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,400);
	assert( itsToolBar != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetMinSize(150, 150);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetWMClass(GetWMClassInstance(), GetTreeWindowClass());

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, windowIcon);
	window->SetIcon(icon);

	JPoint desktopLoc;
	JCoordinate w,h;
	if (GetPrefsManager()->GetWindowSize(kTreeWindSizeID, &desktopLoc, &w, &h))
	{
		window->Place(desktopLoc.x, desktopLoc.y);
		window->SetSize(w,h);
	}

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::TreeDirector_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&TreeDirector::UpdateFileMenu,
		&TreeDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsFileMenu->SetItemImage(kNewTextEditorCmd, jx_file_new);
	itsFileMenu->SetItemImage(kOpenSomethingCmd, jx_file_open);
	itsFileMenu->SetItemImage(kPrintPSCmd,       jx_file_print);

	jnew FileHistoryMenu(DocumentManager::kProjectFileHistory,
						  itsFileMenu, kRecentProjectMenuCmd, menuBar);

	jnew FileHistoryMenu(DocumentManager::kTextFileHistory,
						  itsFileMenu, kRecentTextMenuCmd, menuBar);

	itsTreeMenu = menuBar->AppendTextMenu(JGetString("TreeMenuTitle::TreeDirector"));
	itsTreeMenu->SetMenuItems(treeMenuItems);
	itsTreeMenu->AttachHandlers(this,
		&TreeDirector::UpdateTreeMenu,
		&TreeDirector::HandleTreeMenu);

	itsProjectMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::TreeDirector_Project"));
	itsProjectMenu->SetMenuItems(kProjectMenuStr);
	itsProjectMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsProjectMenu->AttachHandlers(this,
		&TreeDirector::UpdateProjectMenu,
		&TreeDirector::HandleProjectMenu);
	ConfigureProjectMenu(itsProjectMenu);

	itsProjectMenu->SetItemImage(kViewManPageCmd,       jcc_view_man_page);
	itsProjectMenu->SetItemImage(kShowSymbolBrowserCmd, jcc_show_symbol_list);
	itsProjectMenu->SetItemImage(kShowFileListCmd,      jcc_show_file_list);
	itsProjectMenu->SetItemImage(kSearchFilesCmd,       jcc_search_files);
	itsProjectMenu->SetItemImage(kDiffFilesCmd,         jcc_compare_files);
	itsProjectMenu->SetItemImage(kSaveAllTextCmd,       jx_file_save_all);

	itsCmdMenu =
		jnew CommandMenu(itsProjDoc, nullptr, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);

	auto* fileListMenu =
		jnew DocumentMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != nullptr );
	menuBar->AppendMenu(fileListMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::TreeDirector_Preferences"));
	itsPrefsMenu->SetMenuItems(kPreferencesMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &TreeDirector::HandlePrefsMenu);
	ConfigurePreferencesMenu(itsPrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, itsWindowHelpName);

	// must do this after creating menus

	auto f = std::function([this](JString* s)
	{
		if (!CommandMenu::UpgradeToolBarID(s) &&
			!UpgradeTreeMenuToolBarID(s) &&
			s->StartsWith("CB"))
		{
			JStringIterator iter(s);
			iter.SkipNext(2);
			iter.RemoveAllPrev();
			*s += "::TreeDirector";
		}
	});

	itsToolBar->LoadPrefs(&f);
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kNewTextEditorCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenSomethingCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kPrintPSCmd);
		itsToolBar->NewGroup();

		initToolBarFn(itsToolBar, itsTreeMenu);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}

	return scrollbarSet;
}

/******************************************************************************
 UpgradeTreeMenuToolBarID (virtual protected)

 ******************************************************************************/

bool
TreeDirector::UpgradeTreeMenuToolBarID
	(
	JString* s
	)
{
	if (s->StartsWith("CB"))
	{
		JStringIterator iter(s);
		iter.SkipNext(2);
		iter.RemoveAllPrev();
		*s += "::TreeDirector";
		return true;
	}

	return false;
}

/******************************************************************************
 AdjustWindowTitle (private)

 ******************************************************************************/

void
TreeDirector::AdjustWindowTitle()
{
	const JString title = itsProjDoc->GetName() + itsWindowTitleSuffix;
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
TreeDirector::UpdateFileMenu()
{
	const bool canPrint = !itsTree->IsEmpty();
	itsFileMenu->SetItemEnabled(kPrintPSCmd,  canPrint);
	itsFileMenu->SetItemEnabled(kPrintEPSCmd, canPrint);

	itsFileMenu->SetItemEnabled(kCloseCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
TreeDirector::HandleFileMenu
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

	else if (index == kPSPageSetupCmd)
	{
		itsPSPrinter->EditUserPageSetup();
	}
	else if (index == kPrintPSCmd)
	{
		if (itsPSPrinter->ConfirmUserPrintSetup())
		{
			itsTreeWidget->Print(*itsPSPrinter);
		}
	}
	else if (index == kPrintEPSCmd)
	{
		if (itsEPSPrinter->ConfirmUserPrintSetup())
		{
			itsTreeWidget->Print(*itsEPSPrinter);
		}
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
 UpdateProjectMenu (private)

 ******************************************************************************/

void
TreeDirector::UpdateProjectMenu()
{
	itsProjectMenu->SetItemEnabled(kCloseAllTextCmd,
								   GetDocumentManager()->HasTextDocuments());
	itsProjectMenu->SetItemEnabled(kSaveAllTextCmd,
								   GetDocumentManager()->TextDocumentsNeedSave());
}

/******************************************************************************
 HandleProjectMenu (private)

 ******************************************************************************/

void
TreeDirector::HandleProjectMenu
	(
	const JIndex index
	)
{
	GetDocumentManager()->SetActiveProjectDocument(itsProjDoc);

	if (index == kShowSymbolBrowserCmd)
	{
		itsProjDoc->GetSymbolDirector()->Activate();
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
 HandlePrefsMenu (private)

 ******************************************************************************/

void
TreeDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kTreePrefsCmd)
	{
		EditTreePrefs();
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
		GetPrefsManager()->SaveWindowSize(kTreeWindSizeID, GetWindow());
	}
}

/******************************************************************************
 EditTreePrefs (private)

 ******************************************************************************/

void
TreeDirector::EditTreePrefs()
{
	auto* dlog =
		jnew EditTreePrefsDialog(itsTree->GetFontSize(),
								 itsTree->WillAutoMinimizeMILinks(),
								 itsTree->WillDrawMILinksOnTop(),
								 itsTreeWidget->WillRaiseWindowWhenSingleMatch());
	assert( dlog != nullptr );
	if (dlog->DoDialog())
	{
		dlog->UpdateSettings();
	}
}

/******************************************************************************
 SetTreePrefs

 ******************************************************************************/

void
TreeDirector::SetTreePrefs
	(
	const JSize	fontSize,
	const bool	autoMinMILinks,
	const bool	drawMILinksOnTop,
	const bool	raiseWhenSingleMatch
	)
{
	itsTree->SetFontSize(fontSize);
	itsTree->ShouldAutoMinimizeMILinks(autoMinMILinks);
	itsTree->ShouldDrawMILinksOnTop(drawMILinksOnTop);

	itsTreeWidget->ShouldRaiseWindowWhenSingleMatch(raiseWhenSingleMatch);
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
TreeDirector::ReadPrefs
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

	JSize fontSize;
	input >> fontSize;
	itsTree->SetFontSize(fontSize);

	if (vers < 3)
	{
		bool showInheritedFns;
		input >> JBoolFromString(showInheritedFns);
	}

	if (vers >= 1)
	{
		bool autoMinMILinks, drawMILinksOnTop;
		input >> JBoolFromString(autoMinMILinks) >> JBoolFromString(drawMILinksOnTop);
		itsTree->ShouldAutoMinimizeMILinks(autoMinMILinks);
		itsTree->ShouldDrawMILinksOnTop(drawMILinksOnTop);
	}

	if (vers >= 2)
	{
		bool raiseWhenSingleMatch;
		input >> JBoolFromString(raiseWhenSingleMatch);
		itsTreeWidget->ShouldRaiseWindowWhenSingleMatch(raiseWhenSingleMatch);
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
TreeDirector::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;

	output << ' ' << itsTree->GetFontSize();
	output << ' ' << JBoolToString(itsTree->WillAutoMinimizeMILinks());
	output << ' ' << JBoolToString(itsTree->WillDrawMILinksOnTop());
	output << ' ' << JBoolToString(itsTreeWidget->WillRaiseWindowWhenSingleMatch());
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
TreeDirector::ReceiveWithFeedback
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

		JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
		if (itsTree->GetSelectedClasses(&classList))
		{
			const JSize count = classList.GetItemCount();
			JString fullName;
			for (JIndex i=1; i<=count; i++)
			{
				if (classList.GetItem(i)->GetFileName(&fullName))
				{
					info->AddFile(fullName);
				}
			}
		}
	}
	else
	{
		JXWindowDirector::ReceiveWithFeedback(sender, message);
	}
}
