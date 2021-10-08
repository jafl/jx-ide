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

	~DSSFinishSaveTask() override;

	void	Save(const JString& fullName) override;

private:

	TextDocument*	itsDoc;		// not owned
};

#endif
