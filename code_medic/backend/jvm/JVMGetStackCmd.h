/******************************************************************************
 JVMGetStackCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetStackCmd
#define _H_JVMGetStackCmd

#include "GetStackCmd.h"

class JVMGetStackCmd : public GetStackCmd
{
public:

	JVMGetStackCmd(JTree* tree, StackWidget* widget);

	virtual	~JVMGetStackCmd();

	virtual void	Starting() override;

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
