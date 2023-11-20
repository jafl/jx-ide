/******************************************************************************
 GetContextVarsCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetContextVarsCmd
#define _H_XDGetContextVarsCmd

#include "Command.h"
#include <jx-af/jcore/jXMLUtil.h>	// for xmlNode

class VarNode;

namespace xdebug {

class GetContextVarsCmd : public Command
{
public:

	GetContextVarsCmd(::VarNode* rootNode, const JString& contextID);

	~GetContextVarsCmd() override;

	static void	BuildTree(const JSize depth, xmlNode* root, ::VarNode* varRoot);

protected:

	void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned
};

};

#endif
