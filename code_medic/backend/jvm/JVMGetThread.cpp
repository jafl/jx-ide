/******************************************************************************
 JVMGetThread.cpp

	BASE CLASS = GetThread

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetThread.h"
#include "ThreadsWidget.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThread::JVMGetThread
	(
	ThreadsWidget* widget
	)
	:
	GetThread(JString("NOP", JString::kNoCopy), widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThread::~JVMGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThread::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<JVMLink*>(GetLink());
	GetWidget()->FinishedLoading(link->GetCurrentThreadID());
}
