/******************************************************************************
 DisplaySourceForMainCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDDisplaySourceForMainCmd
#define _H_XDDisplaySourceForMainCmd

#include <DisplaySourceForMainCmd.h>

namespace xdebug {

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
