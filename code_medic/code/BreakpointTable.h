/******************************************************************************
 BreakpointTable.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_BreakpointTable
#define _H_BreakpointTable

#include <jx-af/jx/JXEditTable.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JFont.h>

class JXTextMenu;
class JXInputField;
class JXColHeaderWidget;
class Breakpoint;
class BreakpointsDir;

class BreakpointTable : public JXEditTable, public JPrefObject
{
public:

	BreakpointTable(BreakpointsDir* dir,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~BreakpointTable() override;

	void	SetColTitles(JXColHeaderWidget* widget) const;
	void	Update();
	void	Show(const Breakpoint* bp);
	void	EditIgnoreCount(const Breakpoint* bp);
	void	EditCondition(const Breakpoint* bp);

	bool	IsEditable(const JPoint& cell) const override;
	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
									  const JCoordinate x, const JCoordinate y,
									  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	BreakpointsDir*			itsDir;
	JPtrArray<Breakpoint>*	itsBPList;
	JPoint					itsSelectedCell;	// x=col, y=bpIndex

	JXInputField*	itsTextInput;				// nullptr unless editing
	JFont			itsFont;

private:

	bool	FindBreakpointByDebuggerIndex(const Breakpoint* bp, JIndex* index) const;

	static std::weak_ordering
		CompareBreakpointLocations(Breakpoint* const & bp1,
								   Breakpoint* const & bp2);
};

#endif
