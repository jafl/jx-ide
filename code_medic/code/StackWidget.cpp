/******************************************************************************
 StackWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "StackWidget.h"
#include "StackFrameNode.h"
#include "StackArgNode.h"
#include "CommandDirector.h"
#include "StackDir.h"
#include "GetStackCmd.h"
#include "GetFrameCmd.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JNamedTreeList.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

const JIndex kArgValueColIndex = 3;
const JSize kIndentWidth       = 4;	// characters: "xx:"

/******************************************************************************
 Constructor

 *****************************************************************************/

StackWidget::StackWidget
	(
	CommandDirector*	commandDir,
	StackDir*			stackDir,
	JTree*				tree,
	JNamedTreeList*		treeList,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXNamedTreeListWidget(treeList, scrollbarSet,
						  enclosure, hSizing, vSizing, x, y, w, h),
	itsCommandDir(commandDir),
	itsStackDir(stackDir),
	itsTree(tree),
	itsNeedsUpdateFlag(false),
	itsSmartFrameSelectFlag(false),
	itsIsWaitingForReloadFlag(false),
	itsChangingFrameFlag(false),
	itsSelectingFrameFlag(false)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	itsGetStackCmd = itsLink->CreateGetStackCmd(itsTree, this);
	itsGetFrameCmd = itsLink->CreateGetFrameCmd(this);

	AppendCols(1);	// argument values

	SetElasticColIndex(0);

	JString name;
	JSize size;
	GetPrefsManager()->GetDefaultFont(&name, &size);
	SetFont(name, size);

	SetIndentWidth(kIndentWidth * GetFont().GetCharWidth(GetFontManager(), JUtf8Character('0')));

	ListenTo(&(GetTableSelection()));
	ListenTo(GetWindow());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

StackWidget::~StackWidget()
{
	jdelete itsTree;
	jdelete itsGetStackCmd;
	jdelete itsGetFrameCmd;
}

/******************************************************************************
 GetStackFrame

 ******************************************************************************/

bool
StackWidget::GetStackFrame
	(
	const JUInt64			id,
	const StackFrameNode**	frame
	)
{
	const JTreeNode* root = itsTree->GetRoot();
	const JSize count     = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto& node = dynamic_cast<const StackFrameNode&>(*root->GetChild(i));
		if (node.GetID() == id)
		{
			*frame = &node;
			return true;
		}
	}

	*frame = nullptr;
	return false;
}

/******************************************************************************
 SelectFrame

 ******************************************************************************/

void
StackWidget::SelectFrame
	(
	const JUInt64 id
	)
{
	const JTreeList* list = GetTreeList();
	const JTreeNode* root = itsTree->GetRoot();

	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto& node = dynamic_cast<const StackFrameNode&>(*root->GetChild(i));
		if (node.GetID() == id)
		{
			JIndex j;
			const bool found = list->FindNode(&node, &j);
			assert( found );

			itsSelectingFrameFlag = true;
			SelectSingleCell(JPoint(GetNodeColIndex(), j));
			itsSelectingFrameFlag = false;

			break;
		}
	}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
StackWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if (JIndex(cell.x) == GetToggleOpenColIndex() ||
		JIndex(cell.x) == GetNodeColIndex())
	{
		JXNamedTreeListWidget::TableDrawCell(p,cell,rect);
	}
	else if (JIndex(cell.x) == kArgValueColIndex)
	{
		const JPoint fakeCell(GetNodeColIndex(), cell.y);
		HilightIfSelected(p, fakeCell, rect);

		const JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (node->GetDepth() > 1)
		{
			JFont font = GetFont();
			font.SetStyle(GetCellStyle(cell));
			p.SetFont(font);

			auto& argNode = dynamic_cast<const StackArgNode&>(*node);
			p.String(rect, argNode.GetValue(), JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
		}
	}
}

/******************************************************************************
 GetMinCellWidth (virtual protected)

 ******************************************************************************/

