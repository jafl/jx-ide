/******************************************************************************
 sharedUtil.cpp

	Functions used by both Code Crusader and Code Medic.

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "sharedUtil.h"
#include "FnMenuUpdater.h"
#include "CStyler.h"
#include "DStyler.h"
#include "GoStyler.h"
#include "HTMLStyler.h"
#include "JavaStyler.h"

#ifdef CODE_CRUSADER
#include "TextDocument.h"
#include "TextEditor.h"
#endif
#include "globals.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStack.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <editorconfig/editorconfig.h>
#include <jx-af/jcore/jAssert.h>

// shared prefs

static const JString kSharedPrefsFileName(".jxcb/medic_shared_prefs");

const JFileVersion kCurrentSharedPrefsVersion = 7;

// version  7 adds DStyler, GoStyler data and D, Go file suffixes
// version  6 adds HTMLStyler data and PHP file suffixes
// version  5 adds JavaStyler data
// version  4 adds Fortran and Java file suffixes
// version  3 adds WillIncludeNamespace()
// version  2 adds kRightMarginColorIndex to color list
// version  1 adds WillPackFnNames()

// Prototypes

bool	getSharedPrefsFileName(JString* fileName);

#if defined CODE_CRUSADER

/******************************************************************************
 WriteSharedPrefs

 ******************************************************************************/

void
WriteSharedPrefs
	(
	const bool replace
	)
{
	JString fileName;
	if (!getSharedPrefsFileName(&fileName))
	{
		return;
	}

	if (!replace && JFileExists(fileName))
	{
		std::ifstream input(fileName.GetBytes());

		JFileVersion vers;
		input >> vers;
		if (vers >= kCurrentSharedPrefsVersion)
		{
			return;
		}
	}

	JString p,n;
	JSplitPathAndName(fileName, &p, &n);
	if (!JCreateDirectory(p))
	{
		return;
	}

	// don't replace newer prefs

	if (JFileExists(fileName))
	{
		std::ifstream input(fileName.GetBytes());
		JFileVersion vers;
		input >> vers;
		if (vers > kCurrentSharedPrefsVersion)
		{
			return;
		}
	}

	std::ofstream output(fileName.GetBytes());
	output << kCurrentSharedPrefsVersion;

	// font

	PrefsManager* prefsMgr = GetPrefsManager();

	JString fontName;
	JSize fontSize;
	prefsMgr->GetDefaultFont(&fontName, &fontSize);
	output << ' ' << fontName << ' ' << fontSize;

	// editor settings

	auto* doc = jnew TextDocument;

	TextEditor* te         = doc->GetTextEditor();
	FnMenuUpdater* updater = GetFnMenuUpdater();

	output << ' ' << te->GetTabCharCount();
	output << ' ' << JBoolToString(updater->WillSortFnNames());
	output << ' ' << JBoolToString(updater->WillIncludeNamespace());
	output << ' ' << JBoolToString(updater->WillPackFnNames());
	output << ' ' << JBoolToString(doc->WillOpenComplFileOnTop());

	doc->Close();
	doc = nullptr;

	// default text colors

	output << ' ' << (long) PrefsManager::kColorCount;

	for (JIndex i=1; i<=PrefsManager::kColorCount; i++)
	{
		output << ' ' << JColorManager::GetRGB(prefsMgr->GetColor(i));
	}

	// stylers

	output << ' ';
	(CStyler::Instance())->WriteForSharedPrefs(output);

	output << ' ';
	(JavaStyler::Instance())->WriteForSharedPrefs(output);

	output << ' ';
	(HTMLStyler::Instance())->WriteForSharedPrefs(output);

	output << ' ';
	(DStyler::Instance())->WriteForSharedPrefs(output);

	output << ' ';
	(GoStyler::Instance())->WriteForSharedPrefs(output);

	// File suffixes

	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	prefsMgr->GetFileSuffixes(kCSourceFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kCHeaderFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kFortranFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kJavaSourceFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kPHPFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kDFT, &suffixList);
	output << ' ' << suffixList;

	prefsMgr->GetFileSuffixes(kGoFT, &suffixList);
	output << ' ' << suffixList;
}

