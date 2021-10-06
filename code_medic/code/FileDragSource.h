/******************************************************************************
 FileDragSource.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileDragSource
#define _H_FileDragSource

#include <jx-af/jx/JXImageWidget.h>

class SourceDirector;

class FileDragSource : public JXImageWidget
{
public:

	FileDragSource(SourceDirector* doc, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~FileDragSource();

protected:

	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	void	HandleDNDResponse(const JXContainer* target,
									  const bool dropAccepted, const Atom action) override;

private:

	SourceDirector*	itsDoc;		// owns us
};

#endif
