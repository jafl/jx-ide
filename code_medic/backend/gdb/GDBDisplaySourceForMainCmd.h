/******************************************************************************
 GDBDisplaySourceForMainCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBDisplaySourceForMainCmd
#define _H_GDBDisplaySourceForMainCmd

#include "DisplaySourceForMainCmd.h"

class GDBDisplaySourceForMainCmd : public DisplaySourceForMainCmd
{
public:

	GDBDisplaySourceForMainCmd(SourceDirector* sourceDir);

	virtual	~GDBDisplaySourceForMainCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsHasCoreFlag;
	JIndex	itsNextCmdIndex;
};

#endif
