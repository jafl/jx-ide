/******************************************************************************
 Array1DDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "Array1DDir.h"
#include "ArrayExprInput.h"
#include "ArrayIndexInput.h"
#include "VarTreeWidget.h"
#include "VarNode.h"
#include "CommandDirector.h"
#include "globals.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXCloseDirectorTask.h>

#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JNamedTreeList.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

Array1DDir::Array1DDir
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

	itsRequestRange.Set(0, 10);
	Array1DDirX(supervisor);
}

Array1DDir::Array1DDir
	(
	std::istream&			input,
	const JFileVersion	vers,
	CommandDirector*	supervisor
	)
	:
	JXWindowDirector(JXGetApplication())
{
	input >> itsExpr >> itsRequestRange;
	Array1DDirX(supervisor);
	GetWindow()->ReadGeometry(input);
}

// private

void
Array1DDir::Array1DDirX
	(
	CommandDirector* supervisor
	)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	itsCommandDir           = supervisor;
	itsCurrentNode          = nullptr;
	itsWaitingForReloadFlag = false;
	itsDisplayRange.SetToEmptyAt(0);

	BuildWindow();
	BeginCreateNodes();

	itsCommandDir->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Array1DDir::~Array1DDir()
{
	itsCommandDir->DirectorDeleted(this);
}

/******************************************************************************
 StreamOut

 *****************************************************************************/

