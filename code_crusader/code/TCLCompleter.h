/******************************************************************************
 TCLCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_TCLCompleter
#define _H_TCLCompleter

#include "StringCompleter.h"

class TCLCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~TCLCompleter();

protected:

	TCLCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static TCLCompleter*	itsSelf;
};

#endif
