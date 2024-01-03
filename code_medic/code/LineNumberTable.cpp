/******************************************************************************
 LineNumberTable.cpp

	BASE CLASS = LineIndexTable

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LineNumberTable.h"
#include "SourceDirector.h"
#include "BreakpointManager.h"
#include "Link.h"
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LineNumberTable::LineNumberTable
	(
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
	LineIndexTable(CompareBreakpointLines, dir, text, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LineNumberTable::~LineNumberTable()
{
}

/******************************************************************************
 GetCurrentLineMarkerColor (virtual protected)

 ******************************************************************************/

JColorID
LineNumberTable::GetCurrentLineMarkerColor()
	const
{
	return JColorManager::GetCyanColor();
}

/******************************************************************************
 GetLineText (virtual protected)

 ******************************************************************************/

JString
LineNumberTable::GetLineText
	(
	const JIndex lineIndex
	)
	const
{
	return JString((JUInt64) lineIndex);
}

/******************************************************************************
 GetLongestLineText (virtual protected)

 ******************************************************************************/

JString
LineNumberTable::GetLongestLineText
	(
	const JIndex lineCount
	)
	const
{
	return JString((JUInt64) lineCount);
}

/******************************************************************************
 GetBreakpointLineIndex (virtual protected)

 ******************************************************************************/

JIndex
LineNumberTable::GetBreakpointLineIndex
	(
	const JIndex		bpIndex,
	const Breakpoint*	bp
	)
	const
{
	return bp->GetLineNumber();
}

/******************************************************************************
 GetFirstBreakpointOnLine (virtual protected)

 ******************************************************************************/

bool
LineNumberTable::GetFirstBreakpointOnLine
	(
	const JIndex	lineIndex,
	JIndex*			bpIndex
	)
	const
{
	Breakpoint bp(JString::empty, lineIndex);
	return GetBreakpointList()->SearchSorted(&bp, JListT::kFirstMatch, bpIndex);
}

/******************************************************************************
 BreakpointsOnSameLine (virtual protected)

 ******************************************************************************/

bool
LineNumberTable::BreakpointsOnSameLine
	(
	const Breakpoint* bp1,
	const Breakpoint* bp2
	)
	const
{
	return bp1->GetLineNumber() == bp2->GetLineNumber();
}

/******************************************************************************
 GetBreakpoints (virtual protected)

 ******************************************************************************/

void
LineNumberTable::GetBreakpoints
	(
	JPtrArray<Breakpoint>* list
	)
{
	const JString* fullName;
	if (GetDirector()->GetFileName(&fullName))
	{
		GetLink()->GetBreakpointManager()->GetBreakpoints(*fullName, list);
	}
	else
	{
		list->CleanOut();
	}
}

/******************************************************************************
 SetBreakpoint (virtual protected)

 ******************************************************************************/

void
LineNumberTable::SetBreakpoint
	(
	const JIndex	lineIndex,
	const bool	temporary
	)
{
	const JString* fullName;
	const bool hasFile = GetDirector()->GetFileName(&fullName);
	assert( hasFile );
	GetLink()->SetBreakpoint(*fullName, lineIndex, temporary);
}

/******************************************************************************
 RemoveAllBreakpointsOnLine (virtual protected)

 ******************************************************************************/

void
LineNumberTable::RemoveAllBreakpointsOnLine
	(
	const JIndex lineIndex
	)
{
	const JString* fullName;
	const bool hasFile = GetDirector()->GetFileName(&fullName);
	assert( hasFile );
	GetLink()->RemoveAllBreakpointsOnLine(*fullName, lineIndex);
}

/******************************************************************************
 RunUntil (virtual protected)

 ******************************************************************************/

void
LineNumberTable::RunUntil
	(
	const JIndex lineIndex
	)
{
	const JString* fullName;
	const bool hasFile = GetDirector()->GetFileName(&fullName);
	assert( hasFile );
	GetLink()->RunUntil(*fullName, lineIndex);
}

/******************************************************************************
 SetExecutionPoint (virtual protected)

 ******************************************************************************/

void
LineNumberTable::SetExecutionPoint
	(
	const JIndex lineIndex
	)
{
	const JString* fullName;
	const bool hasFile = GetDirector()->GetFileName(&fullName);
	assert( hasFile );
	GetLink()->SetExecutionPoint(*fullName, lineIndex);
}

/******************************************************************************
 CompareBreakpointLines (static private)

 ******************************************************************************/

std::weak_ordering
LineNumberTable::CompareBreakpointLines
	(
	Breakpoint* const & bp1,
	Breakpoint* const & bp2
	)
{
	return bp1->GetLineNumber() <=> bp2->GetLineNumber();
}
