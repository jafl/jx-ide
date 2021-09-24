/******************************************************************************
 LLDBArray2DCommand.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBArray2DCommand
#define _H_LLDBArray2DCommand

#include "CMArray2DCommand.h"

class LLDBArray2DCommand : public CMArray2DCommand
{
public:

	LLDBArray2DCommand(CMArray2DDir* dir,
					   JXStringTable* table, JStringTableData* data);

	virtual	~LLDBArray2DCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
