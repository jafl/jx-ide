/******************************************************************************
 GetInitArgsCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetInitArgsCmd
#define _H_LLDBGetInitArgsCmd

#include <GetInitArgsCmd.h>

class JXInputField;

namespace lldb {

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
