/******************************************************************************
 MakeCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_MakeCompleter
#define _H_MakeCompleter

#include "StringCompleter.h"

class MakeCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~MakeCompleter() override;

protected:

	MakeCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;

private:

	static MakeCompleter*	itsSelf;
};

#endif
