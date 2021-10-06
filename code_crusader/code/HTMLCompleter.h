/******************************************************************************
 HTMLCompleter.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_HTMLCompleter
#define _H_HTMLCompleter

#include "StringCompleter.h"

class HTMLCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~HTMLCompleter();

	static JSize	GetDefaultWordList(const JUtf8Byte*** list);

protected:

	HTMLCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static HTMLCompleter*	itsSelf;
};

#endif
