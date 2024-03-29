/******************************************************************************
 EiffelStyler.cpp

	Helper object for TextEditor that displays C/C++ with styles to hilight
	keywords, preprocessor directives, etc.

	BASE CLASS = StylerBase, TextScanner::Eiffel::Scanner

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "EiffelStyler.h"
#include "PrefsManager.h"
#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

EiffelStyler* EiffelStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 3;

	// Remember to increment kCurrentSharedPrefsVersion in sharedUtil.cpp

	// version 1 removes 20 separate categories (11-30) for operators
	// version 2 inserts kBuiltInDataType after kReservedCPPKeyword (4)
	// version 3 inserts kOperator and kDelimiter after kBuiltInDataType (5)

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",
	"Built-in data type",

	"Operator",
	"Free operator",
	"Delimiter",

	"String",

	"Floating point constant",
	"Decimal constant",
	"Binary constant",

	"Comment",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StylerBase*
EiffelStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew EiffelStyler;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
EiffelStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

EiffelStyler::EiffelStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::EiffelStyler"),
				 kEiffelStyleID, kEiffelFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	SetTypeStyle(kReservedKeyword - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kString          - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kComment         - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kError           - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

EiffelStyler::~EiffelStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
EiffelStyler::Scan
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
			token.type == kBuiltInDataType ||
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
EiffelStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}
