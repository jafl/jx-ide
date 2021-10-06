/******************************************************************************
 MacroSubstitute.h

	Copyright © 1998 by John Lindal.

 *****************************************************************************/

#ifndef _H_MacroSubstitute
#define _H_MacroSubstitute

#include <jx-af/jcore/JSubstitute.h>

class MacroSubstitute : public JSubstitute
{
public:

	MacroSubstitute();

	virtual ~MacroSubstitute();

protected:

	bool	Evaluate(JStringIterator& iter, JString* value) const override;

private:

	JSize	itsExecCount;

private:

	void	TurnOnEscapes();
	void	TurnOffEscapes();

	// not allowed

	MacroSubstitute(const MacroSubstitute&) = delete;
	MacroSubstitute& operator=(const MacroSubstitute&) = delete;
};

#endif
