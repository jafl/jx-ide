/******************************************************************************
 GDBArray2DCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBArray2DCmd
#define _H_GDBArray2DCmd

#include "Array2DCmd.h"

class GDBArray2DCmd : public Array2DCmd
{
public:

	GDBArray2DCmd(Array2DDir* dir,
					 JXStringTable* table, JStringTableData* data);

	virtual	~GDBArray2DCmd();

	virtual void	PrepareToSend(const UpdateType type, const JIndex index,
								  const JInteger arrayIndex) override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
