/******************************************************************************
 LLDBVarNode.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBVarNode
#define _H_LLDBVarNode

#include "VarNode.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValue.h"

namespace lldb {

class VarNode : public ::VarNode
{
public:

	VarNode(const bool shouldUpdate = true);

	VarNode(JTreeNode* parent, const JString& name, const JString& value);

	~VarNode() override;

	JString	GetFullName(bool* isPointer = nullptr) const override;

	static ::VarNode*	BuildTree(SBFrame& frame, SBValue& value);
};

};

#endif
