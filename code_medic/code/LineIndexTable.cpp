/******************************************************************************
 LineIndexTable.cpp

	Base class for displaying values corresponding to lines in SourceText.

	BASE CLASS = JXTable

	Copyright (C) 2001-16 by John Lindal.

 ******************************************************************************/

#include "LineIndexTable.h"
#include "SourceDirector.h"
#include "SourceText.h"
#include "CommandDirector.h"
#include "BreakpointsDir.h"
#include "BreakpointTable.h"
#include "BreakpointManager.h"
#include "AdjustLineTableToTextTask.h"
#include "DeselectLineTask.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/jXMenuUtil.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jAssert.h>

enum
{
	kBreakpointColumn = 1,
	kExecPointColumn,
	kLineNumberColumn
};

const JCoordinate kMarginWidth = 2;

// Line menu

static const JUtf8Byte* kLineMenuStr =
	"    Run until execution reaches this line %k Meta-click"
	"  | Set execution point to this line      %k Ctrl-click"
//	"  | Disassemble function containing this line"
	"%l";

enum
{
	kRunUntilCmd = 1,
	kSetExecPtCmd,		// = kLineMenuItemCount
//	kDisasmCmd
};

const JSize kLineMenuItemCount = kSetExecPtCmd;

static const JUtf8Byte* kAllBreakpointsMenuStr =
	"    Set new breakpoint on this line       %k Click (if none)"
	"  | Set temporary breakpoint on this line %k Meta-Shift-click (if none)"
	"  | Remove all breakpoints from this line %k Click (if only one)"
	"%l";

enum
{
	kSetBreakpointCmd = 1,
	kSetTempBreakpointCmd,
	kRemoveAllBreakpointsCmd	// = kAllBreakpointsMenuItemCount
};

const JSize kAllBreakpointsMenuItemCount = kRemoveAllBreakpointsCmd;

static const JUtf8Byte* kBreakpointMenuStr =
	"    Show information on breakpoint #$index"
	"  | Remove breakpoint"
	"  | Set condition"
	"  | Remove condition"
	"  | Enabled %b %k Shift-click (if only one)"
	"  | Ignore the next N times (currently, N=$ignore_count)"
	"%l";

enum
{
	kShowBreakpointInfoCmd = 1,
	kRemoveBreakpointCmd,
	kSetConditionCmd,
	kRemoveConditionCmd,
	kToggleEnableCmd,
	kIgnoreNextNCmd		// = kBreakpointMenuItemCount
};

const JSize kBreakpointMenuItemCount = kIgnoreNextNCmd;

/******************************************************************************
 Constructor

 ******************************************************************************/

