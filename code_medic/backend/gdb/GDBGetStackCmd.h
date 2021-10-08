/******************************************************************************
 GDBGetStackCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStackCmd
#define _H_GDBGetStackCmd

#include "GetStackCmd.h"

namespace gdb {

class GetStackArgumentsCmd;

class GetStackCmd : public ::GetStackCmd
{
public:

	GetStackCmd(JTree* tree, StackWidget* widget);

	~GetStackCmd();

protected:

	void	HandleSuccess(const JString& data) override;

private:

	GetStackArgumentsCmd*	itsArgsCmd;
};

};

#endif
