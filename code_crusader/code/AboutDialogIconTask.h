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

	~AboutDialogIconTask() override;

protected:

	JSize		GetFrameCount() override;
	void		GetFrameTime(const JIndex frameIndex,
							 Time* tmin, Time* tmax) override;
	JXImage*	GetFrame(const JIndex frameIndex) override;

private:

	JPtrArray<JXImage>*	itsImageList;
};

#endif
