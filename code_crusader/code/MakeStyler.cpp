/******************************************************************************
 MakeStyler.cpp

	Helper object for TextEditor that displays Makefile with styles to hilight
	keywords, strings, etc.

	BASE CLASS = StylerBase, TextScanner::Make::Scanner

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "MakeStyler.h"
#include "PrefsManager.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

MakeStyler* MakeStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Make Target",
	"Special Make Target",
	"Make Variable",
	"Make Operator",

	"Shell Variable",
	"Shell Reserved keyword",
	"Shell Built-in command",

	"Shell Control operator",
	"Shell Redirection operator",
	"Shell History operator",

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
MakeStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew MakeStyler;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
MakeStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

MakeStyler::MakeStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				JGetString("EditDialogTitle::MakeStyler"),
				kMakeStyleID, kMakeFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	SetTypeStyle(kMakeTarget          - kWhitespace, JFontStyle(true, false, 0, false));
	SetTypeStyle(kMakeSpecialTarget   - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kMakeVariable        - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kShellVariable       - kWhitespace, JFontStyle(JColorManager::GetLightBlueColor()));
	SetTypeStyle(kShellReservedWord   - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kShellBuiltInCommand - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kSingleQuoteString   - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString   - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kExecString          - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kComment             - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kError               - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MakeStyler::~MakeStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
MakeStyler::Scan
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
			token.type == kMakeTarget        ||
			token.type == kMakeSpecialTarget ||
			token.type == kMakeVariable      ||
			token.type == kShellVariable     ||
			token.type == kShellReservedWord ||
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
MakeStyler::ExtendCheckRangeForString
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

#define ClassName MakeStyler
#define VariableType kShellVariable
#include "STStylerEmbeddedVariables.th"
#undef ClassName
#undef VariableType

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
MakeStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
