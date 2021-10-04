/******************************************************************************
 Plot2DCmd.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_Plot2DCmd
#define _H_Plot2DCmd

#include "Command.h"
#include <jx-af/jcore/JArray.h>

class Plot2DDir;

class Plot2DCmd : public Command
{
public:

	Plot2DCmd(Plot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~Plot2DCmd();

	virtual void	UpdateRange(const JIndex curveIndex,
								const JInteger min,const JInteger max);

	Plot2DDir*	GetDirector();
	JArray<JFloat>*	GetX();
	JArray<JFloat>*	GetY();

private:

	Plot2DDir*	itsDirector;
	JArray<JFloat>*	itsX;
	JArray<JFloat>*	itsY;
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline Plot2DDir*
Plot2DCmd::GetDirector()
{
	return itsDirector;
}

/******************************************************************************
 GetX

 ******************************************************************************/

inline JArray<JFloat>*
Plot2DCmd::GetX()
{
	return itsX;
}

/******************************************************************************
 GetY

 ******************************************************************************/

inline JArray<JFloat>*
Plot2DCmd::GetY()
{
	return itsY;
}

#endif
