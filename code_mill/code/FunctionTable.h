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
	kFUsed	= 1,
	kFReturnType,
	kFFunctionName,
	kFConst,
	kFArgs
};

public:

	static FunctionTable* Create(Class* list,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	virtual ~FunctionTable();

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	FunctionTable(Class* list,
			JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			const HSizingOption hSizing, const VSizingOption vSizing,
			const JCoordinate x, const JCoordinate y,
			const JCoordinate w, const JCoordinate h);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

private:

	Class*	itsList;	// we don't own this.
	bool		itsNeedsAdjustment;

private:

	void	FunctionTableX();
	void	AdjustColumnWidths();
};
#endif

