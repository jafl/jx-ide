/******************************************************************************
 GDBGetAssemblyCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetAssemblyCmd
#define _H_GDBGetAssemblyCmd

#include "GetAssemblyCmd.h"

class SourceDirector;

namespace gdb {

class GetAssemblyCmd : public ::GetAssemblyCmd
{
public:

	GetAssemblyCmd(SourceDirector* dir);

	~GetAssemblyCmd() override;

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
