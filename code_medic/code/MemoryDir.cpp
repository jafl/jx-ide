/******************************************************************************
 MemoryDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#include "MemoryDir.h"
#include "GetMemoryCmd.h"
#include "ArrayExprInput.h"
#include "ArrayIndexInput.h"
#include "CommandDirector.h"
#include "VarNode.h"
#include "globals.h"
#include "actionDefs.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXCloseDirectorTask.h>
#include <jx-af/jx/JXFontManager.h>

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
	"Save window size as default";

enum
{
	kSavePrefsCmd = 1
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

// Display type

static const JUtf8Byte* kDisplayTypeMenuStr =
	"    Hex bytes"
	"  | Hex short (2 bytes)"
	"  | Hex word (4 bytes)"
	"  | Hex long (8 bytes)"
	"%l| Characters (ISO8859-1)"
	"%l| Assembly code";

/******************************************************************************
 Constructor

 *****************************************************************************/

MemoryDir::MemoryDir
	(
	CommandDirector*	supervisor,
	const JString&		expr
	)
	:
	JXWindowDirector(JXGetApplication())
{
	// format variable for input field

	itsExpr = expr;
	VarNode::TrimExpression(&itsExpr);

	itsDisplayType = kHexByte;
	itsItemCount   = 64;

	MemoryDirX(supervisor);
}

MemoryDir::MemoryDir
	(
	std::istream&		input,
	const JFileVersion	vers,
	CommandDirector*	supervisor
	)
	:
	JXWindowDirector(JXGetApplication())
{
	long type;
	input >> itsExpr >> type >> itsItemCount;
	itsDisplayType = (DisplayType) type;

	MemoryDirX(supervisor);

	GetWindow()->ReadGeometry(input);
}

// private

