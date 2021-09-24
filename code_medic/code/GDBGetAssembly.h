/******************************************************************************
 GDBGetAssembly.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetAssembly
#define _H_GDBGetAssembly

#include "CMGetAssembly.h"

class CMSourceDirector;

class GDBGetAssembly : public CMGetAssembly
{
public:

	GDBGetAssembly(CMSourceDirector* dir);

	virtual	~GDBGetAssembly();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
