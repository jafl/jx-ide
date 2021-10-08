/******************************************************************************
 DSSFinishSaveTask.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_DSSFinishSaveTask
#define _H_DSSFinishSaveTask

#include <jx-af/jx/JXDirectSaveAction.h>

class TextDocument;

class DSSFinishSaveTask : public JXDirectSaveAction
{
public:

	DSSFinishSaveTask(TextDocument* doc);

	~DSSFinishSaveTask();

	virtual void	Save(const JString& fullName);

private:

	TextDocument*	itsDoc;		// not owned
};

#endif
