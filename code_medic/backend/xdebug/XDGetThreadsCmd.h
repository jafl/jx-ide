/******************************************************************************
 XDGetThreadCmdsCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetThreadCmdsCmd
#define _H_XDGetThreadCmdsCmd

#include "GetThreadsCmd.h"

class JTree;

namespace xdebug {

class GetThreadsCmd : public ::GetThreadsCmd
{
public:

	GetThreadsCmd(JTree* tree, ThreadsWidget* widget);

	~GetThreadsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
