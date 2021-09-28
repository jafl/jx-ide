/******************************************************************************
 JVMGetClassInfo.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetClassInfo
#define _H_JVMGetClassInfo

#include "CMCommand.h"

class JVMGetClassInfo : public CMCommand
{
public:

	JVMGetClassInfo(const JUInt64 id);

	virtual	~JVMGetClassInfo();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	const JUInt64	itsID;
};

#endif
