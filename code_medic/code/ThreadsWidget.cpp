/******************************************************************************
 ThreadsWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 2001-11 by John Lindal.

 *****************************************************************************/

#include "ThreadsWidget.h"
#include "ThreadNode.h"
#include "CommandDirector.h"
#include "ThreadsDir.h"
#include "GetThreadsCmd.h"
#include "GetThreadCmd.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jx/JXDeleteObjectTask.h>
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JNamedTreeList.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

ThreadsWidget::ThreadsWidget
	(
	CommandDirector*	commandDir,
	ThreadsDir*			threadDir,
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
	itsThreadDir(threadDir),
	itsTree(tree),
	itsLastSelectedThreadID(0),
	itsNeedsUpdateFlag(false),
	itsIsWaitingForReloadFlag(false),
	itsChangingThreadFlag(false),
	itsSelectingThreadFlag(false),
	itsFlushWhenRunFlag(true),
	itsOpenIDList(nullptr),
	itsFlushStateTask(nullptr)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	itsGetThreadsCmd       = itsLink->CreateGetThreadsCmd(itsTree, this);
	itsGetCurrentThreadCmd = itsLink->CreateGetThreadCmd(this);

	SetElasticColIndex(0);

	JString name;
	JSize size;
	GetPrefsManager()->GetDefaultFont(&name, &size);
	SetFont(name, size);

	ListenTo(&GetTableSelection());
	ListenTo(GetWindow());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

ThreadsWidget::~ThreadsWidget()
{
	jdelete itsTree;
	JXDeleteObjectTask<GetThreadsCmd>::Delete(itsGetThreadsCmd);
	jdelete itsGetCurrentThreadCmd;
	jdelete itsOpenIDList;
	jdelete itsFlushStateTask;
}

/******************************************************************************
 SelectThread

 ******************************************************************************/

void
ThreadsWidget::SelectThread
	(
	const JIndex	id,
	const bool		fromUpdate
	)
{
	if (CalledBySelectThread(itsTree->GetRoot(), id, fromUpdate))
	{
		itsLastSelectedThreadID = id;
	}
}

/******************************************************************************
 CalledBySelectThread (private)

 ******************************************************************************/

bool
ThreadsWidget::CalledBySelectThread
	(
	const JTreeNode*	root,
	const JIndex		id,
	const bool			fromUpdate
	)
{
	const JTreeList* list = GetTreeList();

	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto& node = dynamic_cast<const ThreadNode&>(*root->GetChild(i));
		if (node.GetID() == id)
		{
			JIndex j;
			const bool found = list->FindNode(&node, &j);
			if (found)
			{
				itsSelectingThreadFlag = fromUpdate;
				SelectSingleCell(JPoint(GetNodeColIndex(), j));
				itsSelectingThreadFlag = false;
			}

			return true;
		}

		if (CalledBySelectThread(&node, id, fromUpdate))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 AdjustCursor (virtual protected)

 ******************************************************************************/

void
ThreadsWidget::AdjustCursor
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
ThreadsWidget::HandleMouseDown
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
			auto& threadNode      = dynamic_cast<const ThreadNode&>(*node);

			JString fileName;
			JIndex lineIndex;
			if (threadNode.GetFile(&fileName, &lineIndex))
			{
				itsCommandDir->OpenSourceFile(fileName, lineIndex);
			}
			else
			{
				JGetUserNotification()->ReportError(JGetString("NoSourceFile::ThreadsWidget"));
			}
		}
		else
		{
			SelectSingleCell(JPoint(GetNodeColIndex(), cell.y));
		}
	}
	else
	{
		itsSelectingThreadFlag = true;		// ignore selection changes during open/close
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		itsSelectingThreadFlag = false;
	}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
ThreadsWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsSelectingThreadFlag = true;		// ignore selection changes during open/close
	JXNamedTreeListWidget::HandleMouseUp(pt, button, buttonStates, modifiers);
	itsSelectingThreadFlag = false;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
ThreadsWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&   modifiers
	)
{
	if (c == kJUpArrow)
	{
		if (!SelectNextThread(-1) && GetRowCount() > 0)
		{
			SelectSingleCell(JPoint(GetNodeColIndex(), GetRowCount()));
		}
		ClearIncrementalSearchBuffer();
	}
	else if (c == kJDownArrow)
	{
		if (!SelectNextThread(+1) && GetRowCount() > 0)
		{
			SelectSingleCell(JPoint(GetNodeColIndex(), 1));
		}
		ClearIncrementalSearchBuffer();
	}
	else
	{
		if (c == kJLeftArrow || c == kJRightArrow)
		{
			itsSelectingThreadFlag = true;		// ignore selection changes during open/close
		}
		JXNamedTreeListWidget::HandleKeyPress(c, keySym, modifiers);
		itsSelectingThreadFlag = false;
	}
}

/******************************************************************************
 SelectNextThread (private)

 ******************************************************************************/

bool
ThreadsWidget::SelectNextThread
	(
	const JInteger delta
	)
{
	const JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (s.GetFirstSelectedCell(&cell))
	{
		cell.y += delta;
		if (RowIndexValid(cell.y))
		{
			SelectSingleCell(cell);
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
ThreadsWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(Link::kThreadChanged))
	{
		if (ShouldRebuild())
		{
			itsIsWaitingForReloadFlag = true;
			itsGetCurrentThreadCmd->Send();
		}
		else
		{
			itsNeedsUpdateFlag = true;
		}
	}
	else if (sender == itsLink && message.Is(Link::kThreadListChanged))
	{
		itsFlushWhenRunFlag = false;
		if (!itsIsWaitingForReloadFlag)
		{
			SaveOpenNodes();
			Rebuild();
		}
	}

	else if (sender == itsLink &&
			 (message.Is(Link::kProgramRunning) && itsFlushWhenRunFlag))
	{
		itsIsWaitingForReloadFlag = false;
		itsFlushStateTask = jnew JXFunctionTask(itsLink->GetPauseForStepInterval(), [this]()
		{
			itsFlushStateTask = nullptr;
			FlushOldData();
		},
		"ThreadsWidget::PauseBeforeFlushOldData",
		true);
		itsFlushStateTask->Start();
	}
	else if (sender == itsLink &&
			 (message.Is(Link::kProgramFinished) ||
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
		jdelete itsFlushStateTask;
		itsFlushStateTask = nullptr;

		// This is triggered when gdb prints file:line info.
		// (It would be nice to avoid this if only the frame changed,
		//  but we can't merely set a flag when we get kFrameChanged
		//  because we won't always get kProgramStopped afterwards.)

		const bool wasChanging = itsChangingThreadFlag;
		itsChangingThreadFlag  = false;

		if (!wasChanging)
		{
			Rebuild();
		}
	}
	else if (sender == itsLink && message.Is(Link::kFrameChanged))
	{
		// We don't need to rebuild our list when we get the next
		// ProgramStopped message.

		itsChangingThreadFlag = true;
	}

	else if (sender == itsLink &&
			 (message.Is(Link::kCoreLoaded) ||
			  message.Is(Link::kAttachedToProcess)))
	{
		itsNeedsUpdateFlag = true;

//		We can't do this because gdb often reports threads when a core file
//		is loaded, even if the program doesn't use threads.
//
//		CheckForThreads* cmd = jnew CheckForThreads(itsThreadDir);
//		cmd->Send();
	}

	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
	{
		Update();
	}

	else
	{
		JTableSelection& s = GetTableSelection();

		JPoint cell;
		if (!itsSelectingThreadFlag &&
			sender == &s && message.Is(JTableData::kRectChanged) &&
			s.GetFirstSelectedCell(&cell))
		{
			const JTreeNode* node = GetTreeList()->GetNode(cell.y);
			auto& threadNode      = dynamic_cast<const ThreadNode&>(*node);

			itsChangingThreadFlag   = true;
			itsLastSelectedThreadID = threadNode.GetID();
			itsLink->SwitchToThread(itsLastSelectedThreadID);
		}

		JXNamedTreeListWidget::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
ThreadsWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		itsLink = GetLink();
		ListenTo(itsLink);

		itsFlushWhenRunFlag = true;
		FlushOldData();

		jdelete itsGetThreadsCmd;
		itsGetThreadsCmd = itsLink->CreateGetThreadsCmd(itsTree, this);

		jdelete itsGetCurrentThreadCmd;
		itsGetCurrentThreadCmd = itsLink->CreateGetThreadCmd(this);

		jdelete itsOpenIDList;
		itsOpenIDList = nullptr;
	}
	else
	{
		JXNamedTreeListWidget::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 Update

 ******************************************************************************/

void
ThreadsWidget::Update()
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
ThreadsWidget::ShouldRebuild()
	const
{
	return (itsLink->GetFeature(Link::kWillWaitForThread) ||
			(itsThreadDir->IsActive() && !GetWindow()->IsIconified()));
}

/******************************************************************************
 Rebuild (private)

 ******************************************************************************/

void
ThreadsWidget::Rebuild()
{
	if (ShouldRebuild())
	{
		itsIsWaitingForReloadFlag = true;
		itsNeedsUpdateFlag        = false;
		FlushOldData();
		itsGetThreadsCmd->Send();
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
ThreadsWidget::FlushOldData()
{
	itsSelectingThreadFlag = true;
	itsTree->GetRoot()->DeleteAllChildren();
	itsSelectingThreadFlag = false;

	itsNeedsUpdateFlag    = false;
	itsChangingThreadFlag = false;
}

/******************************************************************************
 SaveOpenNodes (private)

 ******************************************************************************/

void
ThreadsWidget::SaveOpenNodes()
{
	if (itsOpenIDList == nullptr)
	{
		itsOpenIDList = jnew JArray<JUInt64>();
		itsOpenIDList->SetCompareFunction(std::function([](const JUInt64& i1, const JUInt64& i2)
		{
			return i1 <=> i2;
		}));
	}
	else
	{
		itsOpenIDList->RemoveAll();
	}

	itsDisplayState = SaveDisplayState();
	CalledBySaveOpenNodes1(itsTree->GetRoot());
}

/******************************************************************************
 CalledBySaveOpenNodes1 (private)

 ******************************************************************************/

void
ThreadsWidget::CalledBySaveOpenNodes1
	(
	JTreeNode* root
	)
{
	JTreeList* list = GetTreeList();

	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		JTreeNode* child = root->GetChild(i);
		if (list->IsOpen(child))
		{
			auto& threadNode = dynamic_cast<ThreadNode&>(*child);
			itsOpenIDList->InsertSorted(threadNode.GetID());
		}

		CalledBySaveOpenNodes1(child);
	}
}

/******************************************************************************
 RestoreOpenNodes (private)

 ******************************************************************************/

void
ThreadsWidget::RestoreOpenNodes
	(
	JTreeNode* root
	)
{
	JTreeList* list = GetTreeList();

	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto& child = dynamic_cast<ThreadNode&>(*root->GetChild(i));
		JIndex j;
		if (itsOpenIDList->SearchSorted(child.GetID(), JListT::kAnyMatch, &j))
		{
			list->Open(&child);
		}

		RestoreOpenNodes(&child);
	}
}

/******************************************************************************
 FinishedLoading

 ******************************************************************************/

void
ThreadsWidget::FinishedLoading
	(
	const JIndex currentID
	)
{
	if (!itsIsWaitingForReloadFlag)		// program exited
	{
		FlushOldData();
		return;
	}

	if (itsOpenIDList != nullptr)
	{
		RestoreOpenNodes(itsTree->GetRoot());
		RestoreDisplayState(itsDisplayState);
	}

	if (currentID > 0)
	{
		SelectThread(currentID, true);
	}

	itsIsWaitingForReloadFlag = false;
}