JSize
StackWidget::GetMinCellWidth
	(
	const JPoint& cell
	)
	const
{
	if (JIndex(cell.x) > GetNodeColIndex())
	{
		const JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (node->GetDepth() > 1)
		{
			auto& argNode = dynamic_cast<const StackArgNode&>(*node);
			return GetFont().GetStringWidth(GetFontManager(), argNode.GetValue());
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return JXNamedTreeListWidget::GetMinCellWidth(cell);
	}
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
StackWidget::AdjustCursor
	(
	const JPoint&			pt,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsIsWaitingForReloadFlag)
	{
		DisplayCursor(kJXBusyCursor);
	}
	else
	{
		JXNamedTreeListWidget::AdjustCursor(pt, modifiers);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
StackWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ClearIncrementalSearchBuffer();

	JPoint cell;
	if (ScrollForWheel(button, modifiers) ||
		!GetCell(pt, &cell))
	{
		return;
	}

	if (JIndex(cell.x) != GetToggleOpenColIndex())
	{
		if (modifiers.meta())
		{
			const JTreeNode* node = GetTreeList()->GetNode(cell.y);
			while (node->GetDepth() > 1)
			{
				node = node->GetParent();
			}

			auto& stackNode = dynamic_cast<const StackFrameNode&>(*node);
			JString fileName;
			JIndex lineIndex;
			if (stackNode.GetFile(&fileName, &lineIndex))
			{
				itsCommandDir->OpenSourceFile(fileName, lineIndex);
			}
			else
			{
				JGetUserNotification()->ReportError(JGetString("NoSourceFile::StackWidget"));
			}
		}
		else
		{
			SelectSingleCell(JPoint(GetNodeColIndex(), cell.y));
		}
	}
	else
	{
		itsSelectingFrameFlag = true;		// ignore selection changes during open/close
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		itsSelectingFrameFlag = false;
	}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
StackWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsSelectingFrameFlag = true;		// ignore selection changes during open/close
	JXNamedTreeListWidget::HandleMouseUp(pt, button, buttonStates, modifiers);
	itsSelectingFrameFlag = false;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
StackWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&   modifiers
	)
{
	if (c == kJUpArrow)
	{
		if (!SelectNextFrame(-1) && GetRowCount() > 0)
		{
			SelectSingleCell(JPoint(GetNodeColIndex(), GetRowCount()));
		}
		ClearIncrementalSearchBuffer();
	}
	else if (c == kJDownArrow)
	{
		if (!SelectNextFrame(+1) && GetRowCount() > 0)
		{
			SelectSingleCell(JPoint(GetNodeColIndex(), 1));
		}
		ClearIncrementalSearchBuffer();
	}
	else
	{
		if (c == kJLeftArrow || c == kJRightArrow)
		{
			itsSelectingFrameFlag = true;		// ignore selection changes during open/close
		}
		JXNamedTreeListWidget::HandleKeyPress(c, keySym, modifiers);
		itsSelectingFrameFlag = false;
	}
}

/******************************************************************************
 SelectNextFrame (private)

 ******************************************************************************/

bool
StackWidget::SelectNextFrame
	(
	const JInteger delta
	)
{
	const JTableSelection& s = GetTableSelection();
	JNamedTreeList* treeList = GetNamedTreeList();

	JPoint cell;
	if (s.GetFirstSelectedCell(&cell))
	{
		const JTreeNode* node   = treeList->GetNode(cell.y);
		const JTreeNode* parent = node->GetParent();

		JIndex i;
		bool found = parent->FindChild(node, &i);
		assert( found );

		i += delta;
		if (parent->ChildIndexValid(i))
		{
			node  = parent->GetChild(i);
			found = treeList->FindNode(node, &i);
			assert( found );

			SelectSingleCell(JPoint(GetNodeColIndex(), i));
		}

		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
StackWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(Link::kFrameChanged))
	{
		if (ShouldRebuild())
		{
			itsGetFrameCmd->Send();
		}
		else
		{
			itsNeedsUpdateFlag = true;
		}
	}
	else if (sender == itsLink && message.Is(Link::kThreadChanged))
	{
		Rebuild();
	}

	else if (sender == itsLink &&
			 (message.Is(Link::kProgramRunning)  ||
			  message.Is(Link::kProgramFinished) ||
			  message.Is(Link::kDetachedFromProcess)))
	{
		itsIsWaitingForReloadFlag = false;
		FlushOldData();
	}
	else if (sender == itsLink && message.Is(Link::kCoreCleared))
	{
		// When the user has set a breakpoint at the same location as the
		// tbreak for obtaining the PID, then we will only get ProgramStopped
		// before CoreCleared.  We must not discard our data in this case.

		if (!itsLink->IsDebugging())
		{
			FlushOldData();
		}
	}
	else if (sender == itsLink && message.Is(Link::kProgramStopped))
	{
		auto& info = dynamic_cast<const Link::ProgramStopped&>(message);
		if (!info.IsWaitingForThread())
		{
			// This is triggered when gdb prints file:line info.

			const bool wasChanging = itsChangingFrameFlag;
			itsChangingFrameFlag   = false;

			if (!wasChanging &&
				itsGetFrameCmd->GetState() == Command::kUnassigned)
			{
				itsSmartFrameSelectFlag = true;
				Rebuild();
			}
		}
	}

	else if (sender == itsLink &&
			 (message.Is(Link::kCoreLoaded) ||
			  message.Is(Link::kAttachedToProcess)))
	{
		itsNeedsUpdateFlag      = true;
		itsSmartFrameSelectFlag = true;
		itsStackDir->Activate();
	}

	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
	{
		Update();
	}

	else
	{
		JTableSelection& s = GetTableSelection();

		JPoint cell;
		if (!itsSelectingFrameFlag &&
			sender == &s && message.Is(JTableData::kRectChanged) &&
			s.GetFirstSelectedCell(&cell))
		{
			const JTreeNode* node = GetTreeList()->GetNode(cell.y);
			if (node->GetDepth() > 1)
			{
				itsCommandDir->DisplayExpression(
					GetNamedTreeList()->GetNamedNode(cell.y)->GetName());
				node = node->GetParent();
			}

			auto& stackNode = dynamic_cast<const StackFrameNode&>(*node);
			SwitchToFrame(stackNode.GetID());
		}

		JXNamedTreeListWidget::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
StackWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		itsLink = GetLink();
		ListenTo(itsLink);

		FlushOldData();

		jdelete itsGetStackCmd;
		itsGetStackCmd = itsLink->CreateGetStackCmd(itsTree, this);

		jdelete itsGetFrameCmd;
		itsGetFrameCmd = itsLink->CreateGetFrameCmd(this);
	}
	else
	{
		JXNamedTreeListWidget::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 SwitchToFrame

 ******************************************************************************/

void
StackWidget::SwitchToFrame
	(
	const JUInt64 id
	)
{
	itsChangingFrameFlag = true;
	itsLink->SwitchToFrame(id);
}

/******************************************************************************
 Update

 ******************************************************************************/

void
StackWidget::Update()
{
	if ((itsLink->HasCore() || itsLink->ProgramIsStopped()) &&
		itsNeedsUpdateFlag)
	{
		Rebuild();
	}
}

/******************************************************************************
 ShouldRebuild (private)

 ******************************************************************************/

bool
StackWidget::ShouldRebuild()
	const
{
	return itsStackDir->IsActive() && !GetWindow()->IsIconified();
}

/******************************************************************************
 Rebuild (private)

 ******************************************************************************/

void
StackWidget::Rebuild()
{
	if (ShouldRebuild())
	{
		itsIsWaitingForReloadFlag = true;
		FlushOldData();
		itsGetStackCmd->Send();		// need stack before selecting frame
		if (!itsSmartFrameSelectFlag)
		{
			itsGetFrameCmd->Send();
		}
	}
	else
	{
		itsNeedsUpdateFlag = true;
	}
}

/******************************************************************************
 FlushOldData (private)

 ******************************************************************************/

void
StackWidget::FlushOldData()
{
	itsSelectingFrameFlag = true;
	itsTree->GetRoot()->DeleteAllChildren();
	itsSelectingFrameFlag = false;

	itsNeedsUpdateFlag   = false;
	itsChangingFrameFlag = false;
}

/******************************************************************************
 FinishedLoading

 ******************************************************************************/

void
StackWidget::FinishedLoading
	(
	const JIndex initID
	)
{
	if (!itsIsWaitingForReloadFlag)		// program exited
	{
		FlushOldData();
		return;
	}

	SelectFrame(initID);
	if (itsSmartFrameSelectFlag && initID > 0)
	{
		itsLink->SwitchToFrame(initID);
	}
	itsIsWaitingForReloadFlag = false;
	itsSmartFrameSelectFlag   = false;
}
