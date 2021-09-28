/******************************************************************************
 CMThreadNode.h

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMThreadNode
#define _H_CMThreadNode

#include <jx-af/jcore/JNamedTreeNode.h>

class CMThreadNode : public JNamedTreeNode
{
public:

	CMThreadNode(const JUInt64 id, const JString& name,
				 const JString& fileName, const JIndex lineIndex);

	virtual	~CMThreadNode();

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
CMThreadNode::GetID()
	const
{
	return itsID;
}

/******************************************************************************
 GetFile

	This returns the relative path provided by gdb.

 ******************************************************************************/

inline bool
CMThreadNode::GetFile
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
CMThreadNode::AppendThread
	(
	JTreeNode* node
	)
{
	Append(node);
	ShouldBeOpenable(true);
}

#endif
