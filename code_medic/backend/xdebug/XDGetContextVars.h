/******************************************************************************
 XDGetContextVars.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetContextVars
#define _H_XDGetContextVars

#include "CMCommand.h"
#include <jx-af/jcore/jXMLUtil.h>	// need defn of xmlNode

class CMVarNode;

class XDGetContextVars : public CMCommand
{
public:

	XDGetContextVars(CMVarNode* rootNode, const JString& contextID);

	virtual	~XDGetContextVars();

	static void	BuildTree(const JSize depth, xmlNode* root, CMVarNode* varRoot);

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	CMVarNode*	itsRootNode;	// not owned
};

#endif