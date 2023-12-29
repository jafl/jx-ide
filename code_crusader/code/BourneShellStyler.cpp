/******************************************************************************
 BourneShellStyler.cpp

	Helper object for TextEditor that displays bash with styles to hilight
	keywords, strings, etc.

	BASE CLASS = StylerBase, TextScanner::BourneShell::Scanner

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "BourneShellStyler.h"
#include "PrefsManager.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

BourneShellStyler* BourneShellStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Variable",
	"Reserved keyword",
	"Built-in command",

	"Control operator",
	"Redirection operator",
	"History operator",

	"Single quoted string",
	"Double quoted string",
	"Executed string",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StylerBase*
BourneShellStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew BourneShellStyler;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
BourneShellStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

BourneShellStyler::BourneShellStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::BourneShellStyler"),
				 kBourneShellStyleID, kBourneShellFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	SetTypeStyle(kVariable          - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kReservedWord      - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInCommand    - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kSingleQuoteString - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kExecString        - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kComment           - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kError             - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

BourneShellStyler::~BourneShellStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
BourneShellStyler::Scan
	(
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input,
	const TokenExtra&				initData
	)
{
	BeginScan(GetStyledText(), startIndex, input);

	const JString& text = GetText();

	bool keepGoing;
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

		if (token.type == kID                ||
			token.type == kVariable          ||
			token.type == kReservedWord      ||
			token.type == kSingleQuoteString ||
			token.type == kDoubleQuoteString ||
			token.type == kExecString        ||
			token.type == kComment)
		{
			SaveTokenStart(token.range.GetFirst());
		}

		// handle special cases

		if (token.type == kDoubleQuoteString ||
			token.type == kExecString)
		{
			ExtendCheckRangeForString(token.range);
		}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
		{
			style = GetDefaultFont().GetStyle();
		}
		else if (token.type == kSingleQuoteString ||
				 token.type == kDoubleQuoteString ||
				 token.type == kExecString        ||
				 token.type == kComment)
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

		keepGoing = SetStyle(token.range.charRange, style);

		if (token.type == kDoubleQuoteString ||
			token.type == kExecString)
		{
			StyleEmbeddedVariables(token);
		}
	}
		while (keepGoing);
}

/******************************************************************************
 ExtendCheckRangeForString (private)

	There is one case where modifying a string doesn't force a restyling:

	"x"$x"
	  ^
	Inserting the marked character tricks JTEStyler into not continuing
	because the style didn't change and it was at a style run boundary.

 ******************************************************************************/

void
BourneShellStyler::ExtendCheckRangeForString
	(
	const JStyledText::TextRange& tokenRange
	)
{
	ExtendCheckRange(tokenRange.charRange.last+1);
}

/******************************************************************************
 StyleEmbeddedVariables (private)

	Called after lexing a string to mark variables that will be expanded.

 ******************************************************************************/

#define BourneShellStringID "([[:alpha:]_][[:alnum:]_]*|[0-9]+)"

static JRegex variablePattern(
	"(?<!\\\\)\\$(" BourneShellStringID "|\\{[#!]?" BourneShellStringID "(\\[[^]\n]+\\])?([}:]|#{1,2}|%{1,2}|/{1,2})|[-0-9*@#?$!_])");
static JRegex emptyVariablePattern("(?<!\\\\)\\$\\{\\}?");

#undef BourneShellStringID

#define ClassName    BourneShellStyler
#define VariableType kVariable
#include "STStylerEmbeddedVariables.th"
#undef ClassName
#undef VariableType

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
BourneShellStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
