/******************************************************************************
 testUtil.h

	Utility test code.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/jFileUtil.h>
#include <sstream>

inline void
UpdateStyles
	(
	JStyledText*	st,
	StylerBase*	styler
	)
{
	JStyledText::TextRange recalcRange(JStyledText::TextIndex(1,1), st->GetBeyondEnd());
	JStyledText::TextRange redrawRange = recalcRange;

	JArray<JSTStyler::TokenData> tokenStartList;

	styler->UpdateStyles(
		st, st->GetText(), const_cast<JRunArray<JFont>*>(&st->GetStyles()),
		&recalcRange, &redrawRange, false, &tokenStartList);
}

inline void
RunTest
	(
	JStyledText*		st,
	StylerBase*		styler,
	const JUtf8Byte*	inputFilename,
	const JUtf8Byte*	outputFilename
	)
{
	JString text;
	JReadFile(JString(inputFilename), &text);

	st->SetText(text);
	UpdateStyles(st, styler);

	std::ostringstream result;
	st->WritePrivateFormat(result);

	JReadFile(JString(outputFilename), &text);
	JAssertStringsEqual(text, result.str().c_str());
}
