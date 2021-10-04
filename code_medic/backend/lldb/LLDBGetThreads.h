/******************************************************************************
 LLDBGetThreads.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetThreads
#define _H_LLDBGetThreads

#include "GetThreadsCmd.h"

class JTree;

class LLDBGetThreads : public GetThreadsCmd
{
public:

	LLDBGetThreads(JTree* tree, ThreadsWidget* widget);

	virtual	~LLDBGetThreads();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;	// not owned
};

#endif
