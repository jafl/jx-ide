/******************************************************************************
 ArrayIndexInput.h

	Interface for the ArrayIndexInput class

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_ArrayIndexInput
#define _H_ArrayIndexInput

#include <jx-af/jx/JXIntegerInput.h>

class ArrayIndexInput : public JXIntegerInput
{
public:

	ArrayIndexInput(JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~ArrayIndexInput();

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kReturnKeyPressed;

	class ReturnKeyPressed : public JBroadcaster::Message
		{
		public:

			ReturnKeyPressed()
				:
				JBroadcaster::Message(kReturnKeyPressed)
				{ };
		};
};

#endif
