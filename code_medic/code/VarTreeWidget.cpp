/******************************************************************************
 VarTreeWidget.cpp

	BASE CLASS = public JXNamedTreeListWidget

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "VarTreeWidget.h"
#include "VarNode.h"
#include "CommandDirector.h"
#include "sharedUtil.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXTextSelection.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JNamedTreeList.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

const JIndex kValueColIndex = 3;
const JCoordinate kHMargin  = 5;
const JSize kIndentWidth    = 2;	// characters

#include "VarTreeWidget-Edit.h"
#include "VarTreeWidget-Base.h"

// Base menu

static const JSize kMenuIndexToBase[] = { 0, 10, 2, 8, 16, 1 };
const JSize kBaseCount                = sizeof(kMenuIndexToBase) / sizeof(JSize);

/******************************************************************************
 Constructor

 *****************************************************************************/

VarTreeWidget::VarTreeWidget
	(
	CommandDirector*	dir,
	const bool			mainDisplay,
	JXMenuBar*			menuBar,
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
	itsDir(dir),
	itsTree(tree),
	itsIsMainDisplayFlag(mainDisplay),
	itsWaitingForReloadFlag(false),
	itsDragType(kInvalidDrag)
{
	SetSelectionBehavior(true, true);

	// custom Edit menu

	JXTEBase* te     = GetEditMenuHandler();
	itsEditMenu      = te->AppendEditMenu(menuBar);
	const bool found = te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &itsCopyPathCmdIndex);
	assert( found );
	itsCopyPathCmdIndex++;
	itsCopyValueCmdIndex = itsCopyPathCmdIndex+1;
	itsEditMenu->InsertMenuItems(itsCopyPathCmdIndex, kEditMenuStr);
	itsEditMenu->AttachHandlers(this,
		&VarTreeWidget::UpdateEditMenu,
		&VarTreeWidget::HandleEditMenu);
	ConfigureEditMenu(itsEditMenu, itsCopyPathCmdIndex-1);

	// Base conversion menus

	itsBaseMenu = menuBar->AppendTextMenu(JGetString("BaseMenuTitle::VarTreeWidget"));
	itsBaseMenu->SetMenuItems(kBaseMenuStr);
	itsBaseMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsBaseMenu);
	ConfigureBaseMenu(itsBaseMenu);

	itsBasePopupMenu =
		jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10, 10);
	itsBasePopupMenu->SetMenuItems(kBaseMenuStr);
	itsBasePopupMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsBasePopupMenu->SetToHiddenPopupMenu();
	ListenTo(itsBasePopupMenu);

	AppendCols(1);
	SetElasticColIndex(0);

	JString name;
	JSize size;
	GetPrefsManager()->GetDefaultFont(&name, &size);
	SetFont(name, size);

	SetIndentWidth(kIndentWidth * GetFont().GetCharWidth(GetFontManager(), JUtf8Character('0')));

	ListenTo(GetLink());
	ListenTo(&GetTableSelection());

	if (mainDisplay)
	{
		ListenTo(GetWindow());
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

VarTreeWidget::~VarTreeWidget()
{
	jdelete itsTree;
}

/******************************************************************************
 NewExpression

 ******************************************************************************/

VarNode*
VarTreeWidget::NewExpression
	(
	const JString& expr
	)
{
	VarNode* node =
		GetLink()->CreateVarNode(GetTreeList()->GetTree()->GetRoot(),
								 expr, expr, JString::empty);
	if (expr.IsEmpty())
	{
		node->SetName(JString::empty);		// compensate for ctor using " " instead of ""
	}

	JIndex i;
	const bool found = GetNamedTreeList()->FindNode(node, &i);
	assert( found );

	const JPoint cell(GetNodeColIndex(), i);
	BeginEditing(cell);
	itsEditingNewNodeFlag = true;

	ClearIncrementalSearchBuffer();
	return node;
}

/******************************************************************************
 DisplayExpression

 ******************************************************************************/

VarNode*
VarTreeWidget::DisplayExpression
	(
	const JString& expr
	)
{
	VarNode* node = GetLink()->CreateVarNode(itsTree->GetRoot(), expr, expr, JString::empty);
	assert ( node != nullptr );
	ShowNode(node);
	return node;
}

/******************************************************************************
 ShowNode

 ******************************************************************************/

void
VarTreeWidget::ShowNode
	(
	const VarNode* node
	)
{
	JIndex i;
	if (GetTreeList()->FindNode(node, &i))
	{
		SelectSingleCell(JPoint(GetNodeColIndex(), i));
		ClearIncrementalSearchBuffer();
	}
}

/******************************************************************************
 WatchExpression

 ******************************************************************************/

void
VarTreeWidget::WatchExpression()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		GetLink()->WatchExpression(expr);
	}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 WatchLocation

 ******************************************************************************/

