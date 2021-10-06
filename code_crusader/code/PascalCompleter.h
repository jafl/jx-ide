/******************************************************************************
 PascalCompleter.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_PascalCompleter
#define _H_PascalCompleter

#include "StringCompleter.h"

class PascalCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~PascalCompleter();

protected:

	PascalCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static PascalCompleter*	itsSelf;
};

#endif
