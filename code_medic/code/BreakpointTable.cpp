/******************************************************************************
 BreakpointTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "BreakpointTable.h"
#include "BreakpointsDir.h"
#include "BreakpointManager.h"
#include "CommandDirector.h"
#include "LineIndexTable.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

enum
{
	kStatusColumn = 1,		// enabled, disabled, temporary, conditional
	kFileNameColumn,
	kLineNumberColumn,
	kFunctionColumn,
	kAddressColumn,
	kIgnoreCountColumn,
	kConditionColumn
};

const JCoordinate kInitColWidth[] =
{
	10, 150, 50, 150, 70, 50, 150
};

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

// geometry information

const JFileVersion kCurrentGeometryDataVersion = 0;
const JUtf8Byte kGeometryDataEndDelimiter      = '\1';

/******************************************************************************
 Constructor

 ******************************************************************************/

BreakpointTable::BreakpointTable
	(
	BreakpointsDir*	dir,
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
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	JPrefObject(GetPrefsManager(), kBreakpointTableID),
	itsDir(dir),
	itsTextInput(nullptr),
	itsFont(JFontManager::GetDefaultFont())
{
	itsBPList = jnew JPtrArray<Breakpoint>(JPtrArrayT::kForgetAll);
	assert(itsBPList != nullptr);
	itsBPList->SetCompareFunction(CompareBreakpointLocations);
	itsBPList->SetSortOrder(JListT::kSortAscending);

	// font

	JString fontName;
	JSize fontSize;
	GetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	itsFont = JFontManager::GetFont(fontName, fontSize);

	const JSize rowHeight = 2*kVMarginWidth + itsFont.GetLineHeight(GetFontManager());
	SetDefaultRowHeight(rowHeight);

	// data

	for (JIndex i=1; i<=kColCount; i++)
	{
		AppendCols(1, kInitColWidth[i-1]);
	}

	SetColWidth(kStatusColumn, rowHeight);

	JPrefObject::ReadPrefs();

	ListenTo(&(GetTableSelection()));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

BreakpointTable::~BreakpointTable()
{
	JPrefObject::WritePrefs();

	jdelete itsBPList;
}

/******************************************************************************
 Update

 ******************************************************************************/

void
BreakpointTable::Update()
{
	BreakpointManager* mgr = GetLink()->GetBreakpointManager();

	// save selected cell

	const JPoint cell = itsSelectedCell;

	// merge lists

	itsBPList->CleanOut();

	const JPtrArray<Breakpoint>& list1 = mgr->GetBreakpoints();

	JSize count = list1.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsBPList->InsertSorted(list1.GetItem(i));
	}

	const JPtrArray<Breakpoint>& list2 = mgr->GetOtherpoints();

	count = list2.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		itsBPList->InsertSorted(list2.GetItem(i));
	}

	// adjust table

	count            = itsBPList->GetItemCount();
	const JSize orig = GetRowCount();
	if (orig < count)
	{
		AppendRows(count - orig);
	}
	else if (orig > count)
	{
		RemoveNextRows(count+1, orig - count);
	}

	// restore selection

	if (cell.y > 0)
	{
		for (JIndex i=1; i<=count; i++)
		{
			if ((itsBPList->GetItem(i))->GetDebuggerIndex() == (JSize) cell.y)
			{
				SelectSingleCell(JPoint(cell.x, i));
				break;
			}
		}
	}

	Refresh();
}

/******************************************************************************
 Show

 ******************************************************************************/

void
BreakpointTable::Show
	(
	const Breakpoint* bp
	)
{
	JIndex i;
	if (FindBreakpointByDebuggerIndex(bp, &i))
	{
		itsDir->Activate();
		GetWindow()->RequestFocus();
		TableScrollToCell(JPoint(kIgnoreCountColumn, i), true);

		JTableSelection& s = GetTableSelection();
		s.ClearSelection();
		s.SelectRow(i);
	}
}

