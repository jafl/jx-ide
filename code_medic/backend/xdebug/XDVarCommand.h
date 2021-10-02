/******************************************************************************
 XDVarCommand.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDVarCommand
#define _H_XDVarCommand

#include "VarCommand.h"

class VarNode;

class XDVarCommand : public VarCommand
{
public:

	XDVarCommand(const JString& cmd);

	virtual	~XDVarCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	VarNode*	itsRootNode;
};

#endif
