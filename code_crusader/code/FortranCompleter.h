/******************************************************************************
 FortranCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_FortranCompleter
#define _H_FortranCompleter

#include "StringCompleter.h"

class FortranCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~FortranCompleter();

protected:

	FortranCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		MatchCase(const JString& source, JString* target) const override;

private:

	static FortranCompleter*	itsSelf;
};

#endif
