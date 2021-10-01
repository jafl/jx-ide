/******************************************************************************
 PythonStyler.cpp

	Helper object for TextEditor that displays Python with styles to hilight
	keywords, preprocessor directives, etc.

	BASE CLASS = StylerBase, TextScanner::Python::Scanner

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#include "PythonStyler.h"
#include "PrefsManager.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

PythonStyler* PythonStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",

	"Operator",
	"Delimiter",

	"String",

	"Floating point constant",
	"Decimal constant",
	"Hexadecimal constant",
	"Octal constant",
	"Imaginary constant",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StylerBase*
PythonStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew PythonStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
PythonStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

PythonStyler::PythonStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::PythonStyler"),
				 kPythonStyleID, kPythonFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	SetTypeStyle(kReservedKeyword - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kString          - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kComment         - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kError           - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PythonStyler::~PythonStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
PythonStyler::Scan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input,
	const TokenExtra&				initData
	)
{
	BeginScan(GetStyledText(), startIndex, input);

	const JString& text = GetText();

	Token token;
	JFontStyle style;
	do
	{
		token = NextToken();
		if (token.type == kEOF)
		{
			break;
		}

		// save token starts

		if (token.type == kID              ||
			token.type == kReservedKeyword ||
			token.type == kString          ||
			token.type == kComment)
		{
			SaveTokenStart(token.range.GetFirst());
		}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
		{
			style = GetDefaultFont().GetStyle();
		}
		else if (token.type == kComment ||
				 token.type == kString)
		{
			style = GetTypeStyle(typeIndex);
		}
		else if (token.type < kWhitespace)
		{
			style = GetTypeStyle(kError - kWhitespace);
		}
		else if (token.type > kError)	// misc
		{
			if (!GetWordStyle(JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy), &style))
			{
				style = GetDefaultFont().GetStyle();
			}
		}
		else
		{
			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));
		}
	}
		while (SetStyle(token.range.charRange, style));
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
PythonStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
