/******************************************************************************
 LLDBGetAssembly.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetAssembly
#define _H_LLDBGetAssembly

#include "CMGetAssembly.h"

class CMSourceDirector;

class LLDBGetAssembly : public CMGetAssembly
{
public:

	LLDBGetAssembly(CMSourceDirector* dir);

	virtual	~LLDBGetAssembly();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
