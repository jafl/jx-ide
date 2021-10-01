/******************************************************************************
 CShellStyler.cpp

	Helper object for TextEditor that displays csh with styles to hilight
	keywords, strings, etc.

	BASE CLASS = StylerBase, TextScanner::CShell::Scanner

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CShellStyler.h"
#include "PrefsManager.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

CShellStyler* CShellStyler::itsSelf = nullptr;

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
	"Math operator",

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
CShellStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew CShellStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CShellStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CShellStyler::CShellStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CShellStyler"),
				 kCShellStyleID, kCShellFT)
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

CShellStyler::~CShellStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CShellStyler::Scan
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
CShellStyler::ExtendCheckRangeForString
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

#define CShellStringID "([?#%]?[[:alpha:]_][[:alnum:]_]{0,19}|%?[0-9]+)"

static JRegex variablePattern      = "(?<!\\\\)\\$(" CShellStringID "|\\{" CShellStringID "[}[]|[*#?$!_<])";
static JRegex emptyVariablePattern = "(?<!\\\\)\\$\\{\\}?";

#undef CShellStringID

#define ClassName CShellStyler
#include "STStylerEmbeddedVariables.th"
#undef ClassName

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CShellStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
