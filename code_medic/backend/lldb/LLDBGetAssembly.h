/******************************************************************************
 LLDBGetAssembly.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetAssembly
#define _H_LLDBGetAssembly

#include "GetAssemblyCmd.h"

class SourceDirector;

class LLDBGetAssembly : public GetAssemblyCmd
{
public:

	LLDBGetAssembly(SourceDirector* dir);

	virtual	~LLDBGetAssembly();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
