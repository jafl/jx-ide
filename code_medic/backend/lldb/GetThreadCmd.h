/******************************************************************************
 GetThreadCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetThreadCmd
#define _H_LLDBGetThreadCmd

#include <GetThreadCmd.h>

namespace lldb {

class GetThreadCmd : public ::GetThreadCmd
{
public:

	GetThreadCmd(ThreadsWidget* widget);

	~GetThreadCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
