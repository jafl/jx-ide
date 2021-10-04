/******************************************************************************
 JVMArray2DCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMArray2DCmd
#define _H_JVMArray2DCmd

#include "Array2DCmd.h"

class JVMArray2DCmd : public Array2DCmd
{
public:

	JVMArray2DCmd(Array2DDir* dir,
				  JXStringTable* table, JStringTableData* data);

	virtual	~JVMArray2DCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
