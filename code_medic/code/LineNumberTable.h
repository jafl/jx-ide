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

	~LineNumberTable();

protected:

	JColorID	GetCurrentLineMarkerColor() const override;
	JString		GetLineText(const JIndex lineIndex) const override;
	JString		GetLongestLineText(const JIndex lineCount) const override;
	JIndex		GetBreakpointLineIndex(const JIndex bpIndex, const Breakpoint* bp) const override;
	bool		GetFirstBreakpointOnLine(const JIndex lineIndex, JIndex* bpIndex) const override;
	bool		BreakpointsOnSameLine(const Breakpoint* bp1, const Breakpoint* bp2) const override;
	void		GetBreakpoints(JPtrArray<Breakpoint>* list) override;
	void		SetBreakpoint(const JIndex lineIndex, const bool temporary) override;
	void		RemoveAllBreakpointsOnLine(const JIndex lineIndex) override;
	void		RunUntil(const JIndex lineIndex) override;
	void		SetExecutionPoint(const JIndex lineIndex) override;

private:

	static JListT::CompareResult
		CompareBreakpointLines(Breakpoint* const & bp1, Breakpoint* const & bp2);
};

#endif
