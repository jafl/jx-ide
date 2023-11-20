/******************************************************************************
 LLDBArray2DCommand.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBArray2DCommand
#define _H_LLDBArray2DCommand

#include <Array2DCmd.h>

namespace lldb {

class Array2DCmd : public ::Array2DCmd
{
public:

	Array2DCmd(Array2DDir* dir, JXStringTable* table, JStringTableData* data);

	~Array2DCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
