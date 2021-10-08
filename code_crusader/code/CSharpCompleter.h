/******************************************************************************
 CSharpCompleter.h

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_CSharpCompleter
#define _H_CSharpCompleter

#include "StringCompleter.h"

class CSharpCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~CSharpCompleter() override;

protected:

	CSharpCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;

private:

	static CSharpCompleter*	itsSelf;
};

#endif
