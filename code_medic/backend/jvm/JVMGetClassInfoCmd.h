/******************************************************************************
 JVMGetClassInfoCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetClassInfoCmd
#define _H_JVMGetClassInfoCmd

#include "Command.h"

namespace jvm {

class GetClassInfoCmd : public Command
{
public:

	GetClassInfoCmd(const JUInt64 id);

	virtual	~GetClassInfoCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	const JUInt64	itsID;
};

};

#endif
