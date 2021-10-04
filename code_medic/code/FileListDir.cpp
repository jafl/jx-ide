/******************************************************************************
 FileListDir.cpp

	BASE CLASS = JXWindowDirector, JPrefObject

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "FileListDir.h"
#include "CommandDirector.h"
#include "GetSourceFileListCmd.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXFileListSet.h>
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Open source file... %k Meta-O %i" kOpenSourceFileAction
	"%l| Close               %k Meta-W %i" kJXCloseWindowAction
	"  | Quit                %k Meta-Q %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kCloseWindowCmd,
	kQuitCmd
};

// Actions menu

static const JUtf8Byte* kActionMenuStr =
	"    Use wildcard filter %b"
	"  | Use regex filter    %b";

enum
{
	kShowFilterCmd = 1,
	kShowRegexCmd
};

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About"
	"%l| Table of Contents"
	"  | Overview"
	"  | This window %k F1"
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

FileListDir::FileListDir
	(
	CommandDirector* supervisor
	)
	:
	JXWindowDirector(JXGetApplication()),
	JPrefObject(GetPrefsManager(), kFileListSetupID),
	itsCommandDir(supervisor)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	BuildWindow();

	itsCmd = itsLink->CreateGetSourceFileListCmd(this);

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileListDir::~FileListDir()
{
	JPrefObject::WritePrefs();
	GetPrefsManager()->SaveWindowSize(kFileWindSizeID, GetWindow());

	jdelete itsCmd;
}

/******************************************************************************
 GetTable

 ******************************************************************************/

JXFileListTable*
FileListDir::GetTable()
{
	return itsFileListSet->GetTable();
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

#include "medic_file_list_window.xpm"

#include <jx_file_open.xpm>
#include <jx_filter_wildcard.xpm>
#include <jx_filter_regex.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
FileListDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 250,500, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 250,30);
	assert( menuBar != nullptr );

	itsFileListSet =
		jnew JXFileListSet(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 250,470);
	assert( itsFileListSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::FileListDir"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetMinSize(150, 150);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetFileListWindowClass());
	GetPrefsManager()->GetWindowSize(kFileWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_file_list_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	ListenTo(GetTable());

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "ThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsActionsMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::Global"));
	itsActionsMenu->SetMenuItems(kActionMenuStr, "FileListDir");
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsActionsMenu->SetItemImage(kShowFilterCmd, jx_filter_wildcard);
	itsActionsMenu->SetItemImage(kShowRegexCmd,  jx_filter_regex);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "FileListDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
FileListDir::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString title = binaryName;
	title += JGetString("WindowTitleSuffix::FileListDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
FileListDir::GetName()
	const
{
	return JGetString("WindowsMenuText::FileListDir");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
FileListDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetFileListIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
FileListDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetTable() && message.Is(JXFileListTable::kProcessSelection))
	{
		JXFileListTable* table          = GetTable();
		const JPtrArray<JString>& files	= table->GetFullNameList();

		JTableSelectionIterator iter(&(table->GetTableSelection()));
		JPoint cell;
		while (iter.Next(&cell))
		{
			const JIndex index = table->RowIndexToFileIndex(cell.y);
			itsCommandDir->OpenSourceFile(*(files.GetElement(index)));
		}
	}

	else if (sender == itsLink && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
	}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
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

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
FileListDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		itsLink = GetLink();
		ListenTo(itsLink);

		jdelete itsCmd;
		itsCmd = itsLink->CreateGetSourceFileListCmd(this);
	}
	else
	{
		JXWindowDirector::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
FileListDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kCloseWindowCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
FileListDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
	{
		itsCommandDir->OpenSourceFiles();
	}

	else if (index == kCloseWindowCmd)
	{
		Deactivate();
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 UpdateActionsMenu

 ******************************************************************************/

void
FileListDir::UpdateActionsMenu()
{
	const JXFileListSet::FilterType type = itsFileListSet->GetFilterType();
	if (type == JXFileListSet::kWildcardFilter)
	{
		itsActionsMenu->CheckItem(kShowFilterCmd);
	}
	else if (type == JXFileListSet::kRegexFilter)
	{
		itsActionsMenu->CheckItem(kShowRegexCmd);
	}
}

/******************************************************************************
 HandleActionsMenu

 ******************************************************************************/

void
FileListDir::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kShowFilterCmd)
	{
		JXFileListSet::FilterType type = itsFileListSet->GetFilterType();
		if (type == JXFileListSet::kWildcardFilter)
		{
			itsFileListSet->SetFilterType(JXFileListSet::kNoFilter);
		}
		else
		{
			itsFileListSet->SetFilterType(JXFileListSet::kWildcardFilter);
		}
	}
	else if (index == kShowRegexCmd)
	{
		JXFileListSet::FilterType type = itsFileListSet->GetFilterType();
		if (type == JXFileListSet::kRegexFilter)
		{
			itsFileListSet->SetFilterType(JXFileListSet::kNoFilter);
		}
		else
		{
			itsFileListSet->SetFilterType(JXFileListSet::kRegexFilter);
		}
	}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
FileListDir::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
	{
		(GetApplication())->DisplayAbout();
	}
	else if (index == kTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kOverviewCmd)
	{
		JXGetHelpManager()->ShowSection("OverviewHelp");
	}
	else if (index == kThisWindowCmd)
	{
		JXGetHelpManager()->ShowSection("SourceWindowHelp-FileList");
	}
	else if (index == kChangesCmd)
	{
		JXGetHelpManager()->ShowChangeLog();
	}
	else if (index == kCreditsCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
FileListDir::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
	{
		itsFileListSet->ReadSetup(input);
	}
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
FileListDir::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	itsFileListSet->WriteSetup(output);
}
