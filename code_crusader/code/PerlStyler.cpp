/******************************************************************************
 PerlStyler.cpp

	Helper object for TextEditor that displays Perl with styles to hilight
	keywords, comments, etc.

	BASE CLASS = StylerBase, TextScanner::Perl::Scanner

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#include "PerlStyler.h"
#include "PrefsManager.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

PerlStyler* PerlStyler::itsSelf = nullptr;

const JFileVersion kCurrentTypeListVersion = 3;

	// version 3 inserts kFileGlob after kTransliteration (26)
	// version 2 inserts kReference after kPrototypeArgList (5)
	// version 1 inserts kPrototypeArgList after kSubroutine (4)

static const JUtf8Byte* kTypeNames[] =
{
	"Scalar identifier",
	"List identifier",
	"Hash identifier",
	"Subroutine name (explicit via &)",
	"Subroutine prototype argument list",
	"Reference",
	"Reserved keyword",
	"Bareword",

	"Operator",
	"Delimiter",

	"Single quoted string",
	"Double quoted string",
	"Heredoc string",
	"Executed string",
	"Quoted word list",

	"Floating point constant",
	"Decimal constant",
	"Binary constant",
	"Octal constant",
	"Hexadecimal constant",
	"Version constant",

	"Regex search",
	"Regex replace",
	"One-shot regex search (?...?)",
	"Compiled regex",
	"Transliteration",

	"Readline / File Glob",

	"Comment",
	"POD",
	"Preprocessor directive",
	"Module data",

	"Detectable error"
};

const JSize kTypeCount = sizeof(kTypeNames)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StylerBase*
PerlStyler::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew PerlStyler;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
PerlStyler::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

PerlStyler::PerlStyler()
	:
	StylerBase(kCurrentTypeListVersion, kTypeCount, kTypeNames,
				 JGetString("EditDialogTitle::PerlStyler"),
				 kPerlStyleID, kPerlFT)
{
	JFontStyle blankStyle;
	for (JIndex i=1; i<=kTypeCount; i++)
	{
		SetTypeStyle(i, blankStyle);
	}

	SetTypeStyle(kPrototypeArgList   - kWhitespace, JFontStyle(true, false, 0, false));
	SetTypeStyle(kReservedKeyword    - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kSingleQuoteString  - kWhitespace, JFontStyle(JColorManager::GetBrownColor()));
	SetTypeStyle(kDoubleQuoteString  - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kHereDocString      - kWhitespace, JFontStyle(JColorManager::GetDarkRedColor()));
	SetTypeStyle(kExecString         - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));

	SetTypeStyle(kRegexSearch        - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kRegexReplace       - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kOneShotRegexSearch - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));
	SetTypeStyle(kCompiledRegex      - kWhitespace, JFontStyle(JColorManager::GetPinkColor()));
	SetTypeStyle(kTransliteration    - kWhitespace, JFontStyle(JColorManager::GetLightBlueColor()));
	SetTypeStyle(kFileGlob           - kWhitespace, JFontStyle(JColorManager::GetDarkGreenColor()));

	SetTypeStyle(kComment            - kWhitespace, JFontStyle(JColorManager::GetGrayColor(50)));
	SetTypeStyle(kPOD                - kWhitespace, JFontStyle(true, false, 0, false, JColorManager::GetGrayColor(50)));
	SetTypeStyle(kPPDirective        - kWhitespace, JFontStyle(JColorManager::GetBlueColor()));

	SetTypeStyle(kError              - kWhitespace, JFontStyle(JColorManager::GetRedColor()));

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PerlStyler::~PerlStyler()
{
	JPrefObject::WritePrefs();
	itsSelf = nullptr;
}

/******************************************************************************
 Scan (virtual protected)

 ******************************************************************************/

void
PerlStyler::Scan
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

		// save token starts -- must set itsProbableOperatorFlag

		if (token.type == kScalar            ||
			token.type == kList              ||
			token.type == kHash              ||
			token.type == kSubroutine        ||
			token.type == kReservedKeyword   ||
			token.type == kSingleQuoteString ||
			token.type == kDoubleQuoteString ||
			token.type == kExecString)
		{
			SaveTokenStart(token.range.GetFirst());
		}

		// set the style

		const JIndex typeIndex = token.type - kWhitespace;
		if (token.type == kWhitespace)
		{
			style = GetDefaultFont().GetStyle();
		}
		else if (token.type == kSingleQuoteString  ||
				 token.type == kDoubleQuoteString  ||
				 token.type == kHereDocString      ||
				 token.type == kExecString         ||
				 token.type == kWordList           ||
				 token.type == kRegexSearch        ||
				 token.type == kRegexReplace       ||
				 token.type == kOneShotRegexSearch ||
				 token.type == kCompiledRegex      ||
				 token.type == kTransliteration    ||
				 token.type == kFileGlob           ||
				 token.type == kComment            ||
				 token.type == kPOD                ||
				 token.type == kModuleData)
		{
			style = GetTypeStyle(typeIndex);
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
	This catches "#lin" when typing 'e' and the token start happens
	to be after the #.

	modifiedRange is the range of text that was changed.
	deletion is true if the modification was that text was deleted.

 ******************************************************************************/

void
PerlStyler::PreexpandCheckRange
	(
	const JString&			text,
	const JRunArray<JFont>&	styles,
	const JCharacterRange&	modifiedRange,
	const bool				deletion,
	JStyledText::TextRange*	checkRange
	)
{
	// We have to extend past any whitespace to include the next
	// real token.  This fixes:
	//		= 2 ? ... ?
	//		Delete 2 and then undo.  Forces restyling of range to not be regex.

	JStringIterator iter(text, JStringIterator::kStartBeforeChar, checkRange->charRange.last);
	JUtf8Character c;
	const bool ok = iter.Next(&c, JStringIterator::kStay);
	assert( ok );

	if (!c.IsSpace())
	{
		iter.SkipNext();
	}

	bool foundSpace = false;
	while (iter.Next(&c, JStringIterator::kStay) && c.IsSpace())
	{
		iter.SkipNext();
		foundSpace = true;
	}

	if (foundSpace)
	{
		checkRange->charRange.last = iter.GetPrevCharacterIndex();
		checkRange->byteRange.last = iter.GetPrevByteIndex();
	}
}

/******************************************************************************
 UpgradeTypeList (virtual protected)

 ******************************************************************************/

void
PerlStyler::UpgradeTypeList
	(
	const JFileVersion	vers,
	JArray<JFontStyle>*	typeStyles
	)
{
	if (vers < 1)
	{
		typeStyles->InsertItemAtIndex(5, JFontStyle(true, false, 0, false));
	}

	if (vers < 2)
	{
		typeStyles->InsertItemAtIndex(6, typeStyles->GetItem(1));
	}

	if (vers < 3)
	{
		typeStyles->InsertItemAtIndex(27, JFontStyle(JColorManager::GetDarkGreenColor()));
	}

	// set new values after all new slots have been created
}
