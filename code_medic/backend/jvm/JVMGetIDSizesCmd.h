/******************************************************************************
 JVMGetIDSizesCmd.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetIDSizesCmd
#define _H_JVMGetIDSizesCmd

#include "Command.h"

namespace jvm {

class GetIDSizesCmd : public Command
{
public:

	GetIDSizesCmd();

	~GetIDSizesCmd() override;

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
