/******************************************************************************
 SampleText.cpp

	BASE CLASS = JXInputField

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "SampleText.h"
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

static const JString kSampleText("I program therefore I am.");
static const JSize kRightMarginWidth = kSampleText.GetCharacterCount();

/******************************************************************************
 Constructor

 ******************************************************************************/

SampleText::SampleText
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	GetText()->SetText(kSampleText);

	itsDrawRightMarginFlag = false;
	itsRightMarginColor    = JColorManager::GetBlackColor();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SampleText::~SampleText()
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
SampleText::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	if (itsDrawRightMarginFlag)
	{
		const JCoordinate x =
			TEGetLeftMarginWidth() +
			(kRightMarginWidth *
			 GetText()->GetDefaultFont().GetCharWidth(
					GetFontManager(), JUtf8Character(' ')));

		const JColorID saveColor = p.GetPenColor();
		p.SetPenColor(itsRightMarginColor);
		p.Line(x, rect.top, x, rect.bottom);
		p.SetPenColor(saveColor);
	}

	JXInputField::Draw(p, rect);
}
