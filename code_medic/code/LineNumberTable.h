/******************************************************************************
 LineNumberTable.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LineNumberTable
#define _H_LineNumberTable

#include "LineIndexTable.h"

class LineNumberTable : public LineIndexTable
{
public:

	LineNumberTable(SourceDirector* dir, SourceText* text,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~LineNumberTable();

protected:

	virtual JColorID	GetCurrentLineMarkerColor() const;
	virtual JString		GetLineText(const JIndex lineIndex) const;
	virtual JString		GetLongestLineText(const JIndex lineCount) const;
	virtual JIndex		GetBreakpointLineIndex(const JIndex bpIndex, const Breakpoint* bp) const;
	virtual bool		GetFirstBreakpointOnLine(const JIndex lineIndex, JIndex* bpIndex) const;
	virtual bool		BreakpointsOnSameLine(const Breakpoint* bp1, const Breakpoint* bp2) const;
	virtual void		GetBreakpoints(JPtrArray<Breakpoint>* list);
	virtual void		SetBreakpoint(const JIndex lineIndex, const bool temporary);
	virtual void		RemoveAllBreakpointsOnLine(const JIndex lineIndex);
	virtual void		RunUntil(const JIndex lineIndex);
	virtual void		SetExecutionPoint(const JIndex lineIndex);

private:

	static JListT::CompareResult
		CompareBreakpointLines(Breakpoint* const & bp1, Breakpoint* const & bp2);
};

#endif
