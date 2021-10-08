/******************************************************************************
 LLDBGetAssemblyCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetAssemblyCmd
#define _H_LLDBGetAssemblyCmd

#include "GetAssemblyCmd.h"

class SourceDirector;

namespace lldb {

class GetAssemblyCmd : public ::GetAssemblyCmd
{
public:

	GetAssemblyCmd(SourceDirector* dir);

	~GetAssemblyCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
