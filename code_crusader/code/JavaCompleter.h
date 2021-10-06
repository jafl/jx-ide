/******************************************************************************
 JavaCompleter.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JavaCompleter
#define _H_JavaCompleter

#include "StringCompleter.h"

class JavaCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~JavaCompleter();

protected:

	JavaCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static JavaCompleter*	itsSelf;
};

#endif
