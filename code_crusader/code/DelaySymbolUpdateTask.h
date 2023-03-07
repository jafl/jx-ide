/******************************************************************************
 DelaySymbolUpdateTask.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_DelaySymbolUpdateTask
#define _H_DelaySymbolUpdateTask

#include <jx-af/jx/JXIdleTask.h>

class ProjectDocument;

class DelaySymbolUpdateTask : public JXIdleTask
{
public:

	DelaySymbolUpdateTask(ProjectDocument* projDoc);

	~DelaySymbolUpdateTask() override;

protected:

	void	Perform(const Time delta) override;

private:

	ProjectDocument*	itsProjDoc;
};

#endif
