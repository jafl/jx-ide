/******************************************************************************
 JVMVarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMVarNode
#define _H_JVMVarNode

#include "VarNode.h"

namespace jvm {

class VarNode : public ::VarNode
{
public:

	VarNode(const bool shouldUpdate = true);

	VarNode(JTreeNode* parent, const JString& name,
			   const JString& fullName, const JString& value);

	virtual	~VarNode();

	JString	GetFullName(bool* isPointer = nullptr) const override;
};

};

#endif
