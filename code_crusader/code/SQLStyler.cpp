/******************************************************************************
 SQLStyler.cpp

	Helper object for TextEditor that displays bash with styles to hilight
	keywords, strings, etc.

	BASE CLASS = StylerBase, TextScanner::SQL::Scanner

	Copyright © 2017 by John Lindal.

 ******************************************************************************/

#include "SQLStyler.h"
#include "PrefsManager.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

SQLStyler* SQLStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 0;

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Variable",
	"Keyword",
	"Reserved word",
	"Built-in function",
	"Extension: MySQL keyword",
	"Extension: PostgreSQL keyword",
	"Extension: Oracle keyword",

	"Operator",
	"Delimiter",
	"Extension: MySQL operator",

	"Single quoted string",
	"Double quoted string",
	"Backtick quoted string",

	"Floating point value",
	"Decimal integer value",
	"Hex integer value",

	"Comment",
	"Extension: MySQL comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StylerBase*
SQLStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew SQLStyler;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
SQLStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SQLStyler::SQLStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::SQLStyler"),
				 kSQLStyleID, kSQLFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	SetTypeStyle(kVariable            - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));
	SetTypeStyle(kKeyword             - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kReservedWord        - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInFunction     - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kSingleQuoteString   - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString   - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kBackQuoteString     - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kComment             - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));

	SetTypeStyle(kExtensionMySQL      - kWhitespace, JFontStyle(JColorManager::GetRedColor()));
	SetTypeStyle(kExtensionPostgreSQL - kWhitespace, JFontStyle(JColorManager::GetRedColor()));
	SetTypeStyle(kExtensionOracle     - kWhitespace, JFontStyle(JColorManager::GetRedColor()));
	SetTypeStyle(kMySQLOperator       - kWhitespace, JFontStyle(JColorManager::GetRedColor()));
	SetTypeStyle(kMySQLComment        - kWhitespace, JFontStyle(JColorManager::GetRedColor()));
	SetTypeStyle(kError               - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SQLStyler::~SQLStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
SQLStyler::Scan
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

		if (token.type == kID                ||
			token.type == kKeyword           ||
			token.type == kReservedWord      ||
			token.type == kBuiltInFunction   ||
			token.type == kSingleQuoteString ||
			token.type == kDoubleQuoteString ||
			token.type == kBackQuoteString   ||
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
		else if (token.type == kSingleQuoteString ||
				 token.type == kDoubleQuoteString ||
				 token.type == kBackQuoteString   ||
				 token.type == kComment)
		{
			style = GetTypeStyle(typeIndex);
		}
		else if (token.type < kWhitespace)
		{
			style = GetTypeStyle(kError - kWhitespace);
		}
		else
		{
			JString word(text.GetRawBytes(), token.range.byteRange);
			word.ToLower();
			style = GetStyle(typeIndex, word);
		}
	}
		while (SetStyle(token.range.charRange, style));
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
SQLStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
