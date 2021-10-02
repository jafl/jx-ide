/******************************************************************************
 XDGetInitArgs.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetInitArgs
#define _H_XDGetInitArgs

#include "GetInitArgs.h"

class JXInputField;

class XDGetInitArgs : public GetInitArgs
{
public:

	XDGetInitArgs(JXInputField* argInput);

	virtual	~XDGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
