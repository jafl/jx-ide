/******************************************************************************
 GoCompleter.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_GoCompleter
#define _H_GoCompleter

#include "StringCompleter.h"

class GoCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~GoCompleter();

protected:

	GoCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;

private:

	static GoCompleter*	itsSelf;
};

#endif
