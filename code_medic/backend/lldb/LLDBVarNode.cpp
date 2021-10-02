/******************************************************************************
 LLDBVarNode.cpp

	BASE CLASS = public VarNode

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "LLDBVarNode.h"
#include "lldb/API/SBType.h"
#include "VarCommand.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

LLDBVarNode::LLDBVarNode			// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	VarNode(shouldUpdate)
{
}

LLDBVarNode::LLDBVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	value
	)
	:
	VarNode(parent, name, value)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LLDBVarNode::~LLDBVarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
LLDBVarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}

/******************************************************************************
 BuildTree (static)

 ******************************************************************************/

VarNode*
LLDBVarNode::BuildTree
	(
	lldb::SBFrame& f,
	lldb::SBValue& v
	)
{
	const JUtf8Byte* s = v.GetValue();
	if (s == nullptr)
	{
		s = "";
	}

	JString value(s);
	bool isPointer = false;
	bool isSpecial = false;

	JString name;
	if (v.GetName() != nullptr)
	{
		name = v.GetName();
	}

	if (v.TypeIsPointerType())
	{
		lldb::BasicType type = v.GetType().GetPointeeType().GetBasicType();

		if (type == lldb::eBasicTypeChar       ||
			type == lldb::eBasicTypeSignedChar ||
			type == lldb::eBasicTypeUnsignedChar)
		{
#ifdef _J_LLDB_HAS_SBVALUE_GETSUMMARY
			if (v.GetSummary() != nullptr)
			{
				value += "  ";
				value += v.GetSummary();
			}
#endif
			isSpecial = true;
		}
		else
		{
			isPointer = true;
		}
	}
	else if (v.GetType().GetTypeClass() == lldb::eTypeClassArray)
	{
		lldb::BasicType type = v.GetChildAtIndex(0).GetType().GetBasicType();
		if (type == lldb::eBasicTypeChar       ||
			type == lldb::eBasicTypeSignedChar ||
			type == lldb::eBasicTypeUnsignedChar)
		{
#ifdef _J_LLDB_HAS_SBVALUE_GETSUMMARY
			if (v.GetSummary() != nullptr)
			{
				value += "  ";
				value += v.GetSummary();
			}
#endif
			isSpecial = true;
		}
	}
	else if (v.GetType().GetTypeClass() == lldb::eTypeClassClass &&
			 !JString::IsEmpty(v.GetTypeName()) &&
			 name == v.GetTypeName())
	{
		// mark as "fake"
		name.Prepend("<");
		name.Append(">");
	}

	VarNode* node = GetLink()->CreateVarNode(nullptr, name, JString::empty, value);
	assert( node != nullptr );

	if (isPointer)
	{
		node->MakePointer(true);
		return node;
	}
	else if (isSpecial)
	{
		return node;
	}

	if (v.MightHaveChildren())
	{
		const JSize count = v.GetNumChildren();
		for (JUnsignedOffset i=0; i<count; i++)
		{
			lldb::SBValue child = v.GetChildAtIndex(i, lldb::eDynamicDontRunTarget, true);
			if (child.IsValid())
			{
				VarNode* n = BuildTree(f, child);
				node->Append(n);	// avoid automatic update
			}
		}
	}

	return node;
}
