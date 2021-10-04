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

JVMGetThreadCmd::JVMGetThreadCmd
	(
	ThreadsWidget* widget
	)
	:
	GetThreadCmd(JString("NOP", JString::kNoCopy), widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThreadCmd::~JVMGetThreadCmd()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreadCmd::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<JVMLink*>(GetLink());
	GetWidget()->FinishedLoading(link->GetCurrentThreadID());
}
