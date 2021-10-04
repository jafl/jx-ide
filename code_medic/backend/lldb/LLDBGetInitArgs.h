/******************************************************************************
 LLDBGetInitArgs.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetInitArgs
#define _H_LLDBGetInitArgs

#include "GetInitArgsCmd.h"

class JXInputField;

class LLDBGetInitArgs : public GetInitArgsCmd
{
public:

	LLDBGetInitArgs(JXInputField* argInput);

	virtual	~LLDBGetInitArgs();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JXInputField*	itsArgInput;
};

#endif
