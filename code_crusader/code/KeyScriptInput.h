/******************************************************************************
 KeyScriptInput.h

	Interface for the KeyScriptInput class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_KeyScriptInput
#define _H_KeyScriptInput

#include <jx-af/jx/JXInputField.h>

class KeyScriptInput : public JXInputField
{
public:

	KeyScriptInput(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~KeyScriptInput();

protected:

	class StyledText : public JXInputField::StyledText
	{
		public:

		StyledText(JFontManager* fontManager)
			:
			JXInputField::StyledText(false, fontManager)
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
