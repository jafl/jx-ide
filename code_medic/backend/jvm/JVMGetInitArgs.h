/******************************************************************************
 JVMGetInitArgs.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetInitArgs
#define _H_JVMGetInitArgs

#include "GetInitArgs.h"

class JXInputField;

class JVMGetInitArgs : public GetInitArgs
{
public:

	JVMGetInitArgs(JXInputField* argInput);

	virtual	~JVMGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
