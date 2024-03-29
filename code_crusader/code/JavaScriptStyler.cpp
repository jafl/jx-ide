/******************************************************************************
 JavaScriptStyler.cpp

	Helper object for TextEditor that displays JavaScript with styles to
	hilight keywords, strings, etc.

	BASE CLASS = StylerBase, TextScanner::JavaScript::Scanner

	Copyright © 2006 by John Lindal.

 ******************************************************************************/

#include "JavaScriptStyler.h"
#include "PrefsManager.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

JavaScriptStyler* JavaScriptStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 2;

	// version 2 adds kTemplateString after kString (5)
	// version 1 adds kRegexSearch after kHexInt (8)

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",

	"Operator",
	"Delimiter",

	"String",
	"Template String",

	"Floating point constant",
	"Decimal constant",
	"Hexadecimal constant",

	"Regular expression",

	"Comment",
	"HTML tag in javadoc comment",
	"@ tag in javadoc comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

static const JUtf8Byte* kUnusedKeyword[] =
{
	"debugger", "goto"
};

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StylerBase*
JavaScriptStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew JavaScriptStyler;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
JavaScriptStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JavaScriptStyler::JavaScriptStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::JavaScriptStyler"),
				 kJavaScriptStyleID, kJavaScriptFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	const JColorID red = JColorManager::GetRedColor();

	SetTypeStyle(kReservedKeyword      - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kString               - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kTemplateString       - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kRegexSearch          - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kComment              - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kDocCommentHTMLTag    - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kDocCommentSpecialTag - kWhitespace, JFontStyle(false, false, 1, false));

	SetTypeStyle(kError                - kWhitespace, JFontStyle(red));

	for (auto keyword : kUnusedKeyword)
	{
		SetWordStyle(JString(keyword, JString::kNoCopy), JFontStyle(red));
	}

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JavaScriptStyler::~JavaScriptStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
JavaScriptStyler::Scan
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

		if (token.type == kID              ||
			token.type == kReservedKeyword ||
			token.type == kString)
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
				 token.type == kString  ||
				 token.type == kRegexSearch)
		{
			style = GetTypeStyle(typeIndex);
		}
		else if (token.type < kWhitespace)
		{
			style = GetTypeStyle(kError - kWhitespace);
		}
		else
		{
			if (token.type == kDocCommentHTMLTag ||
				token.type == kDocCommentSpecialTag)
			{
				if (!(token.docCommentRange).IsEmpty())
				{
					SetStyle(token.docCommentRange.charRange, GetTypeStyle(kComment - kWhitespace));
				}
				ExtendCheckRange(token.range.charRange.last+1);
			}

			style = GetStyle(typeIndex, JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));
		}

		keepGoing = SetStyle(token.range.charRange, style);

		if (token.type == kTemplateString)
		{
			StyleEmbeddedVariables(token);
		}
	}
		while (keepGoing);
}

/******************************************************************************
 StyleEmbeddedVariables (private)

	Called after lexing a string to mark variables that will be expanded.

 ******************************************************************************/

static JRegex variablePattern("(?<!\\\\)\\$\\{.+?\\}");
static JRegex emptyVariablePattern("(?<!\\\\)\\$\\{\\}?");

#define ClassName JavaScriptStyler
#include "STStylerEmbeddedVariables.th"
#undef ClassName

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
JavaScriptStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	if (vers < 1)
	{
		typeStyles->InsertItemAtIndex(9, JFontStyle(JColorManager::GetDarkGreenColor()));
	}

	if (vers < 2)
	{
		typeStyles->InsertItemAtIndex(6, JFontStyle(JColorManager::GetPinkColor()));
	}

	// set new values after all new slots have been created
}
