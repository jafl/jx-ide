/******************************************************************************
 XDVarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDVarNode
#define _H_XDVarNode

#include "VarNode.h"

class XDVarNode : public VarNode
{
public:

	XDVarNode(const bool shouldUpdate = true);

	XDVarNode(JTreeNode* parent, const JString& name,
			  const JString& fullName, const JString& value);

	virtual	~XDVarNode();

	virtual JString	GetFullName(bool* isPointer = nullptr) const override;

protected:

	virtual void	NameChanged() override;

private:

	JString	itsFullName;
};

#endif
