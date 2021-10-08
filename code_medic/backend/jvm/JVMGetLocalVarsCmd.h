/******************************************************************************
 JVMGetLocalVarsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetLocalVarsCmd
#define _H_JVMGetLocalVarsCmd

#include "GetLocalVarsCmd.h"

class VarNode;

namespace jvm {

class GetLocalVarsCmd : public ::GetLocalVarsCmd
{
public:

	GetLocalVarsCmd(VarNode* rootNode);

	~GetLocalVarsCmd();

protected:

	void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned
};

};

#endif
