/******************************************************************************
 JVMGetInitArgsCmd.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetInitArgsCmd
#define _H_JVMGetInitArgsCmd

#include "GetInitArgsCmd.h"

class JXInputField;

class JVMGetInitArgsCmd : public GetInitArgsCmd
{
public:

	JVMGetInitArgsCmd(JXInputField* argInput);

	virtual	~JVMGetInitArgsCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
