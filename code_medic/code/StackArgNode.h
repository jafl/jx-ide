/******************************************************************************
 StackArgNode.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_StackArgNode
#define _H_StackArgNode

#include <jx-af/jcore/JNamedTreeNode.h>

class StackFrameNode;

class StackArgNode : public JNamedTreeNode
{
public:

	StackArgNode(StackFrameNode* parent,
				   const JString& name, const JString& value);

	~StackArgNode();

	const JString&	GetValue() const;

private:

	JString	itsValue;
};


/******************************************************************************
 GetValue

 ******************************************************************************/

inline const JString&
StackArgNode::GetValue()
	const
{
	return itsValue;
}

#endif
