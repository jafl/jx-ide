/******************************************************************************
 FunctionTable.h

	Interface for the FunctionTable class

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_FunctionTable
#define _H_FunctionTable

// Superclass Header
#include <jx-af/jx/JXTable.h>

class Class;

class FunctionTable : public JXTable
{
public:

enum
{
	kFUsed = 1,
	kFReturnType,
	kFFunctionName,
	kFSignature
};

public:

	FunctionTable(Class* list,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	~FunctionTable() override;

	void	HandleKeyPress(const JUtf8Character& c, const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

private:

	Class*	itsList;	// we don't own this.
	bool	itsNeedsAdjustment;

private:

	void	AdjustColumnWidths();
};
#endif

