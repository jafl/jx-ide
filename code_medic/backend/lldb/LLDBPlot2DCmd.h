/******************************************************************************
 LLDBPlot2DCmd.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBPlot2DCmd
#define _H_LLDBPlot2DCmd

#include "Plot2DCmd.h"

namespace lldb {

class Plot2DCmd : public ::Plot2DCmd
{
public:

	Plot2DCmd(Plot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	~Plot2DCmd();

	void	UpdateRange(const JIndex curveIndex,
						const JInteger min,const JInteger max) override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JIndex	itsCurveIndex;
};

};

#endif
