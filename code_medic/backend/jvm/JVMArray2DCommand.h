/******************************************************************************
 JVMArray2DCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMArray2DCommand
#define _H_JVMArray2DCommand

#include "Array2DCmd.h"

class JVMArray2DCommand : public Array2DCmd
{
public:

	JVMArray2DCommand(Array2DDir* dir,
					 JXStringTable* table, JStringTableData* data);

	virtual	~JVMArray2DCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
