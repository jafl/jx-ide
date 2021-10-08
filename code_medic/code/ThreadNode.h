/******************************************************************************
 ThreadNode.h

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#ifndef _H_ThreadNode
#define _H_ThreadNode

#include <jx-af/jcore/JNamedTreeNode.h>

class ThreadNode : public JNamedTreeNode
{
public:

	ThreadNode(const JUInt64 id, const JString& name,
				const JString& fileName, const JIndex lineIndex);

	~ThreadNode() override;

	JUInt64	GetID() const;
	bool	GetFile(JString* fileName, JIndex* lineIndex) const;

	void	AppendThread(JTreeNode* node);

private:

	JUInt64	itsID;
	JString	itsFileName;	// relative path
	JIndex	itsLineIndex;
};


/******************************************************************************
 GetID

 ******************************************************************************/

inline JUInt64
ThreadNode::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 GetFile

	This returns the relative path provided by gdb.

 ******************************************************************************/

inline bool
ThreadNode::GetFile
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
 AppendThread

 *****************************************************************************/

inline void
ThreadNode::AppendThread
	(
	JTreeNode* node
	)
{
	Append(node);
	ShouldBeOpenable(true);
}

#endif
