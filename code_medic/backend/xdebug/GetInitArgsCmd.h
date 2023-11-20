/******************************************************************************
 GetInitArgsCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetInitArgsCmd
#define _H_XDGetInitArgsCmd

#include <GetInitArgsCmd.h>

class JXInputField;

namespace xdebug {

class GetInitArgsCmd : public ::GetInitArgsCmd
{
public:

	GetInitArgsCmd(JXInputField* argInput);

	~GetInitArgsCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