void
MemoryDir::MemoryDirX
	(
	CommandDirector* supervisor
	)
{
	itsLink = GetLink();
	UpdateDisplayTypeMenu();
	ListenTo(itsLink);

	itsCommandDir           = supervisor;
	itsShouldUpdateFlag     = false;		// window is always initially hidden
	itsNeedsUpdateFlag      = !itsExpr.IsEmpty();
	itsWaitingForReloadFlag = false;

	BuildWindow();

	itsCmd = itsLink->CreateGetMemoryCmd(this);

	itsCommandDir->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MemoryDir::~MemoryDir()
{
	itsCommandDir->DirectorDeleted(this);
}

/******************************************************************************
 StreamOut

 *****************************************************************************/

void
MemoryDir::StreamOut
	(
	std::ostream& output
	)
{
	output << ' ' << itsExpr;
	output << ' ' << (long) itsDisplayType;
	output << ' ' << itsItemCount;
	GetWindow()->WriteGeometry(output);	// must be last
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
MemoryDir::Activate()
{
	JXWindowDirector::Activate();
	itsShouldUpdateFlag = true;
	Update();
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
MemoryDir::Deactivate()
{
	itsShouldUpdateFlag = false;
	return JXWindowDirector::Deactivate();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_memory_window.xpm"

#include <jx_file_open.xpm>
#include <jx_help_toc.xpm>
#include <jx_help_specific.xpm>

void
MemoryDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,500, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	auto* exprLabel =
		jnew JXStaticText(JGetString("exprLabel::MemoryDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 80,20);
	assert( exprLabel != nullptr );
	exprLabel->SetToLabel();

	itsExprInput =
		jnew ArrayExprInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,40, 380,20);
	assert( itsExprInput != nullptr );

	itsDisplayTypeMenu =
		jnew JXTextMenu(JGetString("itsDisplayTypeMenu::MemoryDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 400,25);
	assert( itsDisplayTypeMenu != nullptr );

	auto* countLabel =
		jnew JXStaticText(JGetString("countLabel::MemoryDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,105, 80,20);
	assert( countLabel != nullptr );
	countLabel->SetToLabel();

	itsItemCountInput =
		jnew ArrayIndexInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,105, 60,20);
	assert( itsItemCountInput != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,135, 500,365);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetMinSize(300, 200);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetMemoryWindowClass());

	UpdateWindowTitle();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_memory_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	GetPrefsManager()->GetWindowSize(kMemoryWindSizeID, window, true);

	itsWidget =
		jnew JXStaticText(JString::empty, false, true, true,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert(itsWidget != nullptr);
	itsWidget->FitToEnclosure();

	JString name;
	JSize size;
	GetPrefsManager()->GetDefaultFont(&name, &size);
	itsWidget->SetFont(JFontManager::GetFont(name, size));

	itsExprInput->GetText()->SetText(itsExpr);
	itsExprInput->SetIsRequired();
	ListenTo(itsExprInput);

	itsDisplayTypeMenu->SetMenuItems(kDisplayTypeMenuStr);
	itsDisplayTypeMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsDisplayTypeMenu->SetToPopupChoice(true, itsDisplayType);
	ListenTo(itsDisplayTypeMenu);

	itsItemCountInput->SetLowerLimit(1);
	itsItemCountInput->SetValue(itsItemCount);
	ListenTo(itsItemCountInput);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "ThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd, jx_file_open);

	itsWidget->AppendEditMenu(menuBar);
	itsExprInput->ShareEditMenu(itsWidget);
	itsItemCountInput->ShareEditMenu(itsWidget);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("ActionsMenuTitle::Global"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionMenuStr, "MemoryDir");
	ListenTo(itsActionMenu);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "MemoryDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	(GetDisplay()->GetWDManager())->DirectorCreated(this);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
MemoryDir::UpdateWindowTitle()
{
	JString title = itsExpr;
	title += JGetString("WindowTitleSuffix::MemoryDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
MemoryDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetMemoryIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
MemoryDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsExprInput &&
		(message.Is(JXWidget::kLostFocus) ||
		 message.Is(ArrayExprInput::kReturnKeyPressed)))
	{
		if (itsExprInput->GetText()->GetText() != itsExpr)
		{
			itsExpr = itsExprInput->GetText()->GetText();
			UpdateWindowTitle();
			itsNeedsUpdateFlag = true;
			Update();
		}
	}
	else if (sender == itsDisplayTypeMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		if (selection->GetIndex() != (JIndex) itsDisplayType)
		{
			itsDisplayType = (DisplayType) selection->GetIndex();

			// update all other values, too, since menu selection doesn't trigger input fields

			itsExpr = itsExprInput->GetText()->GetText();

			JInteger value;
			if (itsItemCountInput->GetValue(&value))
			{
				itsItemCount = value;
			}

			itsNeedsUpdateFlag = true;
			Update();
		}
	}
	else if (sender == itsItemCountInput &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(ArrayIndexInput::kReturnKeyPressed)))
	{
		JInteger value;
		if (itsItemCountInput->GetValue(&value) && JSize(value) != itsItemCount)
		{
			itsItemCount       = value;
			itsNeedsUpdateFlag = true;
			Update();
		}
	}

	else if (sender == itsLink && message.Is(Link::kDebuggerRestarted))
	{
		itsWaitingForReloadFlag = true;
	}
	else if (sender == itsLink && message.Is(Link::kDebuggerStarted))
	{
		if (!itsWaitingForReloadFlag)
		{
			JXCloseDirectorTask::Close(this);	// close after bcast is finished
		}
		itsWaitingForReloadFlag = false;
	}
	else if (sender == itsLink &&
			 (message.Is(Link::kProgramFinished) ||
			  message.Is(Link::kCoreCleared)     ||
			  message.Is(Link::kDetachedFromProcess)))
	{
		Update(JString::empty);
	}
	else if (sender == itsLink && VarNode::ShouldUpdate(message))
	{
		itsNeedsUpdateFlag = true;
		Update();
	}

	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
	}

	else if (sender == itsActionMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateActionMenu();
	}
	else if (sender == itsActionMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionMenu(selection->GetIndex());
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
MemoryDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		JXCloseDirectorTask::Close(this);
	}
	else
	{
		JXWindowDirector::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 UpdateDisplayTypeMenu (private)

 ******************************************************************************/

void
MemoryDir::UpdateDisplayTypeMenu()
{
	if (!itsLink->GetFeature(Link::kExamineMemory))
	{
		itsDisplayTypeMenu->DisableItem(kHexByte);
		itsDisplayTypeMenu->DisableItem(kHexShort);
		itsDisplayTypeMenu->DisableItem(kHexWord);
		itsDisplayTypeMenu->DisableItem(kHexLong);
		itsDisplayTypeMenu->DisableItem(kChar);
	}

	if (!itsLink->GetFeature(Link::kDisassembleMemory))
	{
		itsDisplayTypeMenu->DisableItem(kAsm);
	}
}

/******************************************************************************
 SetDisplayType

 ******************************************************************************/

void
MemoryDir::SetDisplayType
	(
	const DisplayType type
	)
{
	if (itsDisplayTypeMenu->IsEnabled(type))
	{
		itsDisplayType = type;
		itsDisplayTypeMenu->SetPopupChoice(type);
	}
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
MemoryDir::Update()
{
	if (itsShouldUpdateFlag && itsNeedsUpdateFlag)
	{
		itsNeedsUpdateFlag = false;

		if (itsCmd != nullptr)
		{
			itsCmd->Command::Send();
		}
	}
}

/******************************************************************************
 Update

 ******************************************************************************/

void
MemoryDir::Update
	(
	const JString& data
	)
{
	itsWidget->GetText()->SetText(data);
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
MemoryDir::HandleFileMenu
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
		Close();
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 UpdateActionMenu

 ******************************************************************************/

void
MemoryDir::UpdateActionMenu()
{
	itsActionMenu->EnableItem(kSavePrefsCmd);
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
MemoryDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kSavePrefsCmd)
	{
		GetPrefsManager()->SaveWindowSize(kMemoryWindSizeID, GetWindow());
	}
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
MemoryDir::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("VarTreeHelp-Memory");
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
