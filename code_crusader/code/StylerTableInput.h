/******************************************************************************
 StylerTableInput.h

	Interface for the StylerTableInput class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_StylerTableInput
#define _H_StylerTableInput

#include <jx-af/jx/JXInputField.h>
#include "TextFileType.h"

class StylerTable;

class StylerTableInput : public JXInputField
{
public:

	StylerTableInput(const TextFileType fileType, StylerTable* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	~StylerTableInput();

protected:

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

private:

	StylerTable*	itsStylerTable;		// the table owns us
	TextFileType	itsFileType;
};

#endif
