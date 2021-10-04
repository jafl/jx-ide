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

	virtual	~GetIDSizesCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

};

#endif
