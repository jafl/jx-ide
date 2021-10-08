/******************************************************************************
 XDArray2DCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDArray2DCmd
#define _H_XDArray2DCmd

#include "Array2DCmd.h"

namespace xdebug {

class Array2DCmd : public ::Array2DCmd
{
public:

	Array2DCmd(Array2DDir* dir,
				JXStringTable* table, JStringTableData* data);

	~Array2DCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
