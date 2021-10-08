/******************************************************************************
 LLDBGetThreadsCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetThreadsCmd
#define _H_LLDBGetThreadsCmd

#include "GetThreadsCmd.h"

class JTree;

namespace lldb {

class GetThreadsCmd : public ::GetThreadsCmd
{
public:

	GetThreadsCmd(JTree* tree, ThreadsWidget* widget);

	~GetThreadsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;	// not owned
};

};

#endif
