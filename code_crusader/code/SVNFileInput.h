/******************************************************************************
 SVNFileInput.h

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNFileInput
#define _H_SVNFileInput

#include <jx-af/jx/JXFileInput.h>

class SVNFileInput : public JXFileInput
{
public:

	SVNFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~SVNFileInput() override;

protected:

	class StyledText : public JXFileInput::StyledText
	{
		public:

		StyledText(SVNFileInput* field, JFontManager* fontManager)
			:
			JXFileInput::StyledText(field, fontManager)
		{ };

		protected:

		void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;
	};
};

#endif
