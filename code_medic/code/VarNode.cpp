/******************************************************************************
 VarNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "VarNode.h"
#include "VarCmd.h"
#include "InitVarNodeTask.h"
#include "globals.h"
#include "sharedUtil.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

VarNode::VarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	JNamedTreeNode(nullptr, JString::empty),
	itsShouldListenToLinkFlag(shouldUpdate)
{
	VarTreeNodeX();
	itsValidFlag = true;

	if (itsShouldListenToLinkFlag)
	{
		ListenTo(GetLink());
	}
}

VarNode::VarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	value
	)
	:
	JNamedTreeNode(nullptr, name.IsEmpty() ? JString(" ", JString::kNoCopy) : name, false),
	itsShouldListenToLinkFlag(false),
	itsValue(value)
{
	VarTreeNodeX();

	if (parent != nullptr)
	{
		parent->Append(this);
		if (parent->IsRoot() && !name.IsEmpty())
		{
			auto* task = jnew InitVarNodeTask(this);
			assert( task != nullptr );
			task->Go();
		}
	}
}

// private

void
VarNode::VarTreeNodeX()
{
	itsValueCommand		= nullptr;
	itsIsPointerFlag	= false;
	itsValidFlag		= !itsValue.IsEmpty();
	itsNewValueFlag		= false;
	itsContentCommand	= nullptr;

	itsShouldUpdateFlag = false;	// window is always initially hidden
	itsNeedsUpdateFlag  = false;

	itsBase               = 0;
	itsCanConvertBaseFlag = false;
	itsOrigValue          = nullptr;
	ConvertToBase();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

VarNode::~VarNode()
{
	jdelete itsValueCommand;
	jdelete itsContentCommand;
	jdelete itsOrigValue;
}

/******************************************************************************
 SetValue

 ******************************************************************************/

void
VarNode::SetValue
	(
	const JString& value
	)
{
	itsNewValueFlag = !itsValue.IsEmpty() && value != itsValue;
	itsValue        = value;	// set *after* checking value

	itsValue.TrimWhitespace();
	if (itsOrigValue != nullptr)
	{
		itsOrigValue->Clear();
	}
	ConvertToBase();

	JTree* tree;
	if (!itsCanConvertBaseFlag && GetTree(&tree))
	{
		tree->BroadcastChange(this);
	}
}

/******************************************************************************
 ConvertToBase (private)

 ******************************************************************************/

static const JRegex valuePattern = "^(-?[[:digit:]]+)([[:space:]]+'.*)?$";

struct SpecialCharInfo
{
	JUtf8Byte			c;
	const JUtf8Byte*	s;
};

static const SpecialCharInfo kSpecialCharInfo[] =
{
{ '\a', "\\a"  },
{ '\b', "\\b"  },
{ '\f', "\\f"  },
{ '\n', "\\n"  },
{ '\r', "\\r"  },
{ '\t', "\\t"  },
{ '\v', "\\v"  },
{ '\\', "\\\\" },
{ '\'', "\\'"  },
{ '"',  "\\\"" },
{ '\0', "\\0"  }
};

