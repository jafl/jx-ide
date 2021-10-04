/******************************************************************************
 GDBGetMemoryCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetMemoryCmd
#define _H_GDBGetMemoryCmd

#include "GetMemoryCmd.h"

namespace gdb {

class GetMemoryCmd : public ::GetMemoryCmd
{
public:

	GetMemoryCmd(MemoryDir* dir);

	virtual	~GetMemoryCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

};

#endif
