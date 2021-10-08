/******************************************************************************
 XDGetInitArgs.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetInitArgs
#define _H_XDGetInitArgs

#include "GetInitArgsCmd.h"

class JXInputField;

class XDGetInitArgs : public GetInitArgsCmd
{
public:

	XDGetInitArgs(JXInputField* argInput);

	~XDGetInitArgs();

protected:

	void	HandleSuccess(const JString& data) override;
};

#endif
