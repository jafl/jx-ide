/******************************************************************************
 JVMGetClassMethodsCmd.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetClassMethodsCmd
#define _H_JVMGetClassMethodsCmd

#include "Command.h"

namespace jvm {

class GetClassMethodsCmd : public Command
{
public:

	GetClassMethodsCmd(const JUInt64 id);

	~GetClassMethodsCmd();

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	const JUInt64	itsID;
};

};

#endif
