/******************************************************************************
 SymbolSRDirector.cpp

	Window to display result of searching SymbolList.

	BASE CLASS = JXWindowDirector

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "SymbolSRDirector.h"
#include "SymbolDirector.h"
#include "SymbolTable.h"
#include "SymbolList.h"
#include "ProjectDocument.h"
#include "CTreeDirector.h"
#include "DTreeDirector.h"
#include "GoTreeDirector.h"
#include "JavaTreeDirector.h"
#include "PHPTreeDirector.h"
#include "Tree.h"
#include "CommandMenu.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// Actions menu

static const JUtf8Byte* kActionsMenuStr =
	"    Copy selected names           %k Meta-C"
	"%l| Update                        %k Meta-U"
	"  | Show C++ class tree           %k Meta-Shift-C"
	"  | Show D class tree             %k Meta-Shift-D"
	"  | Show Go struct/interface tree %k Meta-Shift-G"
	"  | Show Java class tree          %k Meta-Shift-J"
	"  | Show PHP class tree           %k Meta-Shift-P"
	"%l| Close window                  %k Meta-W"
	"  | Close all                     %k Meta-Shift-W";

enum
{
	kCopySelNamesCmd = 1,
	kUpdateCmd,
	kShowCTreeCmd, kShowDTreeCmd, kShowGoTreeCmd, kShowJavaTreeCmd, kShowPHPTreeCmd,
	kCloseWindowCmd, kCloseAllCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

SymbolSRDirector::SymbolSRDirector
	(
	SymbolDirector*		mainSym,
	ProjectDocument*	projDoc,
	SymbolList*			symbolList,
	const JString&		filterStr,
	const bool			isRegex,
	JError*				err
	)
	:
	JXWindowDirector(projDoc)
{
	SymbolSRDirectorX(mainSym, projDoc, symbolList, true);

	*err = itsSymbolTable->SetNameFilter(filterStr, isRegex);
	FitWindowToContent();

	JString title = JGetString("WindowTitlePrefix::SymbolSRDirector");
	title += filterStr;
	GetWindow()->SetTitle(title);
}

SymbolSRDirector::SymbolSRDirector
	(
	SymbolDirector*			mainSym,
	ProjectDocument*		projDoc,
	SymbolList*				symbolList,
	const JArray<JIndex>&	displayList,
	const JString&			searchStr
	)
	:
	JXWindowDirector(projDoc)
{
	SymbolSRDirectorX(mainSym, projDoc, symbolList, true);

	itsSymbolTable->SetDisplayList(displayList);
	FitWindowToContent();

	JString title = JGetString("WindowTitlePrefix::SymbolSRDirector");
	title += searchStr;
	GetWindow()->SetTitle(title);
}

// private

void
SymbolSRDirector::SymbolSRDirectorX
	(
	SymbolDirector*	owner,
	ProjectDocument*	projDoc,
	SymbolList*		symbolList,
	const bool		focus
	)
{
	itsMainSymDir = owner;
	itsProjDoc    = projDoc;
	itsSymbolList = symbolList;

	BuildWindow(itsSymbolList, focus);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SymbolSRDirector::~SymbolSRDirector()
{
	itsMainSymDir->SRDirectorClosed(this);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "jcc_show_c_tree.xpm"
#include "jcc_show_d_tree.xpm"
#include "jcc_show_go_tree.xpm"
#include "jcc_show_java_tree.xpm"
#include "jcc_show_php_tree.xpm"

void
SymbolSRDirector::BuildWindow
	(
	SymbolList*	symbolList,
	const bool	focus
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,230, JString::empty);
	assert( window != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,200);
	assert( scrollbarSet != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

// end JXLayout

	window->LockCurrentMinSize();

	if (focus)
	{
		window->ShouldFocusWhenShow(true);
	}

	itsSymbolTable =
		jnew SymbolTable(itsMainSymDir, symbolList,
						  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsSymbolTable != nullptr );
	itsSymbolTable->FitToEnclosure();

	itsActionsMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::SymbolSRDirector"));
	itsActionsMenu->SetMenuItems(kActionsMenuStr, "SymbolSRDirector");
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsActionsMenu->SetItemImage(kShowCTreeCmd,    jcc_show_c_tree);
	itsActionsMenu->SetItemImage(kShowDTreeCmd,    jcc_show_d_tree);
	itsActionsMenu->SetItemImage(kShowGoTreeCmd,   jcc_show_go_tree);
	itsActionsMenu->SetItemImage(kShowJavaTreeCmd, jcc_show_java_tree);
	itsActionsMenu->SetItemImage(kShowPHPTreeCmd,  jcc_show_php_tree);

	itsCmdMenu =
		jnew CommandMenu(itsProjDoc, nullptr, menuBar,
						  JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsCmdMenu != nullptr );
	menuBar->AppendMenu(itsCmdMenu);
	ListenTo(itsCmdMenu);
}

/******************************************************************************
 FitWindowToContent (private)

 ******************************************************************************/

void
SymbolSRDirector::FitWindowToContent()
{
	GetWindow()->AdjustSize(
		itsSymbolTable->GetBoundsWidth() - itsSymbolTable->GetApertureWidth(),
		JMin(itsSymbolTable->GetBoundsHeight() - itsSymbolTable->GetApertureHeight(),
			 (JCoordinate) 300));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SymbolSRDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateActionsMenu();
	}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionsMenu(selection->GetIndex());
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
SymbolSRDirector::ReceiveWithFeedback
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

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
SymbolSRDirector::UpdateActionsMenu()
{
	itsActionsMenu->SetItemEnabled(kCopySelNamesCmd, itsSymbolTable->HasSelection());

	itsActionsMenu->SetItemEnabled(kShowCTreeCmd,
		!itsProjDoc->GetCTreeDirector()->GetTree()->IsEmpty());
	itsActionsMenu->SetItemEnabled(kShowDTreeCmd,
		!itsProjDoc->GetDTreeDirector()->GetTree()->IsEmpty());
	itsActionsMenu->SetItemEnabled(kShowGoTreeCmd,
		!itsProjDoc->GetGoTreeDirector()->GetTree()->IsEmpty());
	itsActionsMenu->SetItemEnabled(kShowJavaTreeCmd,
		!itsProjDoc->GetJavaTreeDirector()->GetTree()->IsEmpty());
	itsActionsMenu->SetItemEnabled(kShowPHPTreeCmd,
		!itsProjDoc->GetPHPTreeDirector()->GetTree()->IsEmpty());
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
SymbolSRDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kCopySelNamesCmd)
	{
		itsSymbolTable->CopySelectedSymbolNames();
	}

	else if (index == kUpdateCmd)
	{
		itsProjDoc->UpdateSymbolDatabase();	// can delete us
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

	else if (index == kCloseWindowCmd)
	{
		Close();
	}
	else if (index == kCloseAllCmd)
	{
		itsMainSymDir->CloseSymbolBrowsers();
	}
}
