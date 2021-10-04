/******************************************************************************
 JVMGetLocalVarsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetLocalVarsCmd
#define _H_JVMGetLocalVarsCmd

#include "GetLocalVarsCmd.h"

class VarNode;

class JVMGetLocalVarsCmd : public GetLocalVarsCmd
{
public:

	JVMGetLocalVarsCmd(VarNode* rootNode);

	virtual	~JVMGetLocalVarsCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned
};

#endif
