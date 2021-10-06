/******************************************************************************
 CCompleter.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CCompleter
#define _H_CCompleter

#include "StringCompleter.h"

class CCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CCompleter();

	static JSize	GetDefaultWordList(const JUtf8Byte*** list);

protected:

	CCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CCompleter*	itsSelf;
};

#endif