#elif defined CODE_MEDIC

/******************************************************************************
 ReadSharedPrefs

 ******************************************************************************/

bool
ReadSharedPrefs
	(
	JString*			fontName,
	JSize*				fontSize,
	JSize*				tabCharCount,
	bool*				sortFnNames,
	bool*				includeNS,
	bool*				packFnNames,
	bool*				openComplFileOnTop,
	const JSize			userColorCount,
	JRGB				colorList[],
	JPtrArray<JString>*	cSourceSuffixList,
	JPtrArray<JString>*	cHeaderSuffixList,
	JPtrArray<JString>*	fortranSuffixList,
	JPtrArray<JString>*	javaSuffixList,
	JPtrArray<JString>*	phpSuffixList,
	JPtrArray<JString>*	dSuffixList,
	JPtrArray<JString>*	goSuffixList
	)
{
	JString fileName;
	if (!getSharedPrefsFileName(&fileName))
	{
		return false;
	}

	std::ifstream input(fileName.GetBytes());
	if (!input.good())
	{
		return false;
	}

	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSharedPrefsVersion)
	{
		return false;
	}

	// font

	input >> *fontName >> *fontSize;

	// editor settings

	input >> *tabCharCount >> JBoolFromString(*sortFnNames);

	*includeNS = false;
	if (vers >= 3)
	{
		input >> JBoolFromString(*includeNS);
	}

	*packFnNames = false;
	if (vers >= 1)
	{
		input >> JBoolFromString(*packFnNames);
	}

	input >> JBoolFromString(*openComplFileOnTop);

	// default text colors

	JSize colorCount;
	input >> colorCount;

	assert( userColorCount == 6 );
	if (vers < 2)
	{
		assert( colorCount == 5 );
		colorList[5] = JColorManager::GetRGB(JColorManager::GetGrayColor(70));
	}
	else
	{
		assert( colorCount == userColorCount );
	}

	for (JUnsignedOffset i=0; i<colorCount; i++)
	{
		input >> colorList[i];
	}

	// stylers

	CStyler::Instance()->ReadFromSharedPrefs(input);

	if (vers >= 5)
	{
		JavaStyler::Instance()->ReadFromSharedPrefs(input);
	}

	if (vers >= 6)
	{
		HTMLStyler::Instance()->ReadFromSharedPrefs(input);
	}

	if (vers >= 7)
	{
		DStyler::Instance()->ReadFromSharedPrefs(input);
		GoStyler::Instance()->ReadFromSharedPrefs(input);
	}

	// File suffixes

	input >> *cSourceSuffixList >> *cHeaderSuffixList;

	if (vers >= 4)
	{
		input >> *fortranSuffixList >> *javaSuffixList;
	}
	else
	{
		fortranSuffixList->DeleteAll();
		javaSuffixList->DeleteAll();
	}

	if (vers >= 6)
	{
		input >> *phpSuffixList;
	}
	else
	{
		phpSuffixList->DeleteAll();
	}

	if (vers >= 7)
	{
		input >> *dSuffixList;
		input >> *goSuffixList;
	}
	else
	{
		dSuffixList->DeleteAll();
		goSuffixList->DeleteAll();
	}

	return true;
}

#endif

/******************************************************************************
 getSharedPrefsFileName

	Returns true if the user has a home directory.

 ******************************************************************************/

bool
getSharedPrefsFileName
	(
	JString* fileName
	)
{
	if (JGetPrefsDirectory(fileName))
	{
		*fileName = JCombinePathAndName(*fileName, kSharedPrefsFileName);
		return true;
	}
	else
	{
		fileName->Clear();
		return false;
	}
}

/******************************************************************************
 ParseEditorOptions

	If we ever support indent width separate from tab width:
		emacs: c-basic-offset
		vi:    shiftwidth= (sw)

 ******************************************************************************/

