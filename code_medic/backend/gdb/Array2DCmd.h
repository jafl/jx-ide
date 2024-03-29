/******************************************************************************
 GDBArray2DCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBArray2DCmd
#define _H_GDBArray2DCmd

#include <Array2DCmd.h>

namespace gdb {

class Array2DCmd : public ::Array2DCmd
{
public:

	Array2DCmd(Array2DDir* dir,
			   JXStringTable* table, JStringTableData* data);

	~Array2DCmd() override;

	void	PrepareToSend(const UpdateType type, const JIndex index,
						  const JInteger arrayIndex) override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
