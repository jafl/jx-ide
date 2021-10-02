/******************************************************************************
 GDBGetStackArguments.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStackArguments
#define _H_GDBGetStackArguments

#include "Command.h"

class JTree;

class GDBGetStackArguments : public Command
{
public:

	GDBGetStackArguments(JTree* tree);

	virtual	~GDBGetStackArguments();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;		// not owned
};

#endif
