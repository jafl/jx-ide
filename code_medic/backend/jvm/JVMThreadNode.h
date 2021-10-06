/******************************************************************************
 JVMThreadNode.h

	Copyright (C) 2011 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMThreadNode
#define _H_JVMThreadNode

#include "ThreadNode.h"

namespace jvm {

class ThreadNode : public ::ThreadNode
{
public:

	enum
	{
		kRootThreadGroupID = 0	// 0 is reserved for null object
	};

	enum Type
	{
		kThreadType,
		kGroupType
	};

public:

	ThreadNode(const Type type, const JUInt64 id);
	ThreadNode(const JUInt64 id);
	ThreadNode(const ThreadNode& source);

	virtual	~ThreadNode();

	Type	GetType() const;
	void	FindParent(const JUInt64 id);

	static JListT::CompareResult
		CompareID(ThreadNode* const & t1, ThreadNode* const & t2);

protected:

	void	NameChanged() override;

private:

	const Type	itsType;
};

};

/******************************************************************************
 GetType

 ******************************************************************************/

inline jvm::ThreadNode::Type
jvm::ThreadNode::GetType()
	const
{
	return itsType;
}

#endif
