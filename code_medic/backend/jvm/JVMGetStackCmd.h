/******************************************************************************
 JVMGetStackCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetStackCmd
#define _H_JVMGetStackCmd

#include "GetStackCmd.h"

namespace jvm {

class GetStackCmd : public ::GetStackCmd
{
public:

	GetStackCmd(JTree* tree, StackWidget* widget);

	virtual	~GetStackCmd();

	void	Starting() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
