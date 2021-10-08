/******************************************************************************
 XDGetFullPathCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetFullPathCmd
#define _H_XDGetFullPathCmd

#include "GetFullPathCmd.h"

namespace xdebug {

class GetFullPathCmd : public ::GetFullPathCmd
{
public:

	GetFullPathCmd(const JString& fileName, const JIndex lineIndex = 0);

	~GetFullPathCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
