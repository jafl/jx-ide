/******************************************************************************
 GoStyler.cpp

	Helper object for TextEditor that displays Go with styles to
	hilight keywords, strings, etc.

	BASE CLASS = StylerBase, TextScanner::Go::Scanner

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#include "GoStyler.h"
#include "PrefsManager.h"
#include "sharedUtil.h"
#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

GoStyler* GoStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 1;

	// Remember to increment kCurrentSharedPrefsVersion in sharedUtil.cpp

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Reserved keyword",
	"Built-in data type",
	"Built-in function",

	"Operator",
	"Delimiter",

	"String",
	"Raw string",
	"Rune",

	"Floating point constant",
	"Hexadecimal floating point constant",
	"Decimal constant",
	"Binary constant",
	"Octal constant",
	"Hexadecimal constant",
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
GoStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew GoStyler;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
GoStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

GoStyler::GoStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::GoStyler"),
				 kGoStyleID, kGoFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	const JColorID red = JColorManager::GetRedColor();

	SetTypeStyle(kReservedKeyword      - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType      - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInFunction      - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kString               - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kRawString            - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kRune                 - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));

	SetTypeStyle(kComment              - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));

	SetTypeStyle(kError                - kWhitespace, JFontStyle(red));

	SetWordStyle(JString("goto", JString::kNoCopy), JFontStyle(true, false, 0, false, red));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GoStyler::~GoStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
GoStyler::Scan
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
			token.type == kBuiltInFunction ||
			token.type == kString          ||
			token.type == kRawString)
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
				 token.type == kRawString)
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
	}
		while (SetStyle(token.range.charRange, style));
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
GoStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	// set new values after all new slots have been created
}

/******************************************************************************
 Receive (virtual protected)

	Update shared prefs after dialog closes.

 ******************************************************************************/

void
GoStyler::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	StylerBase::Receive(sender, message);

#if defined CODE_CRUSADER && !defined CODE_CRUSADER_UNIT_TEST

	if (message.Is(JXDialogDirector::kDeactivated))
	{
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
		{
			WriteSharedPrefs(true);
		}
	}

#endif
}
