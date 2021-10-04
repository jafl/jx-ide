/******************************************************************************
 GDBGetThreadCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetThreadCmd
#define _H_GDBGetThreadCmd

#include "GetThreadCmd.h"

namespace gdb {

class GetThreadCmd : public ::GetThreadCmd
{
public:

	GetThreadCmd(ThreadsWidget* widget);

	virtual	~GetThreadCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

};

#endif
