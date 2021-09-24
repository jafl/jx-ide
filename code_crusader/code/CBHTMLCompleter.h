/******************************************************************************
 CBHTMLCompleter.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBHTMLCompleter
#define _H_CBHTMLCompleter

#include "CBStringCompleter.h"

class CBHTMLCompleter : public CBStringCompleter
{
public:

	static CBStringCompleter*	Instance();
	static void					Shutdown();

	virtual ~CBHTMLCompleter();

	static JSize	GetDefaultWordList(const JUtf8Byte*** list);

protected:

	CBHTMLCompleter();

	virtual bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static CBHTMLCompleter*	itsSelf;
};

#endif
