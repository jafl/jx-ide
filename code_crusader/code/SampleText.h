/******************************************************************************
 SampleText.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_SampleText
#define _H_SampleText

#include <jx-af/jx/JXInputField.h>

class SampleText : public JXInputField
{
public:

	SampleText(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~SampleText();

	void	ShowRightMargin(const bool show, const JColorID color);

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

private:

	bool	itsDrawRightMarginFlag;
	JColorID	itsRightMarginColor;

private:

	// not allowed

	SampleText(const SampleText& source);
	const SampleText& operator=(const SampleText& source);
};


/******************************************************************************
 ShowRightMargin

 ******************************************************************************/

inline void
SampleText::ShowRightMargin
	(
	const bool		show,
	const JColorID	color
	)
{
	itsDrawRightMarginFlag = show;
	itsRightMarginColor    = color;
	Refresh();
}

#endif