void
VarTreeWidget::WatchLocation()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		GetLink()->WatchLocation(expr);
	}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 Display1DArray

 ******************************************************************************/

void
VarTreeWidget::Display1DArray()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		itsDir->Display1DArray(expr);
	}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 Plot1DArray

 ******************************************************************************/

void
VarTreeWidget::Plot1DArray()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		itsDir->Plot1DArray(expr);
	}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 Display2DArray

 ******************************************************************************/

void
VarTreeWidget::Display2DArray()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();
		itsDir->Display2DArray(expr);
	}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 ExamineMemory

 ******************************************************************************/

void
VarTreeWidget::ExamineMemory
	(
	const MemoryDir::DisplayType type
	)
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	JString expr;
	MemoryDir* dir = nullptr;
	while (iter.Next(&cell))
	{
		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		expr = node->GetFullName();

		dir = jnew MemoryDir(itsDir, expr);
		dir->SetDisplayType(type);
		dir->Activate();
	}

	if (dir == nullptr)
	{
		dir = jnew MemoryDir(itsDir, JString::empty);
		dir->SetDisplayType(type);
		dir->Activate();
	}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
VarTreeWidget::ReadSetup
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	auto* root = dynamic_cast<VarNode*>(itsTree->GetRoot());
	assert( root != nullptr );

	root->DeleteAllChildren();

	JSize count;
	input >> count;

	JString expr;
	JSize base;
	for (JIndex i=1; i<=count; i++)
	{
		input >> expr >> base;

		VarNode* node = DisplayExpression(expr);
		node->SetBase(base);
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
VarTreeWidget::WriteSetup
	(
	std::ostream& output
	)
	const
{
	auto* root = dynamic_cast<VarNode*>(itsTree->GetRoot());
	assert( root != nullptr );

	const JSize count = root->GetChildCount();
	output << ' ' << count;

	for (JIndex i=1; i<=count; i++)
	{
		VarNode* node = root->GetVarChild(i);
		output << ' ' << node->GetName();
		output << ' ' << node->GetBase();
	}
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

bool
VarTreeWidget::HasSelection()
	const
{
	return GetTableSelection().HasSelection();
}

/******************************************************************************
 RemoveSelection

 ******************************************************************************/

void
VarTreeWidget::RemoveSelection()
{
	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
	{
		JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (node->GetDepth() == 1)
		{
			jdelete node;
		}
	}

	ClearIncrementalSearchBuffer();
}

/******************************************************************************
 IsEditable (virtual)

	Derived classes should override this if there are some cells that
	cannot be edited.

 ******************************************************************************/

bool
VarTreeWidget::IsEditable
	(
	const JPoint& cell
	)
	const
{
	const auto* node =
		dynamic_cast<const VarNode*>(GetTreeList()->GetNode(cell.y));
	assert( node != nullptr );

	return JXTreeListWidget::IsEditable(cell) &&
				((JIndex(cell.x) == GetNodeColIndex() &&
				  itsIsMainDisplayFlag && node->GetDepth() == 1) ||
				 (JIndex(cell.x) == kValueColIndex &&
				  node->ValueIsValid() && !(node->GetValue()).IsEmpty()));
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
VarTreeWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if (JIndex(cell.x) == kValueColIndex)
	{
		const JPoint fakeCell(GetNodeColIndex(), cell.y);
		HilightIfSelected(p, fakeCell, rect);

		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		JFont font = GetFont();
		font.SetStyle(node->GetFontStyle());
		p.SetFont(font);
		p.String(rect, node->GetValue(), JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
	}
	else
	{
		JXNamedTreeListWidget::TableDrawCell(p, cell, rect);
	}
}

/******************************************************************************
 GetMinCellWidth (virtual protected)

 ******************************************************************************/

JSize
VarTreeWidget::GetMinCellWidth
	(
	const JPoint& cell
	)
	const
{
	if (JIndex(cell.x) > GetNodeColIndex())
	{
		const auto* node =
			dynamic_cast<const VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );

		return kHMargin + GetFont().GetStringWidth(GetFontManager(), node->GetValue());
	}
	else
	{
		return JXNamedTreeListWidget::GetMinCellWidth(cell);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
VarTreeWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	ClearIncrementalSearchBuffer();
	itsDragType = kInvalidDrag;

	JPoint cell;
	if (ScrollForWheel(button, modifiers))
	{
		// work has been done
	}
	else if (!GetCell(pt, &cell))
	{
		GetTableSelection().ClearSelection();
	}
	else if (JIndex(cell.x) == GetToggleOpenColIndex())
	{
		itsDragType = kOpenNodeDrag;
		JXNamedTreeListWidget::HandleMouseDown(pt, button, clickCount,
											   buttonStates, modifiers);
	}
	else if (button == kJXRightButton)
	{
		JTableSelection& s = GetTableSelection();
		JPoint fakeCell(GetNodeColIndex(), cell.y);
		if (!s.HasSelection() || !s.IsSelected(fakeCell))
		{
			SelectSingleCell(fakeCell);
		}
		itsBasePopupMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else if (clickCount == 1)
	{
		itsDragType = kSelectDrag;
		cell.x      = GetNodeColIndex();
		BeginSelectionDrag(cell, button, modifiers);
	}
	else if (clickCount == 2 && JIndex(cell.x) == GetNodeColIndex())
	{
		const JTreeNode* node = GetTreeList()->GetNode(cell.y);
		if (itsIsMainDisplayFlag && node->GetDepth() == 1)
		{
			BeginEditing(cell);
		}
		else
		{
			const auto* varNode = dynamic_cast<const VarNode*>(node);
			assert( varNode != nullptr );
			const JString expr = varNode->GetFullNameWithCast();
			if (itsIsMainDisplayFlag)
			{
				NewExpression(expr);	// avoid Activate()
			}
			else
			{
				itsDir->DisplayExpression(expr);
			}
		}
	}
	else if (clickCount == 2 && JIndex(cell.x) == kValueColIndex)
	{
		BeginEditing(cell);
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
VarTreeWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType == kOpenNodeDrag)
	{
		JXNamedTreeListWidget::HandleMouseDrag(pt, buttonStates, modifiers);
	}
	else if (IsDraggingSelection())
	{
		ScrollForDrag(pt);

		JPoint cell;
		const bool ok = GetCell(JPinInRect(pt, GetBounds()), &cell);
		assert( ok );

		cell.x = GetNodeColIndex();
		JTable::ContinueSelectionDrag(cell);
	}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
VarTreeWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsDragType == kOpenNodeDrag)
	{
		JXNamedTreeListWidget::HandleMouseUp(pt, button, buttonStates, modifiers);
	}
	else if (IsDraggingSelection())
	{
		FinishSelectionDrag();
	}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
VarTreeWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&   modifiers
	)
{
	if (c == kJReturnKey)
	{
		EndEditing();
	}

	else if (c == kJForwardDeleteKey || c == kJDeleteKey)
	{
		if (itsIsMainDisplayFlag)
		{
			RemoveSelection();
		}
	}

	else if (c == kJEscapeKey)
	{
		JPoint cell;
		if (GetEditedCell(&cell))
		{
			CancelEditing();
			JTreeNode* node = GetTreeList()->GetNode(cell.y);
			if (itsEditingNewNodeFlag &&
				JIndex(cell.x) == GetNodeColIndex() && node->GetDepth() == 1)
			{
				jdelete node;
			}
		}
	}

	else if ((c == kJUpArrow || c == kJDownArrow) && !IsEditing())
	{
		const bool hasSelection = HasSelection();
		if (!hasSelection && c == kJUpArrow && GetRowCount() > 0)
		{
			SelectSingleCell(JPoint(GetNodeColIndex(), GetRowCount()));
		}
		else if (!hasSelection && c == kJDownArrow && GetRowCount() > 0)
		{
			SelectSingleCell(JPoint(GetNodeColIndex(), 1));
		}
		else
		{
			HandleSelectionKeyPress(c, modifiers);
		}
		ClearIncrementalSearchBuffer();
	}

	else
	{
		JXNamedTreeListWidget::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
VarTreeWidget::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	SelectSingleCell(JPoint(GetNodeColIndex(), cell.y));
	itsEditingNewNodeFlag = false;	// must override after BeginEditing()

	JXInputField* input = JXNamedTreeListWidget::CreateXInputField(cell, x,y, w,h);
	if (JIndex(cell.x) == kValueColIndex)
	{
		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );
		input->GetText()->SetText(node->GetValue());
	}

	itsOrigEditValue = input->GetText()->GetText();

	input->SetIsRequired();
	input->GetText()->SetCharacterInWordFunction(::IsCharacterInWord);
	return input;
}

/******************************************************************************
 ExtractInputData

 ******************************************************************************/

bool
VarTreeWidget::ExtractInputData
	(
	const JPoint& cell
	)
{
	JXInputField* input = nullptr;
	const bool ok = GetXInputField(&input);
	assert( ok );
	const JString& text = input->GetText()->GetText();

	auto* node =
		dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
	assert( node != nullptr );

	if (JIndex(cell.x) == GetNodeColIndex() &&
		JXNamedTreeListWidget::ExtractInputData(cell))
	{
		return true;
	}
	else if (JIndex(cell.x) == kValueColIndex && !text.IsEmpty())
	{
		if (text != itsOrigEditValue)
		{
			const JString name = node->GetFullName();
			GetLink()->SetValue(name, text);
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
VarTreeWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetLink() && message.Is(Link::kProgramFinished))
	{
		FlushOldData();
	}
	else if (sender == GetLink() &&
			 (message.Is(Link::kProgramRunning)       ||
			  message.Is(Link::kFrameChanged)         ||
			  message.Is(Link::kThreadChanged)        ||
			  message.Is(Link::kPrepareToLoadSymbols) ||
			  message.Is(Link::kCoreLoaded)           ||
			  message.Is(Link::kCoreCleared)          ||
			  message.Is(Link::kAttachedToProcess)    ||
			  message.Is(Link::kDetachedFromProcess)  ||
			  (message.Is(Link::kValueChanged) && !IsEditing())))
	{
		CancelEditing();
	}

	else if (sender == GetLink() && message.Is(Link::kDebuggerRestarted))
	{
		itsWaitingForReloadFlag = true;
		CancelEditing();
	}
	else if (sender == GetLink() && message.Is(Link::kDebuggerStarted))
	{
		if (!itsWaitingForReloadFlag)
		{
			(itsTree->GetRoot())->DeleteAllChildren();
		}
		itsWaitingForReloadFlag = false;
	}

	else if (sender == itsBaseMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateBaseMenu(itsBaseMenu);
	}
	else if (sender == itsBasePopupMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateBaseMenu(itsBasePopupMenu);
	}
	else if ((sender == itsBaseMenu || sender == itsBasePopupMenu) &&
			 message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleBaseMenu(selection->GetIndex());
	}

	else if (sender == GetWindow() && message.Is(JXWindow::kIconified))
	{
		ShouldUpdate(false);
	}
	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
	{
		ShouldUpdate(true);
	}

	else
	{
		JTableSelection& s = GetTableSelection();
		if (sender == &s && message.Is(JTableData::kRectChanged))
		{
			// refresh value column to show fake selection

			const auto* selection =
				dynamic_cast<const JTableData::RectChanged*>(&message);
			assert( selection != nullptr );
			JRect r = selection->GetRect();
			r.right = kValueColIndex;
			TableRefreshCellRect(r);
		}

		JXNamedTreeListWidget::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
VarTreeWidget::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (!IsShuttingDown())
	{
		ListenTo(GetLink());
	}

	JXNamedTreeListWidget::ReceiveGoingAway(sender);
}

/******************************************************************************
 ShouldUpdate

 ******************************************************************************/

void
VarTreeWidget::ShouldUpdate
	(
	const bool update
	)
{
	dynamic_cast<VarNode*>(itsTree->GetRoot())->ShouldUpdate(update);
}

/******************************************************************************
 FlushOldData (private)

 ******************************************************************************/

void
VarTreeWidget::FlushOldData()
{
	CancelEditing();

	JTreeNode* root   = itsTree->GetRoot();
	const JSize count = root->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto* child = dynamic_cast<VarNode*>(root->GetChild(i));
		assert( child != nullptr );
		child->DeleteAllChildren();
		child->SetValue(JString::empty);
	}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
VarTreeWidget::UpdateEditMenu()
{
	if (!HasFocus())
	{
		return;
	}

	JXTEBase* te = GetEditMenuHandler();

	JIndex index;
	if (te->EditMenuCmdToIndex(JTextEditor::kSelectAllCmd, &index))
	{
		itsEditMenu->EnableItem(index);
	}

	if (GetTableSelection().HasSelection() &&
		te->EditMenuCmdToIndex(JTextEditor::kCopyCmd, &index))
	{
		assert( itsCopyPathCmdIndex == index+1 );

		itsEditMenu->EnableItem(index);
		itsEditMenu->EnableItem(itsCopyPathCmdIndex);
		itsEditMenu->EnableItem(itsCopyValueCmdIndex);
	}
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
VarTreeWidget::HandleEditMenu
	(
	const JIndex index
	)
{
	if (!HasFocus())
	{
		return;
	}

	ClearIncrementalSearchBuffer();

	JTextEditor::CmdIndex cmd;
	if (!GetEditMenuHandler()->EditMenuIndexToCmd(index, &cmd))
	{
		cmd = JTextEditor::kSeparatorCmd;
	}

	if (cmd == JTextEditor::kCopyCmd)
	{
		CopySelectedItems(false, false);
	}
	else if (index == itsCopyPathCmdIndex)
	{
		CopySelectedItems(true, false);
	}
	else if (index == itsCopyValueCmdIndex)
	{
		CopySelectedItems(false, true);
	}
	else if (cmd == JTextEditor::kSelectAllCmd)
	{
		JTableSelection& s = GetTableSelection();
		s.ClearSelection();
		s.SelectCol(GetNodeColIndex());
	}
}

/******************************************************************************
 CopySelectedItems (private)

 ******************************************************************************/

void
VarTreeWidget::CopySelectedItems
	(
	const bool useFullName,
	const bool copyValue
	)
	const
{
	const JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
	{
		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

		JTableSelectionIterator iter(&s);
		JPoint cell;
		while (iter.Next(&cell))
		{
			const auto* node =
				dynamic_cast<const VarNode*>(GetTreeList()->GetNode(cell.y));
			assert( node != nullptr );
			if (copyValue)
			{
				list.Append(node->GetValue());
			}
			else if (useFullName)
			{
				list.Append(node->GetFullName());
			}
			else
			{
				list.Append(node->GetName());
			}
		}

		auto* data = jnew JXTextSelection(GetDisplay(), list);
		assert( data != nullptr );

		GetSelectionManager()->SetData(kJXClipboardName, data);
	}
}

/******************************************************************************
 UpdateBaseMenu (private)

 ******************************************************************************/

void
VarTreeWidget::UpdateBaseMenu
	(
	JXTextMenu* menu
	)
{
	JInteger base = -1;

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
	{
		const auto* node =
			dynamic_cast<const VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );
		const JInteger b = node->GetBase();

		if (base >= 0 && b != base)
		{
			menu->EnableAll();
			return;
		}
		base = b;
	}

	if (base < 0)
	{
		menu->DisableAll();
	}
	else
	{
		menu->EnableAll();

		for (JUnsignedOffset i=0; i<kBaseCount; i++)
		{
			if (JSize(base) == kMenuIndexToBase[i])
			{
				menu->CheckItem(i+1);
				break;
			}
		}
	}
}

/******************************************************************************
 HandleBaseMenu (private)

 ******************************************************************************/

void
VarTreeWidget::HandleBaseMenu
	(
	const JIndex index
	)
{
	ClearIncrementalSearchBuffer();

	assert( index <= kBaseCount );
	const JSize base = kMenuIndexToBase[index-1];

	JTableSelectionIterator iter(&(GetTableSelection()));
	JPoint cell;
	while (iter.Next(&cell))
	{
		auto* node =
			dynamic_cast<VarNode*>(GetTreeList()->GetNode(cell.y));
		assert( node != nullptr );
		node->SetBase(base);
	}
}
