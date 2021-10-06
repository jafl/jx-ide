/******************************************************************************
 XDGetContextVars.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetContextVars
#define _H_XDGetContextVars

#include "Command.h"
#include <jx-af/jcore/jXMLUtil.h>	// need defn of xmlNode

class VarNode;

class XDGetContextVars : public Command
{
public:

	XDGetContextVars(VarNode* rootNode, const JString& contextID);

	virtual	~XDGetContextVars();

	static void	BuildTree(const JSize depth, xmlNode* root, VarNode* varRoot);

protected:

	void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned
};

#endif
