/******************************************************************************
 EiffelCompleter.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_EiffelCompleter
#define _H_EiffelCompleter

#include "StringCompleter.h"

class EiffelCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~EiffelCompleter();

protected:

	EiffelCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		MatchCase(const JString& source, JString* target) const override;

private:

	static EiffelCompleter*	itsSelf;
};

#endif
