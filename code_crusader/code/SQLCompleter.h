/******************************************************************************
 SQLCompleter.h

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_SQLCompleter
#define _H_SQLCompleter

#include "StringCompleter.h"

class SQLCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~SQLCompleter();

protected:

	SQLCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		MatchCase(const JString& source, JString* target) const override;

private:

	static SQLCompleter*	itsSelf;
};

#endif