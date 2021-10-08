/******************************************************************************
 LLDBSymbolsLoadedTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBSymbolsLoadedTask
#define _H_LLDBSymbolsLoadedTask

#include <jx-af/jx/JXUrgentTask.h>
#include <jx-af/jcore/JString.h>

namespace lldb {

class SymbolsLoadedTask : public JXUrgentTask
{
public:

	SymbolsLoadedTask(const JString& fileName);

	~SymbolsLoadedTask() override;

	void	Perform() override;

private:

	JString	itsFileName;
};

};

#endif
