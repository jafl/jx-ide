/******************************************************************************
 JVMVarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMVarNode
#define _H_JVMVarNode

#include "VarNode.h"

class JVMVarNode : public VarNode
{
public:

	JVMVarNode(const bool shouldUpdate = true);

	JVMVarNode(JTreeNode* parent, const JString& name,
			   const JString& fullName, const JString& value);

	virtual	~JVMVarNode();

	virtual JString	GetFullName(bool* isPointer = nullptr) const override;
};

#endif
