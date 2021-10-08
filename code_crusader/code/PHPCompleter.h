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
	static void				Shutdown();

	~PHPCompleter();

protected:

	PHPCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;
	void	UpdateWordList() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static PHPCompleter*	itsSelf;
};

#endif
