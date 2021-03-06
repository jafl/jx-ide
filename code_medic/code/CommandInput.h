/******************************************************************************
 CommandInput.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CommandInput
#define _H_CommandInput

#include <jx-af/jx/JXTEBase.h>

class CommandInput : public JXTEBase
{
public:

	CommandInput(JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~CommandInput() override;

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

protected:

	void	HandleFocusEvent() override;
	void	HandleUnfocusEvent() override;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kReturnKeyPressed;
	static const JUtf8Byte* kTabKeyPressed;
	static const JUtf8Byte* kUpArrowKeyPressed;
	static const JUtf8Byte* kDownArrowKeyPressed;

	class ReturnKeyPressed : public JBroadcaster::Message
	{
		public:

			ReturnKeyPressed()
				:
				JBroadcaster::Message(kReturnKeyPressed)
				{ };
	};

	class TabKeyPressed : public JBroadcaster::Message
	{
		public:

			TabKeyPressed()
				:
				JBroadcaster::Message(kTabKeyPressed)
				{ };
	};

	class UpArrowKeyPressed : public JBroadcaster::Message
	{
		public:

			UpArrowKeyPressed()
				:
				JBroadcaster::Message(kUpArrowKeyPressed)
				{ };
	};

	class DownArrowKeyPressed : public JBroadcaster::Message
	{
		public:

			DownArrowKeyPressed()
				:
				JBroadcaster::Message(kDownArrowKeyPressed)
				{ };
	};
};

#endif
