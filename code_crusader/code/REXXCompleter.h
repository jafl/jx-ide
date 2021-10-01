/******************************************************************************
 REXXCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_REXXCompleter
#define _H_REXXCompleter

#include "StringCompleter.h"

class REXXCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~REXXCompleter();

protected:

	REXXCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static REXXCompleter*	itsSelf;
};

#endif
