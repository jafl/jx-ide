/******************************************************************************
 Plot2DCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_Plot2DCommand
#define _H_Plot2DCommand

#include "Command.h"
#include <jx-af/jcore/JArray.h>

class Plot2DDir;

class Plot2DCommand : public Command
{
public:

	Plot2DCommand(Plot2DDir* dir, JArray<JFloat>* x, JArray<JFloat>* y);

	virtual	~Plot2DCommand();

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
Plot2DCommand::GetDirector()
{
	return itsDirector;
}

/******************************************************************************
 GetX

 ******************************************************************************/

inline JArray<JFloat>*
Plot2DCommand::GetX()
{
	return itsX;
}

/******************************************************************************
 GetY

 ******************************************************************************/

inline JArray<JFloat>*
Plot2DCommand::GetY()
{
	return itsY;
}

#endif
