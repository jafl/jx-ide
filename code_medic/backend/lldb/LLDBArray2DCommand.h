/******************************************************************************
 LLDBArray2DCommand.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBArray2DCommand
#define _H_LLDBArray2DCommand

#include "Array2DCmd.h"

class LLDBArray2DCommand : public Array2DCmd
{
public:

	LLDBArray2DCommand(Array2DDir* dir,
					   JXStringTable* table, JStringTableData* data);

	virtual	~LLDBArray2DCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