void
VarNode::ConvertToBase()
{
	itsCanConvertBaseFlag = false;
	if (itsOrigValue != nullptr && !itsOrigValue->IsEmpty())
	{
		JTree* tree;
		if (itsValue != *itsOrigValue && GetTree(&tree))
		{
			tree->BroadcastChange(this);
		}
		itsValue = *itsOrigValue;
	}

	if (itsBase == 0 || itsIsPointerFlag)
	{
		return;		// avoid JRegex match
	}

	const JStringMatch m = valuePattern.Match(itsValue, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
	{
		JString vStr = m.GetSubstring(1);

		JUInt v;
		if (vStr.GetFirstCharacter() == '-')
		{
			JInteger v1;
			itsCanConvertBaseFlag = itsBase != 1 &&
				vStr.ConvertToInteger(&v1, 10);
			if (itsCanConvertBaseFlag)
			{
				v = (JUInt) v1;
			}
		}
		else
		{
			itsCanConvertBaseFlag = vStr.ConvertToUInt(&v, vStr.GetFirstCharacter() == '0' ? 8 : 10) &&
				(itsBase != 1 || v <= 255);
		}

		if (itsCanConvertBaseFlag)
		{
			// save value for when base reset to "default"

			itsOrigValue = jnew JString(itsValue);
			assert( itsOrigValue != nullptr );

			// replace only the value, preserving whatever else is there

			if (itsBase == 1)
			{
				assert( 0 <= v && v <= 255 );

				vStr  = JString(v, JString::kBase16, true);
				vStr += " '";

				bool found = false;
				for (const auto& info : kSpecialCharInfo)
				{
					if (JUtf8Character(v) == info.c)
					{
						vStr += info.s;
						found = true;
						break;
					}
				}
				if (!found)
				{
					vStr.Append(JUtf8Character(v));
				}

				vStr.Append("'");
			}
			else
			{
				vStr = JString(v, (JString::Base) itsBase, true);
				if (itsBase == 8)
				{
					vStr.Prepend("0");
				}
			}

			const JCharacterRange r = m.GetCharacterRange(1);
			JStringIterator iter(&itsValue, JStringIterator::kStartBeforeChar, r.first);
			iter.BeginMatch();
			iter.SkipNext(r.GetCount());
			iter.FinishMatch();
			iter.ReplaceLastMatch(vStr);
			iter.Invalidate();

			JTree* tree;
			if (GetTree(&tree))
			{
				tree->BroadcastChange(this);
			}
		}
	}
}

/******************************************************************************
 SetValid

 ******************************************************************************/

void
VarNode::SetValid
	(
	const bool valid
	)
{
	if (valid != itsValidFlag)
	{
		itsValidFlag = valid;

		JTree* tree;
		if (GetTree(&tree))
		{
			tree->BroadcastChange(this);
		}
	}

	// pointer values update their own contents

	if (!valid || !itsIsPointerFlag)
	{
		const JSize count = GetChildCount();
		for (JIndex i=1; i<=count; i++)
		{
			(GetVarChild(i))->SetValid(valid);
		}
	}
}

/******************************************************************************
 GetFontStyle

 ******************************************************************************/

JFontStyle
VarNode::GetFontStyle()
{
	if (!itsValidFlag)
	{
		return JFontStyle(JColorManager::GetGrayColor(50));
	}
	else if (itsNewValueFlag)
	{
		return JFontStyle(JColorManager::GetBlueColor());
	}
	else
	{
		return JFontStyle();
	}
}

// static

JFontStyle
VarNode::GetFontStyle
	(
	const bool isValid,
	const bool isNew
	)
{
	if (!isValid)
	{
		return JFontStyle(JColorManager::GetGrayColor(50));
	}
	else if (isNew)
	{
		return JFontStyle(JColorManager::GetBlueColor());
	}
	else
	{
		return JFontStyle();
	}
}

/******************************************************************************
 OKToOpen (virtual)

	Update ourselves if we are being opened.

 ******************************************************************************/

bool
VarNode::OKToOpen()
	const
{
	if (itsIsPointerFlag && !HasChildren())
	{
		const_cast<VarNode*>(this)->UpdateContent();
	}

	return JNamedTreeNode::OKToOpen();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
VarNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetLink() && ShouldUpdate(message))
	{
		// root node only

		if (itsShouldUpdateFlag)
		{
			UpdateChildren();
		}
		else
		{
			itsNeedsUpdateFlag = true;
		}
	}

	else if (sender == itsValueCommand &&
			 message.Is(VarCmd::kValueUpdated))
	{
		const auto& info =
			dynamic_cast<const VarCmd::ValueMessage&>(message);

		SetValid(true);
		Update(info.GetRootNode());
	}
	else if (sender == itsValueCommand &&
			 message.Is(VarCmd::kValueFailed))
	{
		SetValue(itsValueCommand->GetData());
		MakePointer(false);
	}

	else if (sender == itsContentCommand &&
			 message.Is(VarCmd::kValueUpdated))
	{
		const auto& info =
			dynamic_cast<const VarCmd::ValueMessage&>(message);

		VarNode* root = info.GetRootNode();

		// value or pointer

		if (!root->HasChildren())
		{
			DeleteAllChildren();
			VarNode* child = GetLink()->CreateVarNode(this, root->GetName(),
															root->GetFullName(),
															root->GetValue());
			assert( child != nullptr );
			child->MakePointer(root->itsIsPointerFlag);
		}

		// struct or static array

		else if (SameElements(root))
		{
			MergeChildren(root);
		}
		else
		{
			StealChildren(root);
		}
	}
	else if (sender == itsContentCommand &&
			 message.Is(VarCmd::kValueFailed))
	{
		DeleteAllChildren();
		VarNode* child = GetLink()->CreateVarNode(this, JString::empty, JString::empty, itsContentCommand->GetData());
		assert( child != nullptr );
		child->SetValid(false);
	}

	else
	{
		JNamedTreeNode::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
VarNode::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (itsShouldListenToLinkFlag && !IsShuttingDown())
	{
		ListenTo(GetLink());
	}

	JNamedTreeNode::ReceiveGoingAway(sender);
}

/******************************************************************************
 ShouldUpdate (static)

 ******************************************************************************/

bool
VarNode::ShouldUpdate
	(
	const Message& message
	)
{
	if (message.Is(Link::kProgramStopped))
	{
		const auto& info =
			dynamic_cast<const Link::ProgramStopped&>(message);

		const Location* loc;
		return info.GetLocation(&loc) && !(loc->GetFileName()).IsEmpty();
	}
	else
	{
		return message.Is(Link::kValueChanged)      ||
					message.Is(Link::kThreadChanged)     ||
					message.Is(Link::kFrameChanged)      ||
					message.Is(Link::kCoreLoaded)        ||
					message.Is(Link::kCoreCleared)       ||
					message.Is(Link::kAttachedToProcess) ||
					message.Is(Link::kDetachedFromProcess);
	}
}

/******************************************************************************
 NameChanged (virtual protected)

 ******************************************************************************/

void
VarNode::NameChanged()
{
	if (!HasTree() || GetDepth() != 1)
	{
		// parser may be busy; don't interfere
		return;
	}

	const JString& name = GetName();

	JString n = name;
	TrimExpression(&n);
	if (n != name)
	{
		SetName(n);
		return;		// calls us again
	}

	UpdateValue();
}

/******************************************************************************
 TrimExpression (static)

 ******************************************************************************/

void
VarNode::TrimExpression
	(
	JString* s
	)
{
	s->TrimWhitespace();
	while (!s->IsEmpty() &&
		   s->GetFirstCharacter() == '(' &&
		   s->GetLastCharacter()  == ')')
	{
		JStringIterator iter(s, JStringIterator::kStartAfterChar, 1);
		JUtf8Character c;
		if (!BalanceForward(kCLang, &iter, &c) || !iter.AtEnd())
		{
			break;
		}

		iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
		iter.RemoveNext();
		iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		iter.RemovePrev();

		s->TrimWhitespace();
	}
}

/******************************************************************************
 ShouldUpdate

 ******************************************************************************/

void
VarNode::ShouldUpdate
	(
	const bool update
	)
{
	itsShouldUpdateFlag = update;
	if (itsShouldUpdateFlag && itsNeedsUpdateFlag)
	{
		UpdateChildren();
	}
}

/******************************************************************************
 UpdateChildren (private)

 ******************************************************************************/

void
VarNode::UpdateChildren()
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		(GetVarChild(i))->UpdateValue();
	}

	itsNeedsUpdateFlag = false;
}

