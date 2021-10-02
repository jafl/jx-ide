/******************************************************************************
 JVMStackFrameNode.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMStackFrameNode
#define _H_JVMStackFrameNode

#include "StackFrameNode.h"

class JVMStackFrameNode : public StackFrameNode
{
public:

	JVMStackFrameNode(JTreeNode* parent,
					  const JUInt64 id, const JUInt64 classID,
					  const JUInt64 methodID, const JUInt64 codeOffset);

	virtual	~JVMStackFrameNode();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JUInt64	itsClassID;
	const JUInt64	itsMethodID;
	const JUInt64	itsCodeOffset;

private:

	bool	UpdateNodeName();
};

#endif
