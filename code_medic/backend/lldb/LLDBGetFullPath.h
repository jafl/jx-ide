/******************************************************************************
 LLDBGetFullPath.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetFullPath
#define _H_LLDBGetFullPath

#include "CMGetFullPath.h"

class LLDBGetFullPath : public CMGetFullPath
{
public:

	LLDBGetFullPath(const JString& fileName, const JIndex lineIndex = 0);

	virtual	~LLDBGetFullPath();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
