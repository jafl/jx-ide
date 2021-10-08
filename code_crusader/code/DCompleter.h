/******************************************************************************
 DCompleter.h

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#ifndef _H_DCompleter
#define _H_DCompleter

#include "StringCompleter.h"

class DCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~DCompleter();

	static JSize	GetDefaultWordList(const JUtf8Byte*** list);

protected:

	DCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;

private:

	static DCompleter*	itsSelf;
};

#endif
