/******************************************************************************
 CBDelaySymbolUpdateTask.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDelaySymbolUpdateTask
#define _H_CBDelaySymbolUpdateTask

#include <jx-af/jx/JXIdleTask.h>

class CBProjectDocument;

class CBDelaySymbolUpdateTask : public JXIdleTask
{
public:

	CBDelaySymbolUpdateTask(CBProjectDocument* projDoc);

	virtual ~CBDelaySymbolUpdateTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	CBProjectDocument*	itsProjDoc;
};

#endif
