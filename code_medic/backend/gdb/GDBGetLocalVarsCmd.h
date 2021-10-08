/******************************************************************************
 GDBGetLocalVarsCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetLocalVarsCmd
#define _H_GDBGetLocalVarsCmd

#include "GetLocalVarsCmd.h"

class VarNode;

namespace gdb {

class GetLocalVarsCmd : public ::GetLocalVarsCmd
{
public:

	GetLocalVarsCmd(VarNode* rootNode);

	~GetLocalVarsCmd();

protected:

	void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned

private:

	void	CleanVarString(JString* s);
};

};

#endif
