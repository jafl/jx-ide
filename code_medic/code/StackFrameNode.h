/******************************************************************************
 StackFrameNode.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_StackFrameNode
#define _H_StackFrameNode

#include <jx-af/jcore/JNamedTreeNode.h>

class StackFrameNode : public JNamedTreeNode
{
public:

	StackFrameNode(JTreeNode* parent,
					  const JUInt64 id, const JString& name,
					  const JString& fileName, const JIndex lineIndex);

	~StackFrameNode() override;

	JUInt64	GetID() const;
	bool	GetFile(JString* fileName, JIndex* lineIndex) const;

	void	AppendArg(JTreeNode* node);

private:

	JUInt64	itsID;
	JString	itsFileName;	// relative path
	JIndex	itsLineIndex;
};


/******************************************************************************
 GetID

 ******************************************************************************/

inline JUInt64
StackFrameNode::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 GetFile

	This returns the relative path provided by gdb.

 ******************************************************************************/

inline bool
StackFrameNode::GetFile
	(
	JString*	fileName,
	JIndex*		lineIndex
	)
	const
{
	*fileName  = itsFileName;
	*lineIndex = itsLineIndex;
	return !itsFileName.IsEmpty() && itsLineIndex > 0;
}

/******************************************************************************
 AppendArg

 *****************************************************************************/

inline void
StackFrameNode::AppendArg
	(
	JTreeNode* node
	)
{
	Append(node);
	ShouldBeOpenable(true);
}

#endif
