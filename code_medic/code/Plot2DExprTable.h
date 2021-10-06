/******************************************************************************
 Plot2DExprTable.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_Plot2DExprTable
#define _H_Plot2DExprTable

#include <jx-af/jx/JXStringTable.h>

class JXMenuBar;
class JXColHeaderWidget;

class Plot2DExprTable : public JXStringTable
{
public:

	enum
	{
		kXExprColIndex = 1,
		kYExprColIndex,
		kRangeMinColIndex,
		kRangeMaxColIndex,	// = kColCount

		kColCount = kRangeMaxColIndex
	};

public:

	Plot2DExprTable(JXMenuBar* menuBar, JStringTableData* data,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~Plot2DExprTable();

	void	ConfigureColHeader(JXColHeaderWidget* colHeader);

	void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;

	JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h) override;
};

#endif