#define EMACS_TAB_WIDTH "tab-width:[ \t]*([0-9]+)"
#define EMACS_TAB_MODE  "indent-tabs-mode:[ \t]*(t|nil)"
#define EMACS_TOP_START "^/\\*+[ \t]*-\\*-.*[ \t]+"
#define EMACS_TOP_END   ".*-\\*-[ \t]*\\*+/"
#define EMACS_START     "^ \\* Local variables:[ \t]*\n(?: \\* .+\n)* \\* "
#define VI_START        "(?:ex|vim?):[ \t]*set.*[ \t]+"
#define VI_END          "(?:[ \t]+.*)?:"

static JRegex emacsTopTabWidthOption(EMACS_TOP_START EMACS_TAB_WIDTH EMACS_TOP_END, "i");
static JRegex emacsTopTabModeOption(EMACS_TOP_START EMACS_TAB_MODE  EMACS_TOP_END, "i");
static JRegex emacsTabWidthOption(EMACS_START EMACS_TAB_WIDTH, "i");
static JRegex emacsTabModeOption(EMACS_START EMACS_TAB_MODE, "i");
static JRegex viTabModeOption(VI_START "(?:tabmode|tm)=(t|s)" VI_END);
static JRegex viTabWidthOption(VI_START "(?:tabstop|ts)=([0-9]+)" VI_END);
static JRegex viAutoIndentOption(VI_START "(autoindent|noautoindent|ai|noai)" VI_END);

void
ParseEditorOptions
	(
	const JString&	fullName,
	const JString&	text,
	bool*			setTabWidth,
	JSize*			tabWidth,
	bool*			setTabMode,
	bool*			tabInsertsSpaces,
	bool*			setAutoIndent,
	bool*			autoIndent
	)
{
	// configure patterns

	emacsTopTabWidthOption.SetSingleLine(true);
	emacsTopTabModeOption.SetSingleLine(true);

	// process file

	*setTabWidth   = false;
	*setTabMode    = false;
	*setAutoIndent = false;

	{
	editorconfig_handle eh = editorconfig_handle_init();
	if (editorconfig_parse(fullName.GetBytes(), eh) == 0)
	{
		const JUtf8Byte *name, *value;

		const int nvCount = editorconfig_handle_get_name_value_count(eh);
		for (int i=0; i<nvCount; i++)
		{
			editorconfig_handle_get_name_value(eh, i, &name, &value);
			if (strcmp(name, "indent_style") == 0)
			{
				*setTabMode       = true;
				*tabInsertsSpaces = strcmp(value, "space") == 0;
			}
		}

		if (*setTabMode)
		{
			for (int i=0; i<nvCount; i++)
			{
				editorconfig_handle_get_name_value(eh, i, &name, &value);
				if (( *tabInsertsSpaces && strcmp(name, "indent_size") == 0) ||
					(!*tabInsertsSpaces && strcmp(name, "tab_width") == 0))
				{
					*setTabWidth = JString::ConvertToUInt(value, tabWidth);
					break;
				}
			}
		}
	}

	editorconfig_handle_destroy(eh);
	}

	const JStringMatch
		emacsTopTabWidthMatch = emacsTopTabWidthOption.Match(text, JRegex::kIncludeSubmatches),
		emacsTabWidthMatch    = emacsTabWidthOption.Match(text, JRegex::kIncludeSubmatches),
		viTabWidthMatch       = viTabWidthOption.Match(text, JRegex::kIncludeSubmatches),
		emacsTopTabModeMatch  = emacsTopTabModeOption.Match(text, JRegex::kIncludeSubmatches),
		emacsTabModeMatch     = emacsTabModeOption.Match(text, JRegex::kIncludeSubmatches),
		viTabModeMatch        = viTabModeOption.Match(text, JRegex::kIncludeSubmatches),
		viAutoIndentMatch     = viAutoIndentOption.Match(text, JRegex::kIncludeSubmatches);

	if (!emacsTopTabWidthMatch.IsEmpty())
	{
		const JString s = emacsTopTabWidthMatch.GetSubstring(1);
		*setTabWidth    = s.ConvertToUInt(tabWidth);
	}
	else if (!emacsTabWidthMatch.IsEmpty())
	{
		const JString s = emacsTabWidthMatch.GetSubstring(1);
		*setTabWidth    = s.ConvertToUInt(tabWidth);
	}
	else if (!viTabWidthMatch.IsEmpty())
	{
		const JString s = viTabWidthMatch.GetSubstring(1);
		*setTabWidth    = s.ConvertToUInt(tabWidth);
	}

	if (!emacsTopTabModeMatch.IsEmpty())
	{
		const JString s   = emacsTopTabModeMatch.GetSubstring(1);
		*setTabMode       = true;
		*tabInsertsSpaces = s == "nil";
	}
	else if (!emacsTabModeMatch.IsEmpty())
	{
		const JString s   = emacsTabModeMatch.GetSubstring(1);
		*setTabMode       = true;
		*tabInsertsSpaces = s == "nil";
	}
	else if (!viTabModeMatch.IsEmpty())
	{
		const JString s   = viTabModeMatch.GetSubstring(1);
		*setTabMode       = true;
		*tabInsertsSpaces = s == "s";
	}

	if (!viAutoIndentMatch.IsEmpty())
	{
		const JString s = viAutoIndentMatch.GetSubstring(1);
		*setAutoIndent  = true;
		*autoIndent     = !s.StartsWith("no");
	}
}

