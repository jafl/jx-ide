/******************************************************************************
 CShellCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CShellCompleter
#define _H_CShellCompleter

#include "StringCompleter.h"

class CShellCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CShellCompleter();

protected:

	CShellCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CShellCompleter*	itsSelf;
};

#endif
