/******************************************************************************
 JVMPlot2DCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMPlot2DCmd
#define _H_JVMPlot2DCmd

#include <Plot2DCmd.h>

namespace jvm {

class Plot2DCmd : public ::Plot2DCmd
{
public:

	Plot2DCmd(Plot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	~Plot2DCmd() override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
