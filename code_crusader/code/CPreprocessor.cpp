/******************************************************************************
 CPreprocessor.cpp

	Implements a trivial C preprocessor that only understands simple
	identifier replacement.

	BASE CLASS = none

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CPreprocessor.h"
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CPreprocessor::CPreprocessor()
{
	itsMacroList = jnew PPMacroList;
	itsMacroList->SetSortOrder(JListT::kSortAscending);
	itsMacroList->SetCompareFunction(CompareMacros);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CPreprocessor::~CPreprocessor()
{
	itsMacroList->DeleteAll();
	jdelete itsMacroList;
}

/******************************************************************************
 Preprocess

	Returns true if it changed the text.

 ******************************************************************************/

bool
CPreprocessor::Preprocess
	(
	JString* text
	)
	const
{
	bool changed = false;

	JStringIterator iter(text);
	for (const auto& info : *itsMacroList)
	{
		JRegex r("\\b" + JRegex::BackslashForLiteral(*info.name)  + "\\b");

		iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
		while (iter.Next(r))
		{
			iter.ReplaceLastMatch(*info.value);
			changed = true;
		}
	}

	return changed;
}

/******************************************************************************
 PrintMacrosForCTags

	Returns true if any macros have been defined.

	Format:  macro/macro+/macro=value

 ******************************************************************************/

static const JUtf8Byte* kDefineMacroOption = "-I ";	// macro/macro+/macro=value

void
CPreprocessor::PrintMacrosForCTags
	(
	std::ostream& output
	)
	const
{
	const JSize count = itsMacroList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const MacroInfo info = itsMacroList->GetItem(i);

		if (!info.name->Contains(" ") &&
			!info.name->Contains("\t") &&
			!info.value->Contains(" ") &&
			!info.value->Contains("\t"))
		{
			output << kDefineMacroOption;
			info.name->Print(output);

			if (!info.value->IsEmpty())
			{
				output << '=';
				info.value->Print(output);
			}

			output << std::endl;
		}
	}
}

/******************************************************************************
 DefineMacro

 ******************************************************************************/

void
CPreprocessor::DefineMacro
	(
	const JString& name,
	const JString& value
	)
{
	assert( !name.IsEmpty() );

	itsMacroList->InsertSorted(
		MacroInfo(jnew JString(name), jnew JString(value)));
}

/******************************************************************************
 CompareMacros (static private)

 ******************************************************************************/

std::weak_ordering
CPreprocessor::CompareMacros
	(
	const MacroInfo& m1,
	const MacroInfo& m2
	)
{
	return JCompareStringsCaseInsensitive(m1.name, m2.name);
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CPreprocessor::ReadSetup
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	assert( vers >= 28 );

	itsMacroList->DeleteAll();

	if (vers >= 71)
	{
		input >> std::ws;
		JIgnoreLine(input);
	}

	if (vers <= 71)
	{
		JSize count;
		input >> count;

		for (JIndex i=1; i<=count; i++)
		{
			ReadMacro(input, vers);
		}
	}
	else
	{
		while (true)
		{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
			if (!keepGoing)
			{
				break;
			}

			ReadMacro(input, vers);
		}
	}
}

/******************************************************************************
 ReadMacro (private)

 ******************************************************************************/

void
CPreprocessor::ReadMacro
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	MacroInfo info(jnew JString, jnew JString);
	input >> *info.name >> *info.value;
	itsMacroList->AppendItem(info);
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CPreprocessor::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << "# C preprocessor\n";

	const JSize count = itsMacroList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		output << JBoolToString(true);

		const MacroInfo info = itsMacroList->GetItem(i);
		output << ' ' << *(info.name);
		output << ' ' << *(info.value);
		output << '\n';
	}

	output << JBoolToString(false) << '\n';
}

/******************************************************************************
 PPMacroList functions

 ******************************************************************************/

/******************************************************************************
 DeleteAll

 ******************************************************************************/

void
PPMacroList::DeleteAll()
{
	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		CPreprocessor::MacroInfo info = GetItem(i);
		jdelete info.name;
		jdelete info.value;
	}

	RemoveAll();
}