/******************************************************************************
 ScrollForDefinition

	Assumes that the line of the function's definition is selected.

 ******************************************************************************/

static JRegex scriptCommentPattern("^(\n|[ \t]*#)");
static JRegex makeCommentPattern("^(\n|[ \t]*(#|\\.PHONY))", "i");
static JRegex antCommentPattern("^(\n|[ \t]*<!--)");
static JRegex lispCommentPattern("^(\n|[ \t]*;;)");
static JRegex aspCommentPattern("^(\n|[ \t]*')");
static JRegex sqlCommentPattern("^(\n|[ \t]*--)");

inline void
includeScriptComments
	(
	JStringIterator*	iter,
	const JRegex&		pattern
	)
{
	while (!iter->AtBeginning())
	{
		iter->BeginMatch();
		iter->Prev("\n");
		const JStringMatch& m = iter->FinishMatch();
		if (!pattern.Match(m.GetString()))
		{
			iter->Next("\n");
			break;
		}
	}
}

void
ScrollForDefinition
	(
	JXTEBase*		te,
	const Language	lang
	)
{
	JStringIterator* iter = te->GetConstIteratorAtInsertionIndex();
	JUtf8Character c;

	if (lang == kCLang          ||
		lang == kCSharpLang     ||
		lang == kJavaLang       ||
		lang == kJavaScriptLang ||
		lang == kLexLang        ||
		lang == kMatlabLang     ||
		lang == kBisonLang      ||
		lang == kPascalLang     ||
		lang == kPHPLang        ||
		lang == kAdobeFlexLang  ||
		lang == kVeraLang)
	{
		// search backwards for closing brace (end of function) or semicolon (declaration)

		while (iter->Prev(&c) && c != '}' && c != ';') {}

		// find start of following line

		while (iter->Next(&c) && c != '\n') {}
	}
	else if (lang == kAWKLang         ||
			 lang == kPerlLang        ||
			 lang == kPythonLang      ||
			 lang == kRubyLang        ||
			 lang == kBourneShellLang ||
			 lang == kCShellLang      ||
			 lang == kTCLLang         ||
			 lang == kVimLang         ||
			 lang == kSQLLang)
	{
		// search backwards for line that is neither comment nor empty

		includeScriptComments(iter, scriptCommentPattern);
	}
	else if (lang == kMakeLang)
	{
		// search backwards for line that is neither comment nor empty

		includeScriptComments(iter, makeCommentPattern);
	}
	else if (lang == kAntLang)
	{
		// search backwards for line that is neither comment nor empty

		includeScriptComments(iter, antCommentPattern);
	}
	else if (lang == kLispLang)
	{
		// search backwards for line that is neither comment nor empty

		includeScriptComments(iter, lispCommentPattern);
	}
	else if (lang == kASPLang)
	{
		// search backwards for line that is neither comment nor empty

		includeScriptComments(iter, aspCommentPattern);
	}
	else if (lang == kSQLLang)
	{
		// search backwards for line that is neither comment nor empty

		includeScriptComments(iter, sqlCommentPattern);
	}

	// if we moved up without hitting the top of the file,
	// search down again for the next non-blank line

	while (iter->Next(&c) && c == '\n') {}

	// scroll to place this line at the top of the window, if this shifts
	// the original line up - the original line is normally centered in the
	// aperture

	const JCoordinate lineTop =
		te->GetLineTop(te->GetLineForChar(iter->GetPrevCharacterIndex()));

	if (lineTop > te->GetAperture().top)
	{
		te->ScrollTo(0, lineTop);
	}

	te->DisposeConstIterator(iter);
}

