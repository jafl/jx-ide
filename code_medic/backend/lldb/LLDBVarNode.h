/******************************************************************************
 LLDBVarNode.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBVarNode
#define _H_LLDBVarNode

#include "VarNode.h"
#include "lldb/API/SBFrame.h"
#include "lldb/API/SBValue.h"

class LLDBVarNode : public VarNode
{
public:

	LLDBVarNode(const bool shouldUpdate = true);

	LLDBVarNode(JTreeNode* parent, const JString& name, const JString& value);

	virtual	~LLDBVarNode();

	virtual JString	GetFullName(bool* isPointer = nullptr) const override;

	static VarNode*	BuildTree(lldb::SBFrame& frame, lldb::SBValue& value);
};

#endif
