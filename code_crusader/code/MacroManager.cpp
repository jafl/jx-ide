/******************************************************************************
 MacroManager.cpp

	Stores a list of macro strings and associated actions.  The action is
	performed after the string is typed followed by a tab.

	Macros are specified by plain text, not regular expressions, because
	each macro should have a unique name.  Anything more complicated should
	be done with regex search & replace.

	BASE CLASS = none

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "MacroManager.h"
#include "MacroSubstitute.h"
#include "TextDocument.h"
#include "TextEditor.h"
#include "sharedUtil.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

static MacroSubstitute theSubst;

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version 1 no longer stores dialog geometry or column width

/******************************************************************************
 Constructor

 ******************************************************************************/

MacroManager::MacroManager()
{
	MacroManagerX();
}

// private

void
MacroManager::MacroManagerX()
{
	itsMacroList = jnew MacroList;
	assert( itsMacroList != nullptr );
	itsMacroList->SetSortOrder(JListT::kSortAscending);
	itsMacroList->SetCompareFunction(CompareMacros);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

MacroManager::MacroManager
	(
	const MacroManager& source
	)
{
	MacroManagerX();

	const JSize count = (source.itsMacroList)->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const MacroInfo oldInfo = (source.itsMacroList)->GetElement(i);

		MacroInfo newInfo(jnew JString(*(oldInfo.macro)),
						  jnew JString(*(oldInfo.script)));
		assert( newInfo.macro != nullptr && newInfo.script != nullptr );
		itsMacroList->AppendElement(newInfo);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MacroManager::~MacroManager()
{
	itsMacroList->DeleteAll();
	jdelete itsMacroList;
}

/******************************************************************************
 Perform

	Returns true if it found a script and performed it.

 ******************************************************************************/

bool
MacroManager::Perform
	(
	const JStyledText::TextIndex&	caretIndex,
	TextDocument*					doc
	)
{
	if (caretIndex.charIndex <= 1)
	{
		return false;
	}

	const JStyledText* st = doc->GetTextEditor()->GetText();
	const JUtf8Byte* text = st->GetText().GetBytes();

	const JSize macroCount = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=macroCount; i++)
	{
		const MacroInfo info = itsMacroList->GetElement(i);
		if (info.macro->GetCharacterCount() <= caretIndex.charIndex-1)
		{
			const JStyledText::TextIndex j =
				st->AdjustTextIndex(caretIndex, -(JInteger)info.macro->GetCharacterCount());

			if (info.macro->GetByteCount() == caretIndex.byteIndex - j.byteIndex &&
				JString::CompareMaxNBytes(text + j.byteIndex-1, info.macro->GetBytes(),
										  info.macro->GetByteCount()) == 0)
			{
				JStringIterator iter(st->GetText());
				iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, j.charIndex, j.byteIndex);

				JUtf8Character c;
				if (iter.AtBeginning() ||
					(iter.Next(&c, JStringIterator::kStay) && !IsCharacterInWord(c)) ||
					(iter.Prev(&c, JStringIterator::kStay) && !IsCharacterInWord(c)))
				{
					iter.Invalidate();
					Perform(*info.script, doc);
					return true;
				}
			}
		}
	}

	return false;
}

/******************************************************************************
 Perform (static)

 ******************************************************************************/

