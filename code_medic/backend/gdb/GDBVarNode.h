/******************************************************************************
 GDBVarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBVarNode
#define _H_GDBVarNode

#include "VarNode.h"

class GDBVarNode : public VarNode
{
public:

	GDBVarNode(const bool shouldUpdate = true);

	GDBVarNode(JTreeNode* parent, const JString& name, const JString& value);

	virtual	~GDBVarNode();

	virtual JString	GetFullName(bool* isPointer = nullptr) const override;
};

#endif