void
Array1DDir::StreamOut
	(
	std::ostream& output
	)
{
	output << ' ' << itsExpr;
	output << ' ' << itsRequestRange;
	GetWindow()->WriteGeometry(output);	// must be last
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
Array1DDir::Activate()
{
	JXWindowDirector::Activate();
	itsWidget->ShouldUpdate(true);
}

/******************************************************************************
 Deactivate (virtual)

 ******************************************************************************/

bool
Array1DDir::Deactivate()
{
	itsWidget->ShouldUpdate(false);
	return JXWindowDirector::Deactivate();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "Generic-File.h"
#include "Array1DDir-Actions.h"
#include "medic_1d_array_window.xpm"

void
Array1DDir::BuildWindow()
{
	VarNode* root = itsLink->CreateVarNode();
	assert( root != nullptr );
	itsTree = jnew JTree(root);
	auto* treeList = jnew JNamedTreeList(itsTree);

// begin JXLayout

	auto* window = jnew JXWindow(this, 300,500, JString::empty);
	window->SetMinSize(300, 300);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Medic_Variables_Array_1D");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 300,30);
	assert( menuBar != nullptr );

	auto* exprLabel =
		jnew JXStaticText(JGetString("exprLabel::Array1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 90,20);
	exprLabel->SetToLabel(false);

	itsExprInput =
		jnew ArrayExprInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,40, 170,20);

	auto* startLabel =
		jnew JXStaticText(JGetString("startLabel::Array1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,70, 90,20);
	startLabel->SetToLabel(false);

	itsStartIndex =
		jnew ArrayIndexInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,70, 60,20);

	itsStopButton =
		jnew JXTextButton(JGetString("itsStopButton::Array1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 200,85, 60,20);
	itsStopButton->SetShortcuts(JGetString("itsStopButton::shortcuts::Array1DDir::JXLayout"));

	auto* endLabel =
		jnew JXStaticText(JGetString("endLabel::Array1DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,100, 90,20);
	endLabel->SetToLabel(false);

	itsEndIndex =
		jnew ArrayIndexInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,100, 60,20);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,130, 300,370);
	assert( scrollbarSet != nullptr );

	itsWidget =
		jnew VarTreeWidget(itsCommandDir, false, menuBar, itsTree, treeList, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 300,370);

// end JXLayout

	window->ShouldFocusWhenShow(true);

	UpdateWindowTitle();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_1d_array_window);
	window->SetIcon(icon);

	GetPrefsManager()->GetWindowSize(kArray1DWindSizeID, window, true);

	itsExprInput->GetText()->SetText(itsExpr);
	itsExprInput->SetIsRequired();
	ListenTo(itsExprInput);

	itsStartIndex->SetValue(itsRequestRange.first);
	ListenTo(itsStartIndex);

	itsEndIndex->SetValue(itsRequestRange.last);
	ListenTo(itsEndIndex);

	itsStopButton->Hide();
	ListenTo(itsStopButton);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::Generic_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandler(this, &Array1DDir::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	JXTEBase* te = itsWidget->GetEditMenuHandler();
	itsExprInput->ShareEditMenu(te);
	itsStartIndex->ShareEditMenu(te);
	itsEndIndex->ShareEditMenu(te);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::Array1DDir_Actions"));
	menuBar->InsertMenu(3, itsActionMenu);
	itsActionMenu->SetMenuItems(kActionsMenuStr);
	itsActionMenu->AttachHandlers(this,
		&Array1DDir::UpdateActionMenu,
		&Array1DDir::HandleActionMenu);
	ConfigureActionsMenu(itsActionMenu);

	GetApplication()->CreateWindowsMenu(menuBar);
	GetApplication()->CreateHelpMenu(menuBar, "VarTreeHelp-Array1D");

	GetDisplay()->GetWDManager()->DirectorCreated(this);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
Array1DDir::UpdateWindowTitle()
{
	JString title = itsExpr;
	title += JGetString("WindowTitleSuffix::Array1DDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
Array1DDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetArray1DIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Array1DDir::Receive
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
			itsDisplayRange.SetToEmptyAt(0);
			BeginCreateNodes();
		}
	}
	else if (sender == itsStartIndex &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(ArrayIndexInput::kReturnKeyPressed)))
	{
		JInteger value;
		if (itsStartIndex->GetValue(&value) && value != itsRequestRange.first)
		{
			itsRequestRange.first = value;
			BeginCreateNodes();
		}
	}
	else if (sender == itsEndIndex &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(ArrayIndexInput::kReturnKeyPressed)))
	{
		JInteger value;
		if (itsEndIndex->GetValue(&value) && value != itsRequestRange.last)
		{
			itsRequestRange.last = value;
			BeginCreateNodes();
		}
	}

	else if (sender == itsTree && message.Is(JTree::kNodeChanged))
	{
		auto* info = dynamic_cast<const JTree::NodeChanged*>(&message);
		assert(info != nullptr);
		if (info->GetNode() == itsCurrentNode)
		{
			itsCurrentNode = nullptr;
			CreateNextNode();
		}
	}

	else if (sender == itsStopButton && message.Is(JXButton::kPushed))
	{
		itsRequestRange = itsDisplayRange;
		itsStartIndex->SetValue(itsRequestRange.first);
		itsEndIndex->SetValue(itsRequestRange.last);
		CreateNodesFinished();
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

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
Array1DDir::ReceiveGoingAway
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
 BeginCreateNodes (private)

 ******************************************************************************/

void
Array1DDir::BeginCreateNodes()
{
	JIntRange overlap;
	if (JIntersection(itsDisplayRange, itsRequestRange, &overlap))
	{
		JTreeNode* root = itsTree->GetRoot();
		while (itsDisplayRange.first < itsRequestRange.first)
		{
			JTreeNode* node = root->GetChild(1);
			if (node == itsCurrentNode)
			{
				itsCurrentNode = nullptr;
			}
			jdelete node;
			itsDisplayRange.first++;
		}
		while (itsDisplayRange.last > itsRequestRange.last)
		{
			JTreeNode* node = root->GetChild(root->GetChildCount());
			if (node == itsCurrentNode)
			{
				itsCurrentNode = nullptr;
			}
			jdelete node;
			itsDisplayRange.last--;
		}
	}
	else
	{
		(itsTree->GetRoot())->DeleteAllChildren();
		itsCurrentNode = nullptr;
		itsDisplayRange.SetToEmptyAt(itsRequestRange.first);
	}

	if (itsRequestRange.first < itsDisplayRange.first ||
		itsDisplayRange.last  < itsRequestRange.last)
	{
		itsStopButton->Show();
		CreateNextNode();
	}
}

/******************************************************************************
 CreateNextNode (private)

 ******************************************************************************/

void
Array1DDir::CreateNextNode()
{
	if (itsCurrentNode != nullptr)
	{
		return;
	}

	JTreeNode* root = itsTree->GetRoot();
	if (itsRequestRange.first < itsDisplayRange.first)
	{
		itsDisplayRange.first--;
		const JString expr = GetExpression(itsDisplayRange.first);
		VarNode* node      = itsLink->CreateVarNode(root, expr, expr, JString::empty);
		assert( node != nullptr );

		itsCurrentNode = node;
		root->InsertAtIndex(1, node);	// move it to the top -- inserted by ctor so it will update self
		ListenTo(itsTree);
	}
	else if (itsDisplayRange.last < itsRequestRange.last)
	{
		itsDisplayRange.last++;
		const JString expr = GetExpression(itsDisplayRange.last);
		VarNode* node      = itsLink->CreateVarNode(root, expr, expr, JString::empty);
		assert( node != nullptr );

		itsCurrentNode = node;
		ListenTo(itsTree);
	}
	else
	{
		CreateNodesFinished();
	}
}

/******************************************************************************
 GetExpression (private)

 ******************************************************************************/

JString
Array1DDir::GetExpression
	(
	const JInteger i
	)
	const
{
	return itsLink->Build1DArrayExpression(itsExpr, i);
}

/******************************************************************************
 CreateNodesFinished (private)

 ******************************************************************************/

void
Array1DDir::CreateNodesFinished()
{
	itsCurrentNode = nullptr;
	StopListening(itsTree);

	itsStopButton->Hide();
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
Array1DDir::HandleFileMenu
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
Array1DDir::UpdateActionMenu()
{
	if (itsWidget->HasSelection())
	{
		itsActionMenu->EnableItem(kDisplay1DArrayCmd);
		itsActionMenu->EnableItem(kPlot1DArrayCmd);
		itsActionMenu->EnableItem(kDisplay2DArrayCmd);
		itsActionMenu->EnableItem(kWatchVarCmd);
		itsActionMenu->EnableItem(kWatchLocCmd);
	}

	if (itsLink->GetFeature(Link::kExamineMemory))
	{
		itsActionMenu->EnableItem(kExamineMemCmd);
	}
	if (itsLink->GetFeature(Link::kDisassembleMemory))
	{
		itsActionMenu->EnableItem(kDisassembleMemCmd);
	}

	itsActionMenu->EnableItem(kSavePrefsCmd);
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
Array1DDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kDisplay1DArrayCmd)
	{
		itsWidget->Display1DArray();
	}
	else if (index == kPlot1DArrayCmd)
	{
		itsWidget->Plot1DArray();
	}
	else if (index == kDisplay2DArrayCmd)
	{
		itsWidget->Display2DArray();
	}

	else if (index == kWatchVarCmd)
	{
		itsWidget->WatchExpression();
	}
	else if (index == kWatchLocCmd)
	{
		itsWidget->WatchLocation();
	}

	else if (index == kExamineMemCmd)
	{
		itsWidget->ExamineMemory(MemoryDir::kHexByte);
	}
	else if (index == kDisassembleMemCmd)
	{
		itsWidget->ExamineMemory(MemoryDir::kAsm);
	}

	else if (index == kSavePrefsCmd)
	{
		GetPrefsManager()->SaveWindowSize(kArray1DWindSizeID, GetWindow());
	}
}
