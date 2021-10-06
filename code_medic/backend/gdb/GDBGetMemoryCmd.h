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

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
