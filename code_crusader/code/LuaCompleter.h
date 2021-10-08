/******************************************************************************
 LuaCompleter.h

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_LuaCompleter
#define _H_LuaCompleter

#include "StringCompleter.h"

class LuaCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~LuaCompleter();

protected:

	LuaCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;

private:

	static LuaCompleter*	itsSelf;
};

#endif
