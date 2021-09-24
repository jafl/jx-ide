/******************************************************************************
 LLDBSymbolsLoadedTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBSymbolsLoadedTask
#define _H_LLDBSymbolsLoadedTask

#include <JXUrgentTask.h>
#include <JString.h>

class LLDBSymbolsLoadedTask : public JXUrgentTask
{
public:

	LLDBSymbolsLoadedTask(const JString& fileName);

	virtual ~LLDBSymbolsLoadedTask();

	virtual void	Perform() override;

private:

	JString	itsFileName;
};

#endif
