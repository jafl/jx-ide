/******************************************************************************
 XDGetLocalVarsCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetLocalVarsCmd
#define _H_XDGetLocalVarsCmd

#include "GetLocalVarsCmd.h"

class VarNode;

namespace xdebug {

class GetLocalVarsCmd : public ::GetLocalVarsCmd
{
public:

	GetLocalVarsCmd(VarNode* rootNode);

	~GetLocalVarsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned
};

};

#endif
