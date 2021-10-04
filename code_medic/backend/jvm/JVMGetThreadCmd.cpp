/******************************************************************************
 JVMGetThreadCmd.cpp

	BASE CLASS = GetThreadCmd

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreadCmd.h"
#include "ThreadsWidget.h"
#include "JVMLink.h"
#include "globals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

jvm::GetThreadCmd::GetThreadCmd
	(
	ThreadsWidget* widget
	)
	:
	::GetThreadCmd(JString("NOP", JString::kNoCopy), widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

jvm::GetThreadCmd::~GetThreadCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
jvm::GetThreadCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<Link*>(GetLink());
	GetWidget()->FinishedLoading(link->GetCurrentThreadID());
}
