/******************************************************************************
 JVMGetFullPathCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetFullPathCmd
#define _H_JVMGetFullPathCmd

#include "GetFullPathCmd.h"

namespace jvm {

class GetFullPathCmd : public ::GetFullPathCmd
{
public:

	GetFullPathCmd(const JString& fileName, const JIndex lineIndex = 0);

	virtual	~GetFullPathCmd();

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
