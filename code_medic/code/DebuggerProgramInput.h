/******************************************************************************
 DebuggerProgramInput.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_DebuggerProgramInput
#define _H_DebuggerProgramInput

#include <jx-af/jx/JXFileInput.h>

class DebuggerProgramInput : public JXFileInput
{
public:

	DebuggerProgramInput(JXContainer* enclosure,
						   const HSizingOption hSizing, const VSizingOption vSizing,
						   const JCoordinate x, const JCoordinate y,
						   const JCoordinate w, const JCoordinate h);

	virtual ~DebuggerProgramInput();

	virtual bool	InputValid() override;
	virtual bool	GetFile(JString* fullName) const override;

protected:

	class StyledText : public JXFileInput::StyledText
	{
		public:

		StyledText(DebuggerProgramInput* field, JFontManager* fontManager)
			:
			JXFileInput::StyledText(field, fontManager)
		{ };

		protected:

		virtual JSize	ComputeErrorLength(JXFSInputBase* field,
										   const JSize totalLength,
										   const JString& fullName) const override;
	};
};

#endif
