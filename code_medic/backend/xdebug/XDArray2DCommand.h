/******************************************************************************
 XDArray2DCommand.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDArray2DCommand
#define _H_XDArray2DCommand

#include "Array2DCmd.h"

class XDArray2DCommand : public Array2DCmd
{
public:

	XDArray2DCommand(Array2DDir* dir,
					 JXStringTable* table, JStringTableData* data);

	virtual	~XDArray2DCommand();

protected:

	void	HandleSuccess(const JString& data) override;
};

#endif
