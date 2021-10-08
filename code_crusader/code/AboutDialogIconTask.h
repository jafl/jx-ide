/******************************************************************************
 AboutDialogIconTask.h

	Interface for the AboutDialogIconTask class

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_AboutDialogIconTask
#define _H_AboutDialogIconTask

#include <jx-af/jx/JXAnimationTask.h>
#include <jx-af/jcore/JPtrArray.h>

class AboutDialogIconTask : public JXAnimationTask
{
public:

	AboutDialogIconTask(JXImageWidget* widget);

	~AboutDialogIconTask();

protected:

	virtual JSize		GetFrameCount();
	virtual void		GetFrameTime(const JIndex frameIndex,
									 Time* tmin, Time* tmax);
	virtual JXImage*	GetFrame(const JIndex frameIndex);

private:

	JPtrArray<JXImage>*	itsImageList;
};

#endif
