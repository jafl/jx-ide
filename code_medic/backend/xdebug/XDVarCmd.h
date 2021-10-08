/******************************************************************************
 XDVarCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDVarCmd
#define _H_XDVarCmd

#include "VarCmd.h"

class VarNode;

namespace xdebug {

class VarCmd : public ::VarCmd
{
public:

	VarCmd(const JString& cmd);

	~VarCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;
};

};

#endif
