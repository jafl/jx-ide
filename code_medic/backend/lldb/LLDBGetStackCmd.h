/******************************************************************************
 LLDBGetStackCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBGetStackCmd
#define _H_LLDBGetStackCmd

#include "GetStackCmd.h"

namespace lldb {

class GetStackCmd : public ::GetStackCmd
{
public:

	GetStackCmd(JTree* tree, StackWidget* widget);

	~GetStackCmd();

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
