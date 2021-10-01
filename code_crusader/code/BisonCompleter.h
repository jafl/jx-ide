/******************************************************************************
 BisonCompleter.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_BisonCompleter
#define _H_BisonCompleter

#include "StringCompleter.h"

class BisonCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~BisonCompleter();

protected:

	BisonCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		UpdateWordList() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	static BisonCompleter*	itsSelf;
};

#endif
