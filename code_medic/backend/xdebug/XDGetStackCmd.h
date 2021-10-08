/******************************************************************************
 XDGetStackCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDGetStackCmd
#define _H_XDGetStackCmd

#include "GetStackCmd.h"

namespace xdebug {

class GetStackCmd : public ::GetStackCmd
{
public:

	GetStackCmd(JTree* tree, StackWidget* widget);

	~GetStackCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
