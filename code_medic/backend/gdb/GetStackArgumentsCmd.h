/******************************************************************************
 GetStackArgumentsCmd.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStackArgumentsCmd
#define _H_GDBGetStackArgumentsCmd

#include "Command.h"

class JTree;

namespace gdb {

class GetStackArgumentsCmd : public Command
{
public:

	GetStackArgumentsCmd(JTree* tree);

	~GetStackArgumentsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;		// not owned
};

};

#endif
