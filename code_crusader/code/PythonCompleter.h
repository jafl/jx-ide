/******************************************************************************
 PythonCompleter.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_PythonCompleter
#define _H_PythonCompleter

#include "StringCompleter.h"

class PythonCompleter : public StringCompleter
{
public:

	static StringCompleter*	Instance();
	static void					Shutdown();

	virtual ~PythonCompleter();

protected:

	PythonCompleter();

	bool	IsWordCharacter(const JUtf8Character& c,
										const bool includeNS) const override;

private:

	static PythonCompleter*	itsSelf;
};

#endif
