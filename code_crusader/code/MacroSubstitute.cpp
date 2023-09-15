/******************************************************************************
 MacroSubstitute.cpp

	BASE CLASS = JSubstitute

	Copyright © 1998 by John Lindal.

 *****************************************************************************/

#include "MacroSubstitute.h"
#include "sharedUtil.h"
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

MacroSubstitute::MacroSubstitute()
	:
	JSubstitute()
{
	itsExecCount = 0;
	TurnOnEscapes();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MacroSubstitute::~MacroSubstitute()
{
}

/******************************************************************************
 Evaluate (virtual protected)

	Override to handle evaluation of commands:  $(w $($(x) y) z))

 *****************************************************************************/

bool
MacroSubstitute::Evaluate
	(
	JStringIterator&	iter,
	JString*			value
	)
	const
{
	JUtf8Character c;
	if (iter.Next(&c, JStringIterator::kStay) && c == '(')
	{
		iter.SkipNext();
		iter.BeginMatch();
		if (BalanceForward(kCLang, &iter, &c))
		{
			iter.SkipPrev();
			*value = iter.FinishMatch().GetString();
			iter.SkipNext();

			auto* me = const_cast<MacroSubstitute*>(this);
			if (itsExecCount == 0)
			{
				me->TurnOffEscapes();
			}
			me->itsExecCount++;
			Substitute(value);		// recursive!
			me->itsExecCount--;
			if (itsExecCount == 0)
			{
				me->TurnOnEscapes();
			}

			pid_t pid;
			int fromFD, errFD;
			const JError execErr =
				JExecute(*value, &pid, kJIgnoreConnection, nullptr,
						 kJCreatePipe, &fromFD, kJCreatePipe, &errFD);
			if (!execErr.OK())
			{
				execErr.ReportIfError();
				value->Clear();
			}
			else
			{
				JString msg;
				JReadAll(errFD, &msg);
				if (!msg.IsEmpty())
				{
					msg.Prepend(JGetString("Error::global"));
					JGetUserNotification()->ReportError(msg);
				}

				JReadAll(fromFD, value);
				value->TrimWhitespace();
			}
		}
		else
		{
			value->Clear();
		}

		return true;
	}
	else
	{
		return JSubstitute::Evaluate(iter, value);
	}
}

/******************************************************************************
 TurnOnEscapes (private)

 *****************************************************************************/

void
MacroSubstitute::TurnOnEscapes()
{
	SetEscape('n', "\n");
	SetEscape('t', "\t");
	SetEscape('b', "\b");

	JUtf8Byte c[] = { '\0', '\0' };

	c[0] = kJForwardDeleteKey;
	SetEscape('f', c);

	c[0] = kJLeftArrow;
	SetEscape('l', c);

	c[0] = kJRightArrow;
	SetEscape('r', c);

	c[0] = kJUpArrow;
	SetEscape('u', c);

	c[0] = kJDownArrow;
	SetEscape('d', c);
}

/******************************************************************************
 TurnOffEscapes (private)

 *****************************************************************************/

void
MacroSubstitute::TurnOffEscapes()
{
	ClearEscape('n');
	ClearEscape('t');
	ClearEscape('b');
	ClearEscape('f');
	ClearEscape('l');
	ClearEscape('r');
	ClearEscape('u');
	ClearEscape('d');
}
