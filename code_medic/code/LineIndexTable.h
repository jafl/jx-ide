/******************************************************************************
 LineIndexTable.h

	Copyright (C) 2001-16 by John Lindal.

 ******************************************************************************/

#ifndef _H_LineIndexTable
#define _H_LineIndexTable

#include <jx-af/jx/JXTable.h>

class JXTextMenu;
class JXScrollbar;
class Link;
class SourceDirector;
class SourceText;
class Breakpoint;
class DeselectLineTask;

class LineIndexTable : public JXTable
{
	friend class AdjustLineTableToTextTask;
	friend class DeselectLineTask;

public:

	LineIndexTable(JListT::CompareResult (*bpCcompareFn)(Breakpoint *const &, Breakpoint *const &),
					 SourceDirector* dir, SourceText* text,
					 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~LineIndexTable() override;

	JCoordinate	GetCurrentLine() const;
	void		SetCurrentLine(const JIndex line);

	static void	DrawBreakpoint(const Breakpoint* bp, JPainter& p, const JRect& rect);

protected:

	Link*					GetLink() const;
	SourceDirector*			GetDirector() const;
	JPtrArray<Breakpoint>*	GetBreakpointList() const;

	void	TablePrepareToDrawCol(const JIndex colIndex,
										  const JIndex firstRow, const JIndex lastRow) override;
	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

	virtual JColorID	GetCurrentLineMarkerColor() const = 0;
	virtual JString		GetLineText(const JIndex lineIndex) const = 0;
	virtual JString		GetLongestLineText(const JIndex lineCount) const = 0;
	virtual JIndex		GetBreakpointLineIndex(const JIndex bpIndex, const Breakpoint* bp) const = 0;
	virtual bool		GetFirstBreakpointOnLine(const JIndex lineIndex, JIndex* bpIndex) const = 0;
	virtual bool		BreakpointsOnSameLine(const Breakpoint* bp1, const Breakpoint* bp2) const = 0;
	virtual void		GetBreakpoints(JPtrArray<Breakpoint>* list) = 0;
	virtual void		SetBreakpoint(const JIndex lineIndex, const bool temporary) = 0;
	virtual void		RemoveAllBreakpointsOnLine(const JIndex lineIndex) = 0;
	virtual void		RunUntil(const JIndex lineIndex) = 0;
	virtual void		SetExecutionPoint(const JIndex lineIndex) = 0;

private:

	Link*			itsLink;
	SourceDirector*	itsDirector;
	SourceText*		itsText;
	JXScrollbar*	itsVScrollbar;
	JCoordinate		itsCurrentLineIndex;

	JPtrArray<Breakpoint>*	itsBPList;			// contents owned by BreakpointManager
	JIndex					itsBPDrawIndex;		// index of next bp to draw; may be invalid

	JXTextMenu*			itsLineMenu;
	JIndex				itsLineMenuLineIndex;
	bool				itsIsFullLineMenuFlag;
	JIndexRange			itsLineMenuBPRange;
	DeselectLineTask*	itsDeselectTask;

private:

	void	AdjustToText();
	void	UpdateBreakpoints();

	void	DrawBreakpoints(JPainter& p, const JPoint& cell, const JRect& rect);
	bool	FindNextBreakpoint(const JIndex rowIndex, bool* multiple = nullptr);
	bool	HasMultipleBreakpointsOnLine(const JIndex bpIndex) const;

	void	AdjustBreakpoints(const JIndex lineIndex, const JPoint& pt,
							  const JXButtonStates& buttonStates,
							  const JXKeyModifiers& modifiers);
	void	OpenLineMenu(const JIndex lineIndex, const JPoint& pt,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers,
						 const bool onlyBreakpoints,
						 const JIndex firstBPIndex = 0);
	void	UpdateLineMenu();
	void	HandleLineMenu(const JIndex index);
};


/******************************************************************************
 GetLink (protected)

 ******************************************************************************/

inline Link*
LineIndexTable::GetLink()
	const
{
	return itsLink;
}

/******************************************************************************
 GetDirector (protected)

 ******************************************************************************/

inline SourceDirector*
LineIndexTable::GetDirector()
	const
{
	return itsDirector;
}

/******************************************************************************
 GetBreakpointList (protected)

 ******************************************************************************/

inline JPtrArray<Breakpoint>*
LineIndexTable::GetBreakpointList()
	const
{
	return itsBPList;
}

/******************************************************************************
 GetCurrentLine

 ******************************************************************************/

inline JCoordinate
LineIndexTable::GetCurrentLine()
	const
{
	return itsCurrentLineIndex;
}

#endif
