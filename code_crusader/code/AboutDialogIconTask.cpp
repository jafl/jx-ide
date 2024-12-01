/******************************************************************************
 AboutDialogIconTask.cpp

	BASE CLASS = JXAnimationTask

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "AboutDialogIconTask.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXImageWidget.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jcore/jAssert.h>

#include "jcc_about_0.xpm"
#include "jcc_about_1.xpm"
#include "jcc_about_2.xpm"
#include "jcc_about_3.xpm"
#include "jcc_about_4.xpm"

static JXPM kAboutIcon[] =
{
	jcc_about_0, jcc_about_1, jcc_about_2, jcc_about_3, jcc_about_4
};

const JSize kAboutIconCount = sizeof(kAboutIcon)/sizeof(JXPM);

const JSize kMinPausePeriod  = 8000;	// milliseconds
const JSize kMaxPausePeriod  = 10000;	// milliseconds
const JSize kAnimationPeriod = 300;		// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

AboutDialogIconTask::AboutDialogIconTask
	(
	JXImageWidget* widget
	)
	:
	JXAnimationTask(widget)
{
	itsImageList = jnew JPtrArray<JXImage>(JPtrArrayT::kForgetAll, 3);

	JXImageCache* cache = widget->GetDisplay()->GetImageCache();
	for (JIndex i=1; i<=kAboutIconCount; i++)
	{
		itsImageList->Append(cache->GetImage(kAboutIcon[i-1]));
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

AboutDialogIconTask::~AboutDialogIconTask()
{
	jdelete itsImageList;
}

/******************************************************************************
 GetFrameCount (virtual protected)

 ******************************************************************************/

JSize
AboutDialogIconTask::GetFrameCount()
{
	return itsImageList->GetItemCount();
}

/******************************************************************************
 GetFrameTime (virtual protected)

 ******************************************************************************/

void
AboutDialogIconTask::GetFrameTime
	(
	const JIndex	frameIndex,
	Time*			tmin,
	Time*			tmax
	)
{
	if (frameIndex == 1)
	{
		*tmin = kMinPausePeriod;
		*tmax = kMaxPausePeriod;
	}
	else
	{
		*tmin = *tmax = kAnimationPeriod;
	}
}

/******************************************************************************
 GetFrame (virtual protected)

 ******************************************************************************/

JXImage*
AboutDialogIconTask::GetFrame
	(
	const JIndex frameIndex
	)
{
	return itsImageList->GetItem(frameIndex);
}
