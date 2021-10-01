/******************************************************************************
 PerlCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_PerlCompleter
#define _H_PerlCompleter

#include "StringCompleter.h"

class PerlCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~PerlCompleter();

protected:

	PerlCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static PerlCompleter*	itsSelf;
};

#endif