/******************************************************************************
 EditIgnoreCount

 ******************************************************************************/

void
BreakpointTable::EditIgnoreCount
	(
	const Breakpoint* bp
	)
{
	JIndex i;
	if (FindBreakpointByDebuggerIndex(bp, &i))
	{
		itsDir->Activate();
		GetWindow()->RequestFocus();
		BeginEditing(JPoint(kIgnoreCountColumn, i));
	}
}

/******************************************************************************
 EditCondition

 ******************************************************************************/

void
BreakpointTable::EditCondition
	(
	const Breakpoint* bp
	)
{
	JIndex i;
	if (FindBreakpointByDebuggerIndex(bp, &i))
	{
		itsDir->Activate();
		GetWindow()->RequestFocus();
		BeginEditing(JPoint(kConditionColumn, i));
	}
}

/******************************************************************************
 FindBreakpointByDebuggerIndex (private)

 ******************************************************************************/

bool
BreakpointTable::FindBreakpointByDebuggerIndex
	(
	const Breakpoint*	bp,
	JIndex*				index
	)
	const
{
	const JSize count = itsBPList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const Breakpoint* b = itsBPList->GetItem(i);
		if (b->GetDebuggerIndex() == bp->GetDebuggerIndex())
		{
			*index = i;
			return true;
		}
	}

	*index = 0;
	return false;
}

/******************************************************************************
 Receive (virtual protected)

	Listen for changes in our JTableData and JAuxTableData objects.

 ******************************************************************************/

void
BreakpointTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == &(GetTableSelection()) && message.Is(JTableData::kRectChanged))
	{
		if (GetTableSelection().GetSingleSelectedCell(&itsSelectedCell))
		{
			itsSelectedCell.y =
				(itsBPList->GetItem(itsSelectedCell.y))->GetDebuggerIndex();
		}
	}

	JXEditTable::Receive(sender, message);
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

inline JUnsignedOffset
fileNameOffset
	(
	const Breakpoint* bp
	)
{
	JStringIterator iter(bp->GetFileName(), JStringIterator::kStartAtEnd);
	iter.Prev(ACE_DIRECTORY_SEPARATOR_STR);
	return iter.GetNextCharacterIndex();
}

void
BreakpointTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
	{
		return;
	}

	HilightIfSelected(p, cell, rect);

	const Breakpoint* bp = itsBPList->GetItem(cell.y);
	if (cell.x == kStatusColumn)
	{
		JRect r = rect;
		r.Shrink(kHMarginWidth, kHMarginWidth);

		if (bp->GetLineNumber() > 0)
		{
			LineIndexTable::DrawBreakpoint(bp, p, r);
		}
		else
		{
			JPolygon poly;
			poly.AppendItem(JPoint(r.topLeft()));
			poly.AppendItem(JPoint(r.topRight()));
			poly.AppendItem(JPoint(r.xcenter(), r.bottom));

			const JColorID color = bp->IsEnabled() ? JColorManager::GetRedColor() : JColorManager::GetGreenColor();

			p.SetPenColor(color);
			p.SetFilling(true);
			p.Polygon(poly);

			if (bp->GetAction() != Breakpoint::kRemoveBreakpoint)
			{
				p.SetPenColor(JColorManager::GetBlackColor());
				p.SetFilling(false);
				p.Polygon(poly);
			}
		}
	}
	else
	{
		JString s;
		JPainter::HAlign hAlign = JPainter::HAlign::kLeft;
		if (cell.x == kFileNameColumn)
		{
			s.Set(bp->GetFileName().GetBytes() + fileNameOffset(bp));
		}
		else if (cell.x == kLineNumberColumn)
		{
			const JSize line = bp->GetLineNumber();
			if (line > 0)
			{
				s      = JString((JUInt64) line);
				hAlign = JPainter::HAlign::kRight;
			}
		}
		else if (cell.x == kFunctionColumn)
		{
			s = bp->GetFunctionName();
		}
		else if (cell.x == kAddressColumn)
		{
			s = bp->GetAddress();
		}
		else if (cell.x == kIgnoreCountColumn)
		{
			s      = JString((JUInt64) bp->GetIgnoreCount());
			hAlign = JPainter::HAlign::kRight;
		}
		else if (cell.x == kConditionColumn)
		{
			bp->GetCondition(&s);
		}

		p.SetFont(itsFont);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, s, hAlign, JPainter::VAlign::kCenter);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
BreakpointTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (ScrollForWheel(button, modifiers) ||
		!GetCell(pt, &cell))
	{
		return;
	}
	else if (button == kJXLeftButton)
	{
		if (cell.x != kStatusColumn)
		{
			SelectSingleCell(cell);
		}

		Breakpoint* bp = itsBPList->GetItem(cell.y);
		if (cell.x == kStatusColumn)
		{
			GetLink()->SetBreakpointEnabled(bp->GetDebuggerIndex(), !bp->IsEnabled());
		}
		else if (clickCount == 2 &&
				 (cell.x == kFileNameColumn || cell.x == kLineNumberColumn))
		{
			if (bp->GetLineNumber() > 0)
			{
				(itsDir->GetCommandDirector())->OpenSourceFile(bp->GetFileName(), bp->GetLineNumber());
			}
		}
		else if (clickCount == 2 &&
				 (cell.x == kFunctionColumn || cell.x == kAddressColumn))
		{
			if (!bp->GetFunctionName().IsEmpty())
			{
				(itsDir->GetCommandDirector())->DisassembleFunction(bp->GetFunctionName(), bp->GetAddress());
			}
		}
		else if (clickCount == 2)
		{
			BeginEditing(cell);
		}
	}
}

/******************************************************************************
 HandleKeyPress (virtual)

	Backspace/Delete:  clear menu text, menu shortcut, name

 ******************************************************************************/

void
BreakpointTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (IsEditing())
	{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
	}
	else if (c == kJDeleteKey || c == kJForwardDeleteKey)
	{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetSingleSelectedCell(&cell))
		{
			Breakpoint* bp = itsBPList->GetItem(cell.y);
			GetLink()->RemoveBreakpoint(*bp);
		}
	}
	else if (c == kJReturnKey)
	{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (!s.GetSingleSelectedCell(&cell))
		{
			// do nothing
		}
		else if (cell.x == kStatusColumn)
		{
			Breakpoint* bp = itsBPList->GetItem(cell.y);
			GetLink()->SetBreakpointEnabled(bp->GetDebuggerIndex(), !bp->IsEnabled());
		}
		else if (cell.x == kFileNameColumn || cell.x == kLineNumberColumn)
		{
			Breakpoint* bp = itsBPList->GetItem(cell.y);
			if (bp->GetLineNumber() > 0)
			{
				(itsDir->GetCommandDirector())->OpenSourceFile(bp->GetFileName(), bp->GetLineNumber());
			}
		}
		else if (cell.x == kFunctionColumn || cell.x == kAddressColumn)
		{
			Breakpoint* bp = itsBPList->GetItem(cell.y);
			if (!bp->GetFunctionName().IsEmpty())
			{
				(itsDir->GetCommandDirector())->DisassembleFunction(bp->GetFunctionName(), bp->GetAddress());
			}
		}
		else if (cell.x == kIgnoreCountColumn || cell.x == kConditionColumn)
		{
			BeginEditing(cell);
		}
	}
	else
	{
		HandleSelectionKeyPress(c, modifiers);
	}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

