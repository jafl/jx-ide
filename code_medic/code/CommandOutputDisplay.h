/******************************************************************************
 CommandOutputDisplay.h

	Interface for the CommandOutputDisplay class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CommandOutputDisplay
#define _H_CommandOutputDisplay

#include "TextDisplayBase.h"

class CommandOutputDisplay : public TextDisplayBase
{
public:

	CommandOutputDisplay(JXMenuBar* menuBar,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual	~CommandOutputDisplay();

	void	PlaceCursorAtEnd();
};

#endif
