/******************************************************************************
 LexCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_LexCompleter
#define _H_LexCompleter

#include "StringCompleter.h"

class LexCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~LexCompleter();

protected:

	LexCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;
	void	UpdateWordList() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static LexCompleter*	itsSelf;
};

#endif