LineIndexTable::LineIndexTable
	(
	std::weak_ordering (*bpCcompareFn)(Breakpoint *const &, Breakpoint *const &),

	SourceDirector*	dir,
	SourceText*		text,
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
	JXTable(1, 1, nullptr, enclosure, hSizing, vSizing, x, y, w, h),
	itsDirector(dir),
	itsText(text),
	itsVScrollbar(scrollbarSet->GetVScrollbar()),
	itsCurrentLineIndex(0),
	itsLineMenu(nullptr),
	itsDeselectTask(nullptr)
{
	itsLink = ::GetLink();
	ListenTo(itsLink);

	itsBPList = jnew JPtrArray<Breakpoint>(JPtrArrayT::kForgetAll);
	assert(itsBPList != nullptr);
	itsBPList->SetCompareFunction(bpCcompareFn);
	itsBPList->SetSortOrder(JListT::kSortAscending);

	WantInput(false);
	SetBackColor(GetPrefsManager()->GetColor(PrefsManager::kBackColorIndex));
	SetFocusColor(GetBackColor());
	SetRowBorderInfo(0, GetBackColor());
	SetColBorderInfo(0, GetBackColor());
	SetDrawOrder(kDrawByCol);

	AppendCols(3);

	auto* task = jnew AdjustLineTableToTextTask(this);
	task->Go();

	ListenTo(itsText);
	ListenTo(itsVScrollbar);
	ListenTo(itsLink->GetBreakpointManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LineIndexTable::~LineIndexTable()
{
	jdelete itsBPList;
	jdelete itsDeselectTask;
}

/******************************************************************************
 SetCurrentLine

 ******************************************************************************/

void
LineIndexTable::SetCurrentLine
	(
	const JIndex line
	)
{
	itsCurrentLineIndex = line;
	itsText->GoToLine(line);
	Refresh();
}

/******************************************************************************
 TablePrepareToDrawCol (virtual protected)

 ******************************************************************************/

void
LineIndexTable::TablePrepareToDrawCol
	(
	const JIndex colIndex,
	const JIndex firstRow,
	const JIndex lastRow
	)
{
	itsBPDrawIndex = 1;
	FindNextBreakpoint(firstRow);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
LineIndexTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	if (JIndex(cell.x) == kBreakpointColumn)
	{
		DrawBreakpoints(p, cell, rect);
	}

	else if (JIndex(cell.x) == kExecPointColumn && cell.y == itsCurrentLineIndex)
	{
		// We can't use a static polygon because line heights can vary,
		// e.g. due to underlines.

		const JCoordinate delta = rect.height()/2;

		JPolygon poly;
		poly.AppendItem(JPoint(rect.left+kMarginWidth,       rect.top));
		poly.AppendItem(JPoint(rect.left+kMarginWidth+delta, rect.top + delta));
		poly.AppendItem(JPoint(rect.left+kMarginWidth,       rect.top + 2*delta));

		p.SetPenColor(GetCurrentLineMarkerColor());
		p.SetFilling(true);
		p.Polygon(poly);

		p.SetPenColor(JColorManager::GetBlackColor());
		p.SetFilling(false);
		p.Polygon(poly);
	}

	else if (JIndex(cell.x) == kLineNumberColumn)
	{
		p.SetFont(itsText->GetText()->GetDefaultFont());

		JRect r  = rect;
		r.right -= kMarginWidth;

		const JString str = GetLineText(cell.y);
		p.String(r, str, JPainter::HAlign::kRight);
	}
}

/******************************************************************************
 DrawBreakpoints (private)

 ******************************************************************************/

void
LineIndexTable::DrawBreakpoints
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	// check for breakpoint(s) on this line

	bool hasMultiple;
	if (!FindNextBreakpoint(cell.y, &hasMultiple))
	{
		return;
	}

	// draw breakpoint(s)

	JRect r = rect;
	r.Shrink(kMarginWidth, kMarginWidth);

	if (hasMultiple)
	{
		if (r.height() < 9)		// to allow concentric circles to be distinguished
		{
			r.top    = rect.ycenter() - 4;
			r.bottom = r.top + 9;
			r.left   = rect.xcenter() - 4;
			r.right  = r.left + 9;
		}

		p.Ellipse(r);
		r.Shrink(3, 3);
		p.Ellipse(r);
	}
	else
	{
		DrawBreakpoint(itsBPList->GetItem(itsBPDrawIndex), p, r);
	}
}

/******************************************************************************
 DrawBreakpoint (static)

 ******************************************************************************/

void
LineIndexTable::DrawBreakpoint
	(
	const Breakpoint*	bp,
	JPainter&			p,
	const JRect&		rect
	)
{
	const JColorID color =
		!bp->IsEnabled()   ? JColorManager::GetGreenColor() :
		bp->HasCondition() ? JColorManager::GetYellowColor() : JColorManager::GetRedColor();

	p.SetPenColor(color);
	p.SetFilling(true);
	p.Ellipse(rect);

	if (bp->GetAction() != Breakpoint::kRemoveBreakpoint)
	{
		p.SetPenColor(JColorManager::GetBlackColor());
		p.SetFilling(false);
		p.Ellipse(rect);
	}
}

/******************************************************************************
 FindNextBreakpoint (private)

	Increments itsBPDrawIndex to point to the first breakpoint at or beyond
	the given rowIndex.  Returns true if the breakpoint is on the
	specified line.

	If multiple != nullptr, it it set to true if there is more than one
	breakpoint on the specified line.

 ******************************************************************************/

bool
LineIndexTable::FindNextBreakpoint
	(
	const JIndex	rowIndex,
	bool*		multiple
	)
{
	if (multiple != nullptr)
	{
		*multiple = false;
	}

	while (itsBPList->IndexValid(itsBPDrawIndex))
	{
		Breakpoint* bp = itsBPList->GetItem(itsBPDrawIndex);
		const JIndex i   = itsText->CRLineIndexToVisualLineIndex(GetBreakpointLineIndex(itsBPDrawIndex, bp));
		if (i == rowIndex)
		{
			if (multiple != nullptr && itsBPList->IndexValid(itsBPDrawIndex+1))
			{
				*multiple = BreakpointsOnSameLine(bp, itsBPList->GetItem(itsBPDrawIndex+1));
			}
			return true;
		}
		if (i > rowIndex)
		{
			return false;
		}
		itsBPDrawIndex++;
	}

	return false;
}

/******************************************************************************
 HasMultipleBreakpointsOnLine (private)

 ******************************************************************************/

bool
LineIndexTable::HasMultipleBreakpointsOnLine
	(
	const JIndex bpIndex
	)
	const
{
	if (!itsBPList->IndexValid(bpIndex) ||
		!itsBPList->IndexValid(bpIndex+1))
	{
		return false;
	}

	return BreakpointsOnSameLine(
		itsBPList->GetItem(bpIndex),
		itsBPList->GetItem(bpIndex+1));
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
LineIndexTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (ScrollForWheel(button, modifiers, nullptr, itsVScrollbar))
	{
		return;
	}

	JPoint cell;
	if (!GetCell(pt, &cell))
	{
		return;
	}

	itsText->SelectLine(cell.y);
	const JIndex lineIndex = itsText->VisualLineIndexToCRLineIndex(cell.y);

	if (button == kJXRightButton)
	{
		OpenLineMenu(lineIndex, pt, buttonStates, modifiers, false);
	}
	else if (button == kJXLeftButton &&
			 modifiers.GetState(JXAdjustNMShortcutModifier(kJXControlKeyIndex)) &&
			 !modifiers.shift())
	{
		RunUntil(lineIndex);
	}
	else if (button == kJXLeftButton &&
			 modifiers.GetState(JXAdjustNMShortcutModifier(kJXMetaKeyIndex)))
	{
		SetExecutionPoint(lineIndex);
	}
	else if (button == kJXLeftButton)
	{
		AdjustBreakpoints(lineIndex, pt, buttonStates, modifiers);
	}
}

/******************************************************************************
 AdjustBreakpoints (private)

 ******************************************************************************/

void
LineIndexTable::AdjustBreakpoints
	(
	const JIndex			lineIndex,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JIndex bpIndex;
	if (!GetFirstBreakpointOnLine(lineIndex, &bpIndex))
	{
		SetBreakpoint(lineIndex, modifiers.GetState(JXAdjustNMShortcutModifier(kJXControlKeyIndex)) &&
			modifiers.shift());
	}
	else if (HasMultipleBreakpointsOnLine(bpIndex))
	{
		OpenLineMenu(lineIndex, pt, buttonStates, modifiers, true, bpIndex);
	}
	else if (modifiers.shift())
	{
		(itsBPList->GetItem(bpIndex))->ToggleEnabled();
	}
	else
	{
		itsLink->RemoveBreakpoint(*(itsBPList->GetItem(bpIndex)));
	}
}

/******************************************************************************
 OpenLineMenu (private)

 ******************************************************************************/

void
LineIndexTable::OpenLineMenu
	(
	const JIndex			lineIndex,
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers,
	const bool			onlyBreakpoints,
	const JIndex			firstBPIndex
	)
{
	if (itsLineMenu == nullptr)
	{
		itsLineMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
		itsLineMenu->SetToHiddenPopupMenu(true);
		itsLineMenu->SetUpdateAction(JXMenu::kDisableNone);
		itsLineMenu->AttachHandlers(this,
			&LineIndexTable::UpdateLineMenu,
			&LineIndexTable::HandleLineMenu);
	}

	itsLineMenuLineIndex  = lineIndex;
	itsIsFullLineMenuFlag = !onlyBreakpoints;

	itsLineMenu->RemoveAllItems();
	if (itsIsFullLineMenuFlag)
	{
		itsLineMenu->AppendMenuItems(kLineMenuStr);
	}
	itsLineMenu->AppendMenuItems(kAllBreakpointsMenuStr);

	JIndex bpIndex = firstBPIndex;
	if (bpIndex == 0 && !GetFirstBreakpointOnLine(lineIndex, &bpIndex))
	{
		bpIndex = 0;
	}

	itsLineMenuBPRange.SetToNothing();
	if (itsBPList->IndexValid(bpIndex))
	{
		itsLineMenuBPRange.first = bpIndex;
	}

	while (itsBPList->IndexValid(bpIndex))
	{
		itsLineMenuBPRange.last = bpIndex;
		const Breakpoint* bp  = itsBPList->GetItem(bpIndex);

		const JString bpIndexStr(bp->GetDebuggerIndex(), 0);
		const JString ignoreCountStr(bp->GetIgnoreCount(), 0);
		const JUtf8Byte* map[] =
		{
			"index",        bpIndexStr.GetBytes(),
			"ignore_count", ignoreCountStr.GetBytes()
		};

		JString s(kBreakpointMenuStr);
		JGetStringManager()->Replace(&s, map, sizeof(map));

		itsLineMenu->AppendMenuItems(s.GetBytes());

		if (!HasMultipleBreakpointsOnLine(bpIndex))
		{
			break;
		}
		bpIndex++;
	}

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectRow(lineIndex);
	if (itsDeselectTask == nullptr)
	{
		itsDeselectTask = jnew DeselectLineTask(this);
		itsDeselectTask->Start();
	}

	itsLineMenu->PopUp(this, pt, buttonStates, modifiers);
}

/******************************************************************************
 UpdateLineMenu (private)

 ******************************************************************************/

void
LineIndexTable::UpdateLineMenu()
{
	JSize offset = 0;
	if (itsIsFullLineMenuFlag)
	{
		const bool stopped = itsLink->ProgramIsStopped();
		itsLineMenu->SetItemEnabled(offset + kRunUntilCmd, stopped);
		itsLineMenu->SetItemEnabled(offset + kSetExecPtCmd,
			stopped && itsLink->GetFeature(Link::kSetExecutionPoint));

		offset += kLineMenuItemCount;
	}

	if (itsLineMenuBPRange.IsEmpty())
	{
		itsLineMenu->DisableItem(offset + kRemoveAllBreakpointsCmd);
	}
	offset += kAllBreakpointsMenuItemCount;

	if (!itsLineMenuBPRange.IsNothing())
	{
		for (JIndex i=itsLineMenuBPRange.first; i<=itsLineMenuBPRange.last; i++)
		{
			itsLineMenu->SetItemEnabled(offset + kShowBreakpointInfoCmd,
				itsLink->GetFeature(Link::kShowBreakpointInfo));
			itsLineMenu->SetItemEnabled(offset + kSetConditionCmd,
				itsLink->GetFeature(Link::kSetBreakpointCondition));
			itsLineMenu->SetItemEnabled(offset + kIgnoreNextNCmd,
				itsLink->GetFeature(Link::kSetBreakpointIgnoreCount));

			const Breakpoint* bp = itsBPList->GetItem(i);
			if (!bp->HasCondition())
			{
				itsLineMenu->DisableItem(offset + kRemoveConditionCmd);
			}
			if (bp->IsEnabled())
			{
				itsLineMenu->CheckItem(offset + kToggleEnableCmd);
			}

			offset += kBreakpointMenuItemCount;
		}
	}
}

/******************************************************************************
 HandleLineMenu (private)

 ******************************************************************************/

void
LineIndexTable::HandleLineMenu
	(
	const JIndex index
	)
{
	// line

	JSize offset = 0;
	if (itsIsFullLineMenuFlag)
	{
		if (index == offset + kRunUntilCmd)
		{
			RunUntil(itsLineMenuLineIndex);
			return;
		}
		else if (index == offset + kSetExecPtCmd)
		{
			SetExecutionPoint(itsLineMenuLineIndex);
			return;
		}
		offset += kLineMenuItemCount;
	}

	// all breakpoints

	if (index == offset + kSetBreakpointCmd)
	{
		SetBreakpoint(itsLineMenuLineIndex, false);
		return;
	}
	else if (index == offset + kSetTempBreakpointCmd)
	{
		SetBreakpoint(itsLineMenuLineIndex, true);
		return;
	}
	else if (index == offset + kRemoveAllBreakpointsCmd)
	{
		RemoveAllBreakpointsOnLine(itsLineMenuLineIndex);
		return;
	}
	offset += kAllBreakpointsMenuItemCount;

	// individual breakpoints

	if (!itsLineMenuBPRange.IsNothing())
	{
		for (JIndex i=itsLineMenuBPRange.first; i<=itsLineMenuBPRange.last; i++)
		{
			Breakpoint* bp = itsBPList->GetItem(i);
			if (index == offset + kShowBreakpointInfoCmd)
			{
				itsDirector->GetCommandDirector()->GetBreakpointsDir()->GetBreakpointTable()->Show(bp);
				return;
			}
			else if (index == offset + kRemoveBreakpointCmd)
			{
				itsLink->RemoveBreakpoint(*bp);
				return;
			}
			else if (index == offset + kSetConditionCmd)
			{
				itsDirector->GetCommandDirector()->GetBreakpointsDir()->GetBreakpointTable()->EditCondition(bp);
				return;
			}
			else if (index == offset + kRemoveConditionCmd)
			{
				bp->RemoveCondition();
				return;
			}
			else if (index == offset + kToggleEnableCmd)
			{
				bp->ToggleEnabled();
				return;
			}
			else if (index == offset + kIgnoreNextNCmd)
			{
				(itsDirector->GetCommandDirector()->GetBreakpointsDir()->GetBreakpointTable())->EditIgnoreCount(bp);
				return;
			}

			offset += kBreakpointMenuItemCount;
		}
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
LineIndexTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsText &&
		(message.Is(JStyledText::kTextChanged) ||
		 message.Is(JStyledText::kTextSet)))
	{
		AdjustToText();
	}

	else if (sender == itsVScrollbar && message.Is(JXScrollbar::kScrolled))
	{
		const auto* info =
			dynamic_cast<const JXScrollbar::Scrolled*>(&message);
		assert( info != nullptr );
		ScrollTo(0, info->GetValue());
	}

	else if (sender == itsLink && message.Is(Link::kProgramRunning))
	{
		itsCurrentLineIndex = 0;
		Refresh();
	}
	else if (sender == itsLink->GetBreakpointManager() &&
			 message.Is(BreakpointManager::kBreakpointsChanged))
	{
		UpdateBreakpoints();
	}

	else
	{
		JXTable::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
LineIndexTable::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		itsLink = ::GetLink();
		ListenTo(itsLink);
		ListenTo(itsLink->GetBreakpointManager());

		itsBPList->RemoveAll();
	}
	else
	{
		JXTable::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 AdjustToText (private)

 ******************************************************************************/

void
LineIndexTable::AdjustToText()
{
	JFontManager* fontMgr = GetFontManager();

	JFont font = itsText->GetCurrentFont();

	const JSize lineCount       = itsText->GetText()->IsEmpty() ? 0 : itsText->GetLineCount();
	const JString lineCountStr  = GetLongestLineText(lineCount);
	const JSize lineHeight      = font.GetLineHeight(fontMgr);
	const JSize lineNumberWidth = font.GetStringWidth(fontMgr, lineCountStr);

	SetColWidth(kBreakpointColumn, lineHeight);
	SetColWidth(kExecPointColumn,  lineHeight);
	SetColWidth(kLineNumberColumn, lineNumberWidth + 2*kMarginWidth);

	SetAllRowHeights(lineHeight);
	const JSize origRowCount = GetRowCount();
	if (origRowCount < lineCount)
	{
		AppendRows(lineCount - origRowCount, lineHeight);
	}
	else if (origRowCount > lineCount)
	{
		RemovePrevRows(origRowCount, origRowCount - lineCount);
	}

	const JCoordinate tableWidth = GetBoundsWidth();
	const JCoordinate apWidth    = GetApertureWidth();
	if (tableWidth != apWidth)
	{
		AdjustSize(tableWidth-apWidth, 0);
		itsText->Place(GetFrameWidth(), 0);
		itsText->AdjustSize(apWidth-tableWidth, 0);
	}

	ScrollTo(0, itsVScrollbar->GetValue());
	UpdateBreakpoints();
}

/******************************************************************************
 UpdateBreakpoints (private)

 ******************************************************************************/

void
LineIndexTable::UpdateBreakpoints()
{
	itsBPList->RemoveAll();
	GetBreakpoints(itsBPList);
	Refresh();
}
