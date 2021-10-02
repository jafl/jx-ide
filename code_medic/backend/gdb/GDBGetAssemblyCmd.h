/******************************************************************************
 GDBGetAssemblyCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetAssemblyCmd
#define _H_GDBGetAssemblyCmd

#include "GetAssemblyCmd.h"

class SourceDirector;

class GDBGetAssemblyCmd : public GetAssemblyCmd
{
public:

	GDBGetAssemblyCmd(SourceDirector* dir);

	virtual	~GDBGetAssemblyCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
