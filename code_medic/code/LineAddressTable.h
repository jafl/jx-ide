/******************************************************************************
 LineAddressTable.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LineAddressTable
#define _H_LineAddressTable

#include "LineIndexTable.h"

class LineAddressTable : public LineIndexTable
{
public:

	LineAddressTable(SourceDirector* dir, SourceText* text,
					   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	~LineAddressTable();

	void	SetLineNumbers(JPtrArray<JString>* list);
	void	ClearLineNumbers();
	bool	FindAddressLineNumber(const JString& address, JIndex* index) const;

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

	JArray<JIndex>*		itsVisualBPIndexList;	// visual line of each breakpoint
	JPtrArray<JString>*	itsLineTextList;		// text for each line

private:

	static JString	BuildAddress(const JString& addr);
	JString			GetLineTextFromAddress(const JString& addr) const;

	static JListT::CompareResult
		CompareBreakpointAddresses(Breakpoint* const & bp1, Breakpoint* const & bp2);
};

#endif
