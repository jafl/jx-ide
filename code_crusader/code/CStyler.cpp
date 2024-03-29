/******************************************************************************
 CStyler.cpp

	Helper object for TextEditor that displays C/C++ with styles to hilight
	keywords, preprocessor directives, etc.

	BASE CLASS = StylerBase, TextScanner::C::Scanner

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CStyler.h"
#include "PrefsManager.h"
#include "sharedUtil.h"
#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

CStyler* CStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 3;

	// Remember to increment kCurrentSharedPrefsVersion in sharedUtil.cpp

	// version 1 removes 20 separate categories (11-30) for operators
	// version 2 inserts kBuiltInDataType after kReservedCPPKeyword (4)
	// version 3 inserts kOperator and kDelimiter after kBuiltInDataType (5)

static const JUtf8Byte* kTypeNames[] =
{
	"Identifier",
	"Identifier containing $",
	"Reserved C keyword",
	"Reserved C++ keyword",
	"Built-in data type",

	"Operator",
	"Delimiter",

	"String",
	"Character constant",

	"Floating point constant",
	"Decimal constant",
	"Hexadecimal constant",
	"Octal constant",

	"Comment",
	"Preprocessor directive",

	"Trigraph",
	"Respelling",
	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StylerBase*
CStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew CStyler;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CStyler::CStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::CStyler"),
				 kCStyleID, kCSourceFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	const JColorID red = JColorManager::GetRedColor();

	SetTypeStyle(kDollarID           - kWhitespace, JFontStyle(red));
	SetTypeStyle(kReservedCKeyword   - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kReservedCPPKeyword - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kBuiltInDataType    - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kString             - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kCharConst          - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kPPDirective        - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));

	SetTypeStyle(kTrigraph           - kWhitespace, JFontStyle(red));
	SetTypeStyle(kRespelling         - kWhitespace, JFontStyle(red));
	SetTypeStyle(kError              - kWhitespace, JFontStyle(red));

	SetWordStyle("#pragma",       JFontStyle(red));
	SetWordStyle("#include_next", JFontStyle(red));
	SetWordStyle("goto",          JFontStyle(true, false, 0, false, red));

	JPrefObject::ReadPrefs();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CStyler::~CStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
CStyler::Scan
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

		if (token.type == kID                 ||
			token.type == kReservedCKeyword   ||
			token.type == kReservedCPPKeyword ||
			token.type == kBuiltInDataType    ||
			token.type == kString             ||
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
		else if (token.type == kPPDirective && SlurpPPComment(&(token.range)))
		{
			token.type = kComment;
			style      = GetTypeStyle(kComment - kWhitespace);
		}
		else if (token.type == kPPDirective)
		{
			style = GetStyle(typeIndex, GetPPCommand(text));
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
 PreexpandCheckRange (virtual protected)

	Expand checkRange if the preceding text is #[[:space:]]*
	This catches "#includ" when typing 'e' and the token start happens
	to be after the #.

	modifiedRange is the range of text that was changed.
	deletion is true if the modification was that text was deleted.

 ******************************************************************************/

void
CStyler::PreexpandCheckRange
	(
	const JString&			text,
	const JRunArray<JFont>&	styles,
	const JCharacterRange&	modifiedRange,
	const bool				deletion,
	JStyledText::TextRange*	checkRange
	)
{
	JStringIterator iter(text);
	iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, checkRange->charRange.first, checkRange->byteRange.first);

	JUtf8Character c;
	while (iter.Prev(&c, JStringIterator::kStay) && c.IsSpace())
	{
		iter.SkipPrev();
	}
	if (iter.Prev(&c) && c == '#')
	{
		checkRange->charRange.first = iter.GetNextCharacterIndex();
		checkRange->byteRange.first = iter.GetNextByteIndex();
	}
}

/******************************************************************************
 SlurpPPComment (private)

	Treat "#if 0 ... #(elif|else|endif)" as a comment.

 ******************************************************************************/

static const JRegex ppCommentPattern(
	"^[[:space:]]*(#|%:|\\?\\?=)[[:space:]]*(el)?if[[:space:]]+0[[:space:]]*(//.*|/\\*[^*]*\\*/)?\n");

static const JRegex ppIfPattern("^[[:space:]]*(#|%:|\\?\\?=)[[:space:]]*if");
static const JRegex ppElsePattern("^[[:space:]]*(#|%:|\\?\\?=)[[:space:]]*(else|elif)");
static const JRegex ppEndPattern("^[[:space:]]*(#|%:|\\?\\?=)[[:space:]]*endif");

bool
CStyler::SlurpPPComment
	(
	JStyledText::TextRange* totalRange
	)
{
	const JString& text = GetText();
	const JString s(text.GetRawBytes(), JUtf8ByteRange(GetPPNameRange().byteRange.first, totalRange->byteRange.last), JString::kNoCopy);
	if (!ppCommentPattern.Match(s))
	{
		return false;
	}

	Token token;
	JSize nestCount = 1;
	while (true)
	{
		token = NextToken();
		if (token.type == kEOF)
		{
			break;
		}
		else if (token.type == kPPDirective)
		{
			const JString ppCmd(text.GetRawBytes(), GetPPNameRange().byteRange, JString::kNoCopy);
			if (ppIfPattern.Match(ppCmd))
			{
				nestCount++;
			}
			else if (ppEndPattern.Match(ppCmd))
			{
				nestCount--;
				if (nestCount == 0)
				{
					break;
				}
			}
			else if (ppElsePattern.Match(ppCmd) && nestCount == 1)
			{
				JSize prevCharByteCount;
				const bool ok =
					JUtf8Character::GetPrevCharacterByteCount(
						text.GetRawBytes() + token.range.byteRange.first-2,
						&prevCharByteCount);
				assert( ok );

				Undo(token.range, prevCharByteCount,
					 JString(text.GetRawBytes(), token.range.byteRange, JString::kNoCopy));	// rescan
				token.range.charRange.SetToEmptyAt(token.range.charRange.first);
				token.range.byteRange.SetToEmptyAt(token.range.byteRange.first);
				break;
			}
		}
	}

	totalRange->charRange.last = token.range.charRange.last;
	totalRange->byteRange.last = token.range.byteRange.last;
	return true;
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
CStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	if (vers < 1)
	{
		for (JIndex i=1; i<=20; i++)
		{
			typeStyles->RemoveItem(11);
		}
	}

	if (vers < 2)
	{
		typeStyles->InsertItemAtIndex(5, typeStyles->GetItem(4));
	}

	if (vers < 3)
	{
		const JFontStyle style = typeStyles->GetItem(1);
		typeStyles->InsertItemAtIndex(6, style);
		typeStyles->InsertItemAtIndex(6, style);
	}

	// set new values after all new slots have been created
}

/******************************************************************************
 Receive (virtual protected)

	Update shared prefs after dialog closes.

 ******************************************************************************/

void
CStyler::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	StylerBase::Receive(sender, message);

#if defined CODE_CRUSADER && ! defined CODE_CRUSADER_UNIT_TEST

	if (sender == this && message.Is(kWordListChanged))
	{
		WriteSharedPrefs(true);
	}

#endif
}
