/******************************************************************************
 Link.cpp

	BASE CLASS = public JBroadcaster

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#include "Link.h"
#include "MemberFunction.h"
#include "Class.h"
#include "globals.h"

#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kCtagsArgs =
	"--format=2 --excmd=number --sort=no "
	"--c-kinds=p --fields=+a --fields-C++=+{properties}";

// JBroadcaster messages

const JUtf8Byte* Link::kFileParsed = "FileParsed::Link";

/******************************************************************************
 Constructor

 *****************************************************************************/

Link::Link()
	:
	CtagsUser(kCtagsArgs)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Link::~Link()
{
}

/******************************************************************************
 ParseClass

 ******************************************************************************/

void
Link::ParseClass
	(
	Class*			list,
	const JString&	fileName, 
	const JString&	className
	)
{
	JString fullName;
	JConvertToAbsolutePath(fileName, JString::empty, &fullName);

	JString data;
	Language lang;
	if (!ProcessFile(fullName, kCHeaderFT, &data, &lang))
	{
		return;
	}

	JPtrArray<JString> lines(JPtrArrayT::kDeleteAll);
	ReadFile(fullName, &lines);

	icharbufstream input(data.GetBytes(), data.GetByteCount());

	input >> std::ws;
	while (input.peek() == '!')
	{
		JIgnoreLine(input);
		input >> std::ws;
	}

	JString name;
	JStringPtrMap<JString> flags(JPtrArrayT::kDeleteAll);
	while (true)
	{
		input >> std::ws;
		name = JReadUntil(input, '\t');			// function name
		if (input.eof() || input.fail())
		{
			break;
		}

		JIgnoreUntil(input, '\t');				// file name

		JIndex lineIndex;
		input >> lineIndex;						// line index

		ReadExtensionFlags(input, &flags);

		if (name == className || name.StartsWith("~"))	// ctor or dtor
		{
			continue;
		}

		JString* s;
		if (!flags.GetItem("class", &s) || *s != className)
		{
			continue;
		}

		bool required = false, isConst = false;

		if (flags.GetItem("properties", &s))
		{
			if (s->Contains("virtual"))
			{
				required = s->Contains("pure");
			}
			else
			{
				continue;
			}

			if (s->Contains("const"))
			{
				isConst = true;
			}
		}
		else
		{
			continue;
		}

		auto* fn = jnew MemberFunction;

		fn->SetFnName(name);
		fn->ShouldBeRequired(required);
		fn->ShouldBeConst(isConst);

		if (flags.GetItem("access", &s) && *s == "protected")
		{
			fn->ShouldBeProtected(true);
		}

		if (flags.GetItem("signature", &s) && !s->IsEmpty())
		{
			fn->SetSignature(*s);
		}

		fn->SetReturnType(GetReturnType(name, lineIndex, lines));

		// Override entry from base class so function will only be
		// marked as pure virtual if nobody implemented it.

		bool found;
		const JIndex i = list->SearchSortedOTI(fn, JListT::kAnyMatch, &found);
		if (found)
		{
			list->DeleteItem(i);
		}
		list->InsertAtIndex(i, fn);

		Broadcast(FileParsed());
	}
}

/******************************************************************************
 ReadFile (private)

 *****************************************************************************/

void
Link::ReadFile
	(
	const JString&		fullName,
	JPtrArray<JString>*	lines
	)
	const
{
	JString data;
	JReadFile(fullName, &data);

	data.Split("\n", lines);
}

/******************************************************************************
 GetReturnType (private)

 *****************************************************************************/

static const JRegex startPattern("[;:{]");
static const JRegex commentPattern("(/\\*(.|\n)+?\\*/)|(//.+?(?=\n))");
static const JRegex spacePattern("[[:space:]]+");

JString
Link::GetReturnType
	(
	const JString&				name,
	const JIndex				lineIndex,
	const JPtrArray<JString>&	lines
	)
	const
{
	JIndex i = lineIndex;
	while (i > 1)
	{
		i--;

		if (startPattern.Match(*lines.GetItem(i)))
		{
			break;
		}
	}

	JString s = *lines.GetItem(i);
	{
	JStringIterator iter(&s, JStringIterator::kStartAtEnd);
	if (iter.Prev(startPattern))
	{
		iter.SkipNext();
		iter.RemoveAllPrev();
	}
	}

	for (JIndex j=i+1; j<=lineIndex; j++)
	{
		s += "\n";
		s += *lines.GetItem(j);
	}

	JStringIterator iter(&s, JStringIterator::kStartAtEnd);
	const bool ok = iter.Prev(name);
	assert( ok );
	iter.RemoveAllNext();

	iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	while (iter.Next(commentPattern))
	{
		iter.RemoveLastMatch();
	}

	iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	if (iter.Next("virtual"))
	{
		iter.RemoveLastMatch();
	}

	iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
	while (iter.Next(spacePattern))
	{
		iter.ReplaceLastMatch(" ");
	}

	s.TrimWhitespace();
	return s;
}