void
MacroManager::Perform
	(
	const JString&	script,
	TextDocument*	doc
	)
{
	TextEditor* te = doc->GetTextEditor();
	te->GetText()->DeactivateCurrentUndo();

	bool onDisk;
	const JString fullName = JPrepArgForExec(doc->GetFullName(&onDisk));
	const JString path     = JPrepArgForExec(doc->GetFilePath());
	const JString fileName = JPrepArgForExec(doc->GetFileName());

	JString root, suffix;
	if (JSplitRootAndSuffix(doc->GetFileName(), &root, &suffix))
	{
		root = JPrepArgForExec(root);
		suffix.Prepend(".");
	}

	const JIndex charIndex = te->GetInsertionCharIndex();
	const JIndex lineIndex = te->GetLineForChar(charIndex);
	const JIndex lineStart = te->GetLineCharStart(lineIndex);

	const JString lineIndexStr((JUInt64) lineIndex);

	JString lineStr;
	if (charIndex > lineStart)
	{
		JStringIterator iter(te->GetText()->GetText(), JStringIterator::kStartBeforeChar, lineStart);
		iter.BeginMatch();
		iter.MoveTo(JStringIterator::kStartBeforeChar, charIndex);
		lineStr = JPrepArgForExec(iter.FinishMatch().GetString());
	}

	theSubst.UndefineAllVariables();
	theSubst.DefineVariable("f", fullName);
	theSubst.DefineVariable("p", path);
	theSubst.DefineVariable("n", fileName);
	theSubst.DefineVariable("r", root);
	theSubst.DefineVariable("s", suffix);
	theSubst.DefineVariable("l", lineIndexStr);
	theSubst.DefineVariable("t", lineStr);

	JString s = script;
	theSubst.Substitute(&s);

	JXKeyModifiers modifiers(te->GetDisplay());

	JStringIterator iter(s);
	JUtf8Character c;
	while (iter.Next(&c))
	{
		te->HandleKeyPress(c, 0, modifiers);
	}
}

/******************************************************************************
 HighlightErrors (static)

 ******************************************************************************/

void
MacroManager::HighlightErrors
	(
	const JString&		script,
	JRunArray<JFont>*	styles
	)
{
	const JFont f = styles->GetFirstElement();
	styles->RemoveAll();

	JFont black = f;
	black.SetColor(JColorManager::GetBlackColor());

	JFont blue = f;
	blue.SetColor(JColorManager::GetBlueColor());

	JFont red = f;
	red.SetColor(JColorManager::GetRedColor());

	styles->AppendElements(black, script.GetCharacterCount());

	JStringIterator siter(script);
	JRunArrayIterator<JFont> fiter(styles);
	JUtf8Character c;
	while (siter.Next(&c))
	{
		if (c == '\\')
		{
			siter.SkipNext();
			fiter.SkipNext(2);
		}
		else if (c == '$' && siter.AtEnd())
		{
			fiter.SetNext(red, JListT::kStay);
		}
		else if (c == '$' && siter.Next(&c, JStringIterator::kStay) && c == '(')
		{
			siter.SkipNext();
			const bool ok = BalanceForward(kCLang, &siter, &c);
			fiter.SetNext(ok ? blue : red, siter.GetPrevCharacterIndex() - fiter.GetNextElementIndex() + 1);
		}
		else
		{
			fiter.SkipNext();
		}
	}
}

/******************************************************************************
 AddMacro

 ******************************************************************************/

void
MacroManager::AddMacro
	(
	const JUtf8Byte* macro,
	const JUtf8Byte* script
	)
{
	assert( !JString::IsEmpty(macro) && script != nullptr );

	MacroInfo info(jnew JString(macro), jnew JString(script));
	assert( info.macro != nullptr && info.script != nullptr );
	itsMacroList->InsertSorted(info);
}

/******************************************************************************
 CompareMacros (static private)

 ******************************************************************************/

std::weak_ordering
MacroManager::CompareMacros
	(
	const MacroInfo& m1,
	const MacroInfo& m2
	)
{
	return JCompareStringsCaseInsensitive(m1.macro, m2.macro);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
MacroManager::ReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
	{
		return;
	}

	itsMacroList->DeleteAll();

	JSize count;
	input >> count;

	for (JIndex i=1; i<=count; i++)
	{
		MacroInfo info(jnew JString, jnew JString);
		assert( info.macro != nullptr && info.script != nullptr );
		input >> *(info.macro) >> *(info.script);
		itsMacroList->AppendElement(info);
	}

	if (vers == 0)
	{
		JString geom;
		JCoordinate width;
		input >> geom >> width;
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
MacroManager::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	const JSize count = itsMacroList->GetElementCount();
	output << ' ' << count;

	for (JIndex i=1; i<=count; i++)
	{
		const MacroInfo info = itsMacroList->GetElement(i);
		output << ' ' << *(info.macro) << ' ' << *(info.script);
	}

	output << ' ';
}

/******************************************************************************
 MacroList functions

 ******************************************************************************/

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
MacroList::DeleteAll()
{
	const JSize count = GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		MacroManager::MacroInfo info = GetElement(i);
		jdelete info.macro;
		jdelete info.script;
	}
	RemoveAll();
}
