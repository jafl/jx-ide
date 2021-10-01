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

	virtual ~CommandPathInput();

	virtual bool	InputValid();
	virtual bool	GetPath(JString* path) const;
	virtual JString		GetTextForChoosePath() const;

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

		virtual void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;
	};

private:

	// not allowed

	CommandPathInput(const CommandPathInput& source);
	const CommandPathInput& operator=(const CommandPathInput& source);
};

#endif
