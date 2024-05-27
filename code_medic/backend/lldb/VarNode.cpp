/******************************************************************************
 VarNode.cpp

	BASE CLASS = public VarNode

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "lldb/VarNode.h"
#include "lldb/API/SBType.h"
#include "lldb/API/SBError.h"
#include "VarCmd.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

lldb::VarNode::VarNode			// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	::VarNode(shouldUpdate)
{
}

lldb::VarNode::VarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	value
	)
	:
	::VarNode(parent, name, value)
{
}

lldb::VarNode::VarNode
	(
	const JUtf8Byte*	type,
	const JString&		name,
	const JString&		value
	)
	:
	::VarNode(nullptr, name, value)
{
	SetType(type);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

lldb::VarNode::~VarNode()
{
}

/******************************************************************************
 GetFullName (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
lldb::VarNode::GetFullName
	(
	bool* isPointer
	)
	const
{
	return GetFullNameForCFamilyLanguage(isPointer);
}

/******************************************************************************
 GetFullNameWithCast (virtual)

	isPointer can be nullptr.  Its content is only modified if GetFullName()
	has to directly recurse due to fake nodes in the path.

 ******************************************************************************/

JString
lldb::VarNode::GetFullNameWithCast
	(
	bool* isPointer
	)
	const
{
	JString s = GetFullNameForCFamilyLanguage(isPointer), t;
	if (GetType(&t))
	{
		s.Prepend(") ");
		s.Prepend(t);
		s.Prepend("(");
	}
	return s;
}

/******************************************************************************
 BuildTree (static)

 ******************************************************************************/

static const JRegex errorPrefixPattern("[0-9]+:[0-9]+:\\s+");

::VarNode*
lldb::VarNode::BuildTree
	(
	SBFrame& f,
	SBValue& v
	)
{
	JString name(v.GetName()), value(v.GetValue());
	bool isPointer = false;

	SBType t = v.GetType();
	if (t.IsPointerType())
	{
		isPointer = true;

		value.Prepend(") ");
		value.Prepend(t.GetDisplayTypeName());
		value.Prepend("(");
	}
	else if (v.GetType().GetTypeClass() == lldb::eTypeClassClass &&
			 !JString::IsEmpty(v.GetTypeName()) &&
			 name == v.GetTypeName())
	{
		// mark as "fake"
		name.Prepend("<");
		name.Append(">");
	}

	if (v.GetSummary() != nullptr)
	{
		if (!value.IsEmpty())
		{
			value += "  ";
		}
		value += v.GetSummary();
	}

	SBError e = v.GetError();
	if (e.Fail())
	{
		value = e.GetCString();

		JStringIterator iter(&value);
		if (iter.Next(errorPrefixPattern))
		{
			iter.RemoveAllPrev();
		}
		else
		{
			iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
		}

		if (iter.Next("\n"))
		{
			iter.SkipPrev();
			iter.RemoveAllNext();
		}
	}

	auto* node = jnew VarNode(t.GetDisplayTypeName(), name, value);
	if (isPointer)
	{
		node->MakePointer(true);
		return node;
	}

	if (v.MightHaveChildren())
	{
		const JSize count = v.GetNumChildren();
		for (JUnsignedOffset i=0; i<count; i++)
		{
			SBValue child = v.GetChildAtIndex(i, lldb::eDynamicDontRunTarget, true);
			if (child.IsValid())
			{
				::VarNode* n = BuildTree(f, child);
				node->Append(n);	// avoid automatic update
			}
		}
	}

	return node;
}