bool
BreakpointTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	Breakpoint* bp = itsBPList->GetItem(cell.y);
	return cell.x == kIgnoreCountColumn ||
				 (cell.x == kConditionColumn && bp->GetLineNumber() > 0);
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
BreakpointTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == nullptr );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	Breakpoint* bp = itsBPList->GetItem(cell.y);
	JString text;
	if (cell.x == kIgnoreCountColumn)
	{
		auto* input = jnew JXIntegerInput(this, kFixedLeft, kFixedTop, x,y, w,h);
		input->SetLowerLimit(0);
		input->SetValue(bp->GetIgnoreCount());

		itsTextInput = input;
	}
	else if (cell.x == kConditionColumn)
	{
		bp->GetCondition(&text);
	}

	if (itsTextInput == nullptr)
	{
		itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);
		itsTextInput->GetText()->SetText(text);
	}

	itsTextInput->SetFont(itsFont);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
BreakpointTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != nullptr );

	if (!itsTextInput->InputValid())
	{
		return false;
	}

	Breakpoint* bp = itsBPList->GetItem(cell.y);
	if (cell.x == kIgnoreCountColumn)
	{
		JInteger count;
		const bool ok = dynamic_cast<JXIntegerInput*>(itsTextInput)->GetValue(&count);
		assert( ok );
		if (((JSize) count) != bp->GetIgnoreCount())
		{
			GetLink()->SetBreakpointIgnoreCount(bp->GetDebuggerIndex(), count);
		}
		return true;
	}
	else if (cell.x == kConditionColumn)
	{
		const JString& s = itsTextInput->GetText()->GetText();

		JString cond;
		bp->GetCondition(&cond);
		if (s != cond)
		{
			GetLink()->SetBreakpointCondition(bp->GetDebuggerIndex(), s);
		}
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
BreakpointTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
BreakpointTable::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentGeometryDataVersion)
	{
		JCoordinate w;
		input >> w;
		SetColWidth(kStatusColumn, w);
		input >> w;
		SetColWidth(kFileNameColumn, w);
		input >> w;
		SetColWidth(kLineNumberColumn, w);
		input >> w;
		SetColWidth(kFunctionColumn, w);
		input >> w;
		SetColWidth(kAddressColumn, w);
		input >> w;
		SetColWidth(kIgnoreCountColumn, w);
		input >> w;
		SetColWidth(kConditionColumn, w);
	}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
BreakpointTable::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentGeometryDataVersion;
	output << ' ' << GetColWidth(kStatusColumn);
	output << ' ' << GetColWidth(kFileNameColumn);
	output << ' ' << GetColWidth(kLineNumberColumn);
	output << ' ' << GetColWidth(kFunctionColumn);
	output << ' ' << GetColWidth(kAddressColumn);
	output << ' ' << GetColWidth(kIgnoreCountColumn);
	output << ' ' << GetColWidth(kConditionColumn);
	output << kGeometryDataEndDelimiter;
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
BreakpointTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	widget->SetColumnTitles("BreakpointTable", kColCount);
}

/******************************************************************************
 CompareBreakpointLocations (static)

 ******************************************************************************/

std::weak_ordering
BreakpointTable::CompareBreakpointLocations
	(
	Breakpoint* const & bp1,
	Breakpoint* const & bp2
	)
{
	int r = JString::Compare(
		bp1->GetFileName().GetBytes() + fileNameOffset(bp1),
		bp2->GetFileName().GetBytes() + fileNameOffset(bp2), JString::kIgnoreCase);
	if (r != 0)
	{
		return JIntToWeakOrdering(r);
	}

	std::weak_ordering r1 = bp1->GetLineNumber() <=> bp2->GetLineNumber();
	if (r1 == std::weak_ordering::equivalent)
	{
		r1 = JCompareStringsCaseInsensitive(
				const_cast<JString*>(&(bp1->GetFunctionName())),
				const_cast<JString*>(&(bp2->GetFunctionName())));
	}

	if (r1 == std::weak_ordering::equivalent)
	{
		JString c1, c2;
		bp1->GetCondition(&c1);
		bp2->GetCondition(&c2);

		r1 = JCompareStringsCaseInsensitive(&c1, &c2);
	}

	return r1;
}