/******************************************************************************
 UpdateValue

	Called by self and *GetLocalVarsCmd.

 ******************************************************************************/

void
VarNode::UpdateValue()
{
	if (HasTree() && GetDepth() == 1 && !(GetName()).IsEmpty())
	{
		jdelete itsValueCommand;

		const JString expr = GetFullName();
		itsValueCommand = GetLink()->CreateVarValueCmd(expr);
		ListenTo(itsValueCommand);

		SetValid(false);
		itsValueCommand->Command::Send();
	}
	else
	{
		SetValue(JString::empty);
	}
}

/******************************************************************************
 UpdateValue

	Called by *GetLocalVarsCmd.

 ******************************************************************************/

void
VarNode::UpdateValue
	(
	VarNode* root
	)
{
	SetValid(true);
	Update(root);
}

/******************************************************************************
 UpdateFailed

	Called by *GetLocalVarsCmd.

 ******************************************************************************/

void
VarNode::UpdateFailed
	(
	const JString& data
	)
{
	SetValue(data);
	MakePointer(false);
	SetValid(false);		// faster after deleting children
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
VarNode::Update
	(
	VarNode* node
	)
{
	SetValue(node->GetValue());
	SetValid(node->ValueIsValid());

	// value or pointer

	if (!node->HasChildren())
	{
		MakePointer(node->itsIsPointerFlag);
		if (itsIsPointerFlag && HasChildren())
		{
			UpdateContent();
		}
	}

	// struct or static array

	else if (SameElements(node))
	{
		MakePointer(node->itsIsPointerFlag, false);
		MergeChildren(node);
	}
	else
	{
		MakePointer(node->itsIsPointerFlag, false);
		StealChildren(node);
	}
}

/******************************************************************************
 SameElements (private)

	Returns true if the given node has the same number of children with
	the same names as we do.

 ******************************************************************************/

bool
VarNode::SameElements
	(
	const VarNode* node
	)
	const
{
	bool sameElements = false;

	if (node->GetChildCount() == GetChildCount())
	{
		sameElements = true;

		const JSize count = GetChildCount();
		for (JIndex i=1; i<=count; i++)
		{
			if ((GetVarChild(i))->GetName() != (node->GetVarChild(i))->GetName())
			{
				sameElements = false;
				break;
			}
		}
	}

	return sameElements;
}

/******************************************************************************
 MakePointer

 ******************************************************************************/

void
VarNode::MakePointer
	(
	const bool pointer,
	const bool adjustOpenable
	)
{
	itsIsPointerFlag = pointer;

	if (adjustOpenable)
	{
		ShouldBeOpenable(itsIsPointerFlag);
	}
}

/******************************************************************************
 UpdateContent (private)

 ******************************************************************************/

void
VarNode::UpdateContent()
{
	assert( itsIsPointerFlag );

	jdelete itsContentCommand;

	const JString expr = GetFullName();
	itsContentCommand = GetLink()->CreateVarContentCmd(expr);
	ListenTo(itsContentCommand);

	itsContentCommand->Command::Send();
}

/******************************************************************************
 StealChildren (private)

 ******************************************************************************/

void
VarNode::StealChildren
	(
	VarNode* node
	)
{
	DeleteAllChildren();

	const JSize count = node->GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		Append(node->GetChild(1));
	}
}

