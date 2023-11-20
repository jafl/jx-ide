/******************************************************************************
 GetClassInfoCmd.h

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

	~GetClassInfoCmd() override;

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	const JUInt64	itsID;
};

};

#endif
