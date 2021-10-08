/******************************************************************************
 GDBGetThreadsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetThreadsCmd
#define _H_GDBGetThreadsCmd

#include "GetThreadsCmd.h"

class JTree;

namespace gdb {

class GetThreadsCmd : public ::GetThreadsCmd
{
public:

	GetThreadsCmd(JTree* tree, ThreadsWidget* widget);

	~GetThreadsCmd();

	static bool	ExtractThreadIndex(const JString& line, JIndex* threadIndex);
	static bool	ExtractLocation(const JString& line,
									JString* fileName, JIndex* lineIndex);

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;	// not owned

private:

	static JListT::CompareResult
		CompareThreadIndices(JString* const & l1, JString* const & l2);
};

};

#endif
