/******************************************************************************
 ProjectFileInput.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_ProjectFileInput
#define _H_ProjectFileInput

#include <jx-af/jx/JXFileInput.h>

class ProjectFileInput : public JXFileInput
{
public:

	ProjectFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~ProjectFileInput();

protected:

	class StyledText : public JXFileInput::StyledText
	{
		public:

		StyledText(ProjectFileInput* field, JFontManager* fontManager)
			:
			JXFileInput::StyledText(field, fontManager)
		{ };

		protected:

		JSize	ComputeErrorLength(JXFSInputBase* field,
										   const JSize totalLength,
										   const JString& fullName) const override;
	};
};

#endif
