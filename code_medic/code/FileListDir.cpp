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
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

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
	if (HasPrefsManager())
	{
		JPrefObject::WritePrefs();
		GetPrefsManager()->SaveWindowSize(kFileWindSizeID, GetWindow());
	}

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

#include "Generic-File.h"
#include "FileListDir-Actions.h"
#include "medic_file_list_window.xpm"

void
FileListDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 250,500, JString::empty);

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
	window->SetIcon(icon);

	ListenTo(GetTable());

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::Generic_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&FileListDir::UpdateFileMenu,
		&FileListDir::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::FileListDir_Actions"));
	itsActionMenu->SetMenuItems(kActionsMenuStr);
	itsActionMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsActionMenu->AttachHandlers(this,
		&FileListDir::UpdateActionMenu,
		&FileListDir::HandleActionMenu);
	ConfigureActionsMenu(itsActionMenu);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	GetApplication()->CreateHelpMenu(menuBar, "SourceWindowHelp-FileList");
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
			itsCommandDir->OpenSourceFile(*(files.GetItem(index)));
		}
	}

	else if (sender == itsLink && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
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
	itsFileMenu->SetItemEnabled(kCloseWindowCmd, !GetWindow()->IsDocked());
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
FileListDir::UpdateActionMenu()
{
	const JXFileListSet::FilterType type = itsFileListSet->GetFilterType();
	if (type == JXFileListSet::kWildcardFilter)
	{
		itsActionMenu->CheckItem(kShowFilterCmd);
	}
	else if (type == JXFileListSet::kRegexFilter)
	{
		itsActionMenu->CheckItem(kShowRegexCmd);
	}
}

/******************************************************************************
 HandleActionsMenu

 ******************************************************************************/

void
FileListDir::HandleActionMenu
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
