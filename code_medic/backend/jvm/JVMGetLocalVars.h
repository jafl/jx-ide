/******************************************************************************
 JVMGetLocalVars.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetLocalVars
#define _H_JVMGetLocalVars

#include "GetLocalVars.h"

class VarNode;

class JVMGetLocalVars : public GetLocalVars
{
public:

	JVMGetLocalVars(VarNode* rootNode);

	virtual	~JVMGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned
};

#endif
