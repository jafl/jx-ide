/******************************************************************************
 GDBPlot2DCmd.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBPlot2DCmd
#define _H_GDBPlot2DCmd

#include "Plot2DCmd.h"

namespace gdb {

class Plot2DCmd : public ::Plot2DCmd
{
public:

	Plot2DCmd(Plot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	~Plot2DCmd() override;

	void	UpdateRange(const JIndex curveIndex,
						const JInteger min,const JInteger max) override;

protected:

	void	HandleSuccess(const JString& data) override;
};

};

#endif