/******************************************************************************
 SelectLines

 ******************************************************************************/

void
SelectLines
	(
	JTextEditor*		te,
	const JIndexRange&	lineRange
	)
{
	JIndex lineIndex = te->CRLineIndexToVisualLineIndex(lineRange.first);
	te->SelectLine(lineIndex);

	if (lineRange.last > lineRange.first)
	{
		lineIndex = te->CRLineIndexToVisualLineIndex(lineRange.last);
		te->SetSelection(JCharacterRange(
			te->GetInsertionCharIndex(), te->GetLineCharEnd(lineIndex)));
	}

	if (!te->WillBreakCROnly())
	{
		const JIndex charIndex         = te->GetLineCharStart(lineIndex);
		const JStyledText::TextRange r = te->GetText()->CharToTextRange(nullptr, JCharacterRange(charIndex, charIndex));
		te->SetSelection(JCharacterRange(
			te->GetInsertionCharIndex(), te->GetText()->GetParagraphEnd(r.GetFirst()).charIndex));
	}
}

/******************************************************************************
 BalanceFromSelection

 ******************************************************************************/

void
BalanceFromSelection
	(
	JXTEBase*		te,
	const Language	lang
	)
{
	JCharacterRange sel;
	bool hasSelection = te->GetSelection(&sel);

	JStringIterator openIter(te->GetText()->GetText(), JStringIterator::kStartBeforeChar, te->GetInsertionCharIndex());

	// If a single grouping symbol is enclosed, balance it.

	JUtf8Character c;
	if (hasSelection && sel.first == sel.last)
	{
		openIter.Next(&c, JStringIterator::kStay);
		if (IsOpenGroup(lang, c))
		{
			hasSelection = false;
			openIter.SkipNext();
		}
		else if (IsCloseGroup(lang, c))
		{
			hasSelection = false;
		}
	}
	else if (openIter.Next(&c, JStringIterator::kStay) && IsOpenGroup(lang, c) &&
			 (openIter.AtBeginning() ||
			  (openIter.Prev(&c, JStringIterator::kStay) &&
			   !IsOpenGroup(lang, c) && !IsCloseGroup(lang, c))))
	{
		openIter.SkipNext();
	}
	else if (openIter.Prev(&c, JStringIterator::kStay) && IsCloseGroup(lang, c) &&
			 (openIter.AtEnd() ||
			  (openIter.Next(&c, JStringIterator::kStay) &&
			   !IsOpenGroup(lang, c) && !IsCloseGroup(lang, c))))
	{
		openIter.SkipPrev();
	}

	if (openIter.AtBeginning() || openIter.AtEnd())
	{
		te->GetDisplay()->Beep();
		return;
	}

	const JString s(te->GetText()->GetText(), JString::kNoCopy);
	JStringIterator closeIter(s, JStringIterator::kStartBeforeChar, openIter.GetNextCharacterIndex());

	// balance groupers until the selection is enclosed or we get an error

	while (true)
	{
		JUtf8Character cOpen;
		const bool foundOpen = BalanceBackward(lang, &openIter, &cOpen);

		JUtf8Character cClose;
		const bool foundClose = BalanceForward(lang, &closeIter, &cClose);

		if (foundOpen && foundClose && IsMatchingPair(lang, cOpen, cClose))
		{
			const JIndex openIndex  = openIter.GetNextCharacterIndex();
			const JIndex closeIndex = closeIter.GetPrevCharacterIndex();

			if ((hasSelection &&
				 (sel.first-1 < openIndex || closeIndex < sel.last+1 ||
				  (sel.first-1 == openIndex && closeIndex == sel.last+1))) ||
				(!hasSelection && openIndex+1 == closeIndex))
			{
				// keep going
			}
			else if (openIndex < closeIndex-1)
			{
				te->SetSelection(JCharacterRange(openIndex+1, closeIndex-1));
				break;
			}
			else
			{
				te->SetCaretLocation(closeIndex);
				break;
			}
		}
		else
		{
			te->GetDisplay()->Beep();
			break;
		}
	}
}

