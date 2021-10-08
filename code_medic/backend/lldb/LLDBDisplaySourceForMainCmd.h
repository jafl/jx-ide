/******************************************************************************
 LLDBDisplaySourceForMainCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBDisplaySourceForMainCmd
#define _H_LLDBDisplaySourceForMainCmd

#include "DisplaySourceForMainCmd.h"

namespace lldb {

class DisplaySourceForMainCmd : public ::DisplaySourceForMainCmd
{
public:

	DisplaySourceForMainCmd(SourceDirector* sourceDir);

	~DisplaySourceForMainCmd();

protected:

	void	HandleSuccess(const JString& data) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;
};

};

#endif
