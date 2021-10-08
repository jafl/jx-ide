/******************************************************************************
 CommandPathInput.h

	Copyright © 2005 by John Lindal.

 ******************************************************************************/

#ifndef _H_CommandPathInput
#define _H_CommandPathInput

#include <jx-af/jx/JXPathInput.h>

class CommandPathInput : public JXPathInput
{
public:

	CommandPathInput(JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	~CommandPathInput() override;

	bool	InputValid() override;
	bool	GetPath(JString* path) const override;
	JString	GetTextForChoosePath() const override;

	static JColorID	GetTextColor(const JString& path, const JString& base,
								 const bool requireWrite);

protected:

	class StyledText : public JXPathInput::StyledText
	{
		public:

		StyledText(CommandPathInput* field, JFontManager* fontManager)
			:
			JXPathInput::StyledText(field, fontManager)
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
