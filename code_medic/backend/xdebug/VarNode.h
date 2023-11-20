/******************************************************************************
 VarNode.h

	Copyright (C) 2008 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDVarNode
#define _H_XDVarNode

#include <VarNode.h>

namespace xdebug {

class VarNode : public ::VarNode
{
public:

	VarNode(const bool shouldUpdate = true);

	VarNode(JTreeNode* parent, const JString& name,
			  const JString& fullName, const JString& value);

	~VarNode() override;

	JString	GetFullName(bool* isPointer = nullptr) const override;

protected:

	void	NameChanged() override;

private:

	JString	itsFullName;
};

};

#endif
