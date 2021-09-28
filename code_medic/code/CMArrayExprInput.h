/******************************************************************************
 CMArrayExprInput.h

	Interface for the CMArrayExprInput class

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMArrayExprInput
#define _H_CMArrayExprInput

#include <jx-af/jx/JXInputField.h>

class CMArrayExprInput : public JXInputField
{
public:

	CMArrayExprInput(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~CMArrayExprInput();

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
