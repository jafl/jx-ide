/******************************************************************************
 PHPCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_PHPCompleter
#define _H_PHPCompleter

#include "StringCompleter.h"

class PHPCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~PHPCompleter();

protected:

	PHPCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;
	virtual void		UpdateWordList() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	static PHPCompleter*	itsSelf;
};

#endif