/******************************************************************************
 MergeChildren (private)

 ******************************************************************************/

void
VarNode::MergeChildren
	(
	VarNode* node
	)
{
	const JSize count = GetChildCount();
	assert( count == node->GetChildCount() );

	for (JIndex i=1; i<=count; i++)
	{
		(GetVarChild(i))->Update(node->GetVarChild(i));
	}
}

/******************************************************************************
 GetLastChild

 ******************************************************************************/

void
VarNode::GetLastChild
	(
	VarNode** child
	)
{
	*child = this;

	const JSize count = GetChildCount();
	if (count > 0)
	{
		GetVarChild(count)->GetLastChild(child);
	}
}

/******************************************************************************
 GetVarParent

 ******************************************************************************/

VarNode*
VarNode::GetVarParent()
{
	JTreeNode* p = GetParent();
	auto* n = dynamic_cast<VarNode*>(p);
	assert( n != nullptr );
	return n;
}

const VarNode*
VarNode::GetVarParent()
	const
{
	const JTreeNode* p = GetParent();
	const auto* n = dynamic_cast<const VarNode*>(p);
	assert( n != nullptr );
	return n;
}

bool
VarNode::GetVarParent
	(
	VarNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
	{
		*parent = dynamic_cast<VarNode*>(p);
		assert( *parent != nullptr );
		return true;
	}
	else
	{
		*parent = nullptr;
		return false;
	}
}

bool
VarNode::GetVarParent
	(
	const VarNode** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
	{
		*parent = dynamic_cast<const VarNode*>(p);
		assert( *parent != nullptr );
		return true;
	}
	else
	{
		*parent = nullptr;
		return false;
	}
}

/******************************************************************************
 GetVarChild

 ******************************************************************************/

VarNode*
VarNode::GetVarChild
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<VarNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

const VarNode*
VarNode::GetVarChild
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const VarNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

/******************************************************************************
 GetFullNameForCFamilyLanguage (protected)

 ******************************************************************************/

JString
VarNode::GetFullNameForCFamilyLanguage
	(
	bool* isPointer
	)
	const
{
	JString str;
	if (IsRoot())
	{
		return str;
	}

	const VarNode* parent = GetVarParent();
	const JString& name     = GetName();
	if (parent->IsRoot())
	{
		str = "(" + name + ")";
	}
	else if (name.IsEmpty())
	{
		JIndex i;
		const bool found = parent->FindChild(this, &i);
		assert( found );
		str = parent->GetFullName(isPointer);
		if (!str.StartsWith("(") || !str.EndsWith(")"))
		{
			str.Prepend("(");
			str.Append(")");
		}
		str += "[" + JString((JUInt64) i-1) + "]";
	}
	else if (name.StartsWith("<"))
	{
		if (isPointer != nullptr)
		{
			*isPointer = parent->IsPointer();
		}
		str = parent->GetFullName(isPointer);
	}
	else if (name.StartsWith("["))
	{
		str = parent->GetFullName(isPointer) + name;
	}
	else if (name.StartsWith("*"))
	{
		str = parent->GetPathForCFamilyLanguage() + "(" + name + ")";
	}
	else
	{
		str = name;
		if (str.StartsWith("static "))
		{
			JStringIterator iter(&str);
			iter.RemoveNext(7);
		}
		str.Prepend(parent->GetPathForCFamilyLanguage());
	}

	return str;
}

/******************************************************************************
 GetPathForCFamilyLanguage (protected)

 ******************************************************************************/

JString
VarNode::GetPathForCFamilyLanguage()
	const
{
	JString str;
	if (IsRoot())
	{
		return str;
	}

	bool isPointer = IsPointer();
	str  = GetFullName(&isPointer);
	str += isPointer ? "->" : ".";
	return str;
}
