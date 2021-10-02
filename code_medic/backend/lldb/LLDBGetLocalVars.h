/******************************************************************************
 LLDBGetLocalVars.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetLocalVars
#define _H_LLDBGetLocalVars

#include "GetLocalVars.h"

class VarNode;

class LLDBGetLocalVars : public GetLocalVars
{
public:

	LLDBGetLocalVars(VarNode* rootNode);

	virtual	~LLDBGetLocalVars();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;	// not owned

private:

	void	CleanVarString(JString* s);
};

#endif