/******************************************************************************
 Balancing (private)

	Forward:	Search for the first unbalanced closing paren: ) } ]
				starting from the given index.  Modifies the iterator
				to be after the closing paren.

	Backward:	Search for the first unbalanced opening paren: ( { [
				starting from the character in front of the given index.
				Modifies the iterator to be before the opening paren.

	By not using a C/C++ lexer to balance parentheses, we will display the
	wrong answer when parens are used inside character and string constants.

	However, we gain the ability to balance parens inside all other files,
	including letters to Grandma and other languages such as perl, sh, and
	FORTRAN.  We still make a few mistakes there, too, but the ability to
	get it right 99% of the time in all files is better than 100% of the
	time only in C/C++ files.

	In addition, this method is *much* faster because it is local, while a
	lexer is global because it has to start from the beginning of the file
	every time.

 ******************************************************************************/

bool
BalanceForward
	(
	const Language		lang,
	JStringIterator*	iter,
	JUtf8Character*		c
	)
{
	JStack<JUtf8Byte, JArray<JUtf8Byte> > openList;

	while (iter->Next(c))
	{
		const bool isOpen  = IsOpenGroup(lang, *c);
		const bool isClose = IsCloseGroup(lang, *c);

		if (isOpen)
		{
			openList.Push(c->GetBytes()[0]);
		}
		else if (isClose && openList.IsEmpty())
		{
			return true;
		}
		else if (isClose)
		{
			const JUtf8Byte c1 = openList.Pop();
			if (!IsMatchingPair(lang, JUtf8Character(c1), *c))
			{
				return false;
			}
		}
	}

	return false;
}

bool
BalanceBackward
	(
	const Language		lang,
	JStringIterator*	iter,
	JUtf8Character*		c
	)
{
	JStack<JUtf8Byte, JArray<JUtf8Byte> > closeList;

	while (iter->Prev(c))
	{
		const bool isOpen  = IsOpenGroup(lang, *c);
		const bool isClose = IsCloseGroup(lang, *c);

		if (isClose)
		{
			closeList.Push(c->GetBytes()[0]);
		}
		else if (isOpen && closeList.IsEmpty())
		{
			return true;
		}
		else if (isOpen)
		{
			const JUtf8Byte c1 = closeList.Pop();
			if (!IsMatchingPair(lang, *c, JUtf8Character(c1)))
			{
				return false;
			}
		}
	}

	return false;
}

/******************************************************************************
 IsCharacterInWord

	Returns true if the given character should be considered part of
	a word.  Our definition is [A-Za-z0-9_].

 ******************************************************************************/

bool
IsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return c.IsAlnum() || c == '_';
}
