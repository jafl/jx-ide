/******************************************************************************
 RubyCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_RubyCompleter
#define _H_RubyCompleter

#include "StringCompleter.h"

class RubyCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~RubyCompleter() override;

protected:

	RubyCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;

private:

	static RubyCompleter*	itsSelf;
};

#endif
