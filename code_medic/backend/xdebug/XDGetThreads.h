/******************************************************************************
 XDGetThreads.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetThreads
#define _H_XDGetThreads

#include "GetThreadsCmd.h"

class JTree;

class XDGetThreads : public GetThreadsCmd
{
public:

	XDGetThreads(JTree* tree, ThreadsWidget* widget);

	virtual	~XDGetThreads();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
