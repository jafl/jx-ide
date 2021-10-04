/******************************************************************************
 JVMGetInitArgsCmd.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetInitArgsCmd
#define _H_JVMGetInitArgsCmd

#include "GetInitArgsCmd.h"

class JXInputField;

namespace jvm {

class GetInitArgsCmd : public ::GetInitArgsCmd
{
public:

	GetInitArgsCmd(JXInputField* argInput);

	virtual	~GetInitArgsCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

};

#endif
