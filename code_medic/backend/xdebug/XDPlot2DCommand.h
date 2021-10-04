/******************************************************************************
 XDPlot2DCommand.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDPlot2DCommand
#define _H_XDPlot2DCommand

#include "Plot2DCmd.h"

class XDPlot2DCommand : public Plot2DCmd
{
public:

	XDPlot2DCommand(Plot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~XDPlot2DCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;
};

#endif
