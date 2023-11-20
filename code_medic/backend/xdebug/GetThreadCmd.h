/******************************************************************************
 GetThreadCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetThreadCmd
#define _H_XDGetThreadCmd

#include <GetThreadCmd.h>

namespace xdebug {

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
