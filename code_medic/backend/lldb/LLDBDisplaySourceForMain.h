/******************************************************************************
 LLDBDisplaySourceForMain.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBDisplaySourceForMain
#define _H_LLDBDisplaySourceForMain

#include "DisplaySourceForMainCmd.h"

class LLDBDisplaySourceForMain : public DisplaySourceForMainCmd
{
public:

	LLDBDisplaySourceForMain(SourceDirector* sourceDir);

	virtual	~LLDBDisplaySourceForMain();

protected:

	virtual void	HandleSuccess(const JString& data) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
};

#endif
