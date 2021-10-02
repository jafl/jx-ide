/******************************************************************************
 GDBGetInitArgs.h

	Copyright (C) 2002 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetInitArgs
#define _H_GDBGetInitArgs

#include "GetInitArgs.h"

class JXInputField;

class GDBGetInitArgs : public GetInitArgs
{
public:

	GDBGetInitArgs(JXInputField* argInput);

	virtual	~GDBGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JXInputField*	itsArgInput;
};

#endif
