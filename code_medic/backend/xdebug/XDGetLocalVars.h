/******************************************************************************
 XDGetLocalVars.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetLocalVars
#define _H_XDGetLocalVars

#include "GetLocalVarsCmd.h"

class VarNode;

class XDGetLocalVars : public GetLocalVarsCmd
{
public:

	XDGetLocalVars(VarNode* rootNode);

	~XDGetLocalVars();

protected:

	void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned
};

#endif
