/******************************************************************************
 ProcessText.h

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_ProcessText
#define _H_ProcessText

#include <jx-af/jx/JXStaticText.h>

class ChooseProcessDialog;

class ProcessText : public JXStaticText
{
public:

	ProcessText(ChooseProcessDialog* dir,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual	~ProcessText();

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

private:

	ChooseProcessDialog*	itsDir;
};

#endif
