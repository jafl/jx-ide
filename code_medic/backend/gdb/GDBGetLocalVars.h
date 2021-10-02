/******************************************************************************
 GDBGetLocalVars.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetLocalVars
#define _H_GDBGetLocalVars

#include "GetLocalVars.h"

class VarNode;

class GDBGetLocalVars : public GetLocalVars
{
public:

	GDBGetLocalVars(VarNode* rootNode);

	virtual	~GDBGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned

private:

	void	CleanVarString(JString* s);
};

#endif
