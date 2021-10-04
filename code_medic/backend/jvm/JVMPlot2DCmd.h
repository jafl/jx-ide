/******************************************************************************
 JVMPlot2DCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMPlot2DCmd
#define _H_JVMPlot2DCmd

#include "Plot2DCmd.h"

class JVMPlot2DCmd : public Plot2DCmd
{
public:

	JVMPlot2DCmd(Plot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~JVMPlot2DCmd();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
