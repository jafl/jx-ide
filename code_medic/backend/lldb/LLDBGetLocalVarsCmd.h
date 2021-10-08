/******************************************************************************
 LLDBGetLocalVarsCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetLocalVarsCmd
#define _H_LLDBGetLocalVarsCmd

#include "GetLocalVarsCmd.h"

class VarNode;

namespace lldb {

class GetLocalVarsCmd : public ::GetLocalVarsCmd
{
public:

	GetLocalVarsCmd(::VarNode* rootNode);

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
