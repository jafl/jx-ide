/******************************************************************************
 BourneShellCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_BourneShellCompleter
#define _H_BourneShellCompleter

#include "StringCompleter.h"

class BourneShellCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~BourneShellCompleter() override;

protected:

	BourneShellCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;

private:

	static BourneShellCompleter*	itsSelf;
};

#endif
