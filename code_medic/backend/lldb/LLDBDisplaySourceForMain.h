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

	void	HandleSuccess(const JString& data) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;
};

#endif
