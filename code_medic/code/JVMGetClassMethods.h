/******************************************************************************
 JVMGetClassMethods.h

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetClassMethods
#define _H_JVMGetClassMethods

#include "CMCommand.h"

class JVMGetClassMethods : public CMCommand
{
public:

	JVMGetClassMethods(const JUInt64 id);

	virtual	~JVMGetClassMethods();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	const JUInt64	itsID;
};

#endif
