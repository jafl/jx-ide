/******************************************************************************
 LLDBPlot2DCommand.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBPlot2DCommand
#define _H_LLDBPlot2DCommand

#include "Plot2DCmd.h"

class LLDBPlot2DCommand : public Plot2DCmd
{
public:

	LLDBPlot2DCommand(Plot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~LLDBPlot2DCommand();

	void	UpdateRange(const JIndex curveIndex,
								const JInteger min,const JInteger max) override;

protected:

	void	HandleSuccess(const JString& data) override;

private:

	JIndex	itsCurveIndex;
};

#endif
