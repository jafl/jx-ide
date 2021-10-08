/******************************************************************************
 GDBGetInitArgsCmd.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetInitArgsCmd
#define _H_GDBGetInitArgsCmd

#include "GetInitArgsCmd.h"

class JXInputField;

namespace gdb {

class GetInitArgsCmd : public ::GetInitArgsCmd
{
public:

	GetInitArgsCmd(JXInputField* argInput);

	~GetInitArgsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JXInputField*	itsArgInput;
};

};

#endif
