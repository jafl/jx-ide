/******************************************************************************
 VarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBVarNode
#define _H_GDBVarNode

#include <VarNode.h>

namespace gdb {

class VarNode : public ::VarNode
{
public:

	VarNode(const bool shouldUpdate = true);

	VarNode(JTreeNode* parent, const JString& name, const JString& value);

	~VarNode() override;

	JString	GetFullName(bool* isPointer = nullptr) const override;
	JString	GetFullNameWithCast(bool* isPointer = nullptr) const override;
};

};

#endif
