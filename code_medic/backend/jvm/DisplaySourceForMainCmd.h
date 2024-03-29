/******************************************************************************
 DisplaySourceForMainCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMDisplaySourceForMainCmd
#define _H_JVMDisplaySourceForMainCmd

#include <DisplaySourceForMainCmd.h>

namespace jvm {

class DisplaySourceForMainCmd : public ::DisplaySourceForMainCmd
{
public:

	DisplaySourceForMainCmd(SourceDirector* sourceDir);

	~DisplaySourceForMainCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
