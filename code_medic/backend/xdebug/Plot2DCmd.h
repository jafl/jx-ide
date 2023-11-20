/******************************************************************************
 XDPlot2DCmd.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDPlot2DCmd
#define _H_XDPlot2DCmd

#include <Plot2DCmd.h>

namespace xdebug {

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
