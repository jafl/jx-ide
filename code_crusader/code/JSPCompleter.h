/******************************************************************************
 JSPCompleter.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_JSPCompleter
#define _H_JSPCompleter

#include "StringCompleter.h"

class JSPCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~JSPCompleter();

protected:

	JSPCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		UpdateWordList() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	static JSPCompleter*	itsSelf;
};

#endif
