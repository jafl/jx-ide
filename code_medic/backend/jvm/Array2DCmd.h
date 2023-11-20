/******************************************************************************
 JVMArray2DCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMArray2DCmd
#define _H_JVMArray2DCmd

#include <Array2DCmd.h>

namespace jvm {

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
