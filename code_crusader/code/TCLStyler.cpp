/******************************************************************************
 TCLStyler.cpp

	Helper object for TextEditor that displays TCL with styles to hilight
	keywords, strings, etc.

	BASE CLASS = StylerBase, TextScanner::TCL::Scanner

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "TCLStyler.h"
#include "PrefsManager.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

TCLStyler* TCLStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JUtf8Byte* kTypeNames[] =
{
	"Predefined word",
	"Variable",
	"String",

	"Comment",

	"Brace: { }",
	"Square bracket: [ ]",
	"Parenthesis: ( )",
	"Semicolon: ;",

	"Other word",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StylerBase*
TCLStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew TCLStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
TCLStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

TCLStyler::TCLStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::TCLStyler"),
				 kTCLStyleID, kTCLFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	SetTypeStyle(kPredefinedWord - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kVariable       - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kString         - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kComment        - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kError          - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TCLStyler::~TCLStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
TCLStyler::Scan
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

		if (token.type == kPredefinedWord ||
			token.type == kOtherWord ||
			token.type == kVariable ||
			token.type == kString ||
			token.type == kComment)
		{
			SaveTokenStart(token.range.GetFirst());
		}

		// handle special cases

		if (token.type == kString ||
			token.type == kUnterminatedString)
		{
			ExtendCheckRangeForString(token.range);
		}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
		{
			style = GetDefaultFont().GetStyle();
		}
		else if (token.type == kComment       ||
				 token.type == kString        ||
				 token.type == kBrace         ||
				 token.type == kSquareBracket ||
				 token.type == kParenthesis)
		{
			style = GetTypeStyle(typeIndex);
		}
		else if (token.type < kWhitespace)
		{
			style = GetTypeStyle(kError - kWhitespace);
		}
		else
		{
			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));
		}

		keepGoing = SetStyle(token.range.charRange, style);

		if (token.type == kString)
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
TCLStyler::ExtendCheckRangeForString
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

static JRegex emptyVariablePattern = "(?<!\\\\)\\$\\{\\}";
static JRegex variablePattern      = "(?<!\\\\)\\$(\\{[^}]+\\}|(_|\\p{L}|\\d)+)";

#define ClassName    TCLStyler
#define VariableType kVariable
#include "STStylerEmbeddedVariables.th"
#undef ClassName
#undef VariableType

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
TCLStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
