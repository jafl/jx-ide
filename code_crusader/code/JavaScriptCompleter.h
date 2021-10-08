/******************************************************************************
 JavaScriptCompleter.h

	Copyright © 2006 by John Lindal.

 ******************************************************************************/

#ifndef _H_JavaScriptCompleter
#define _H_JavaScriptCompleter

#include "StringCompleter.h"

class JavaScriptCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void				Shutdown();

	~JavaScriptCompleter();

	static JSize	GetDefaultWordList(const JUtf8Byte*** list);

protected:

	JavaScriptCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
							const bool includeNS) const override;
	void	MatchCase(const JString& source, JString* target) const override;

private:

	static JavaScriptCompleter*	itsSelf;
};

#endif
