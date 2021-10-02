/******************************************************************************
 ArrayExprInput.h

	Interface for the ArrayExprInput class

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_ArrayExprInput
#define _H_ArrayExprInput

#include <jx-af/jx/JXInputField.h>

class ArrayExprInput : public JXInputField
{
public:

	ArrayExprInput(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~ArrayExprInput();

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
