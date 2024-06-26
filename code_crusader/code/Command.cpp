/******************************************************************************
 Command.cpp

	Executes a sequence of commands from CommandManager.

	If multiple sequences of commands are to be executed, each sequence is
	delimited by a blank (nullptr) command.  This allows us to skip to the
	next complete sequence if a command returns an error.

	BASE CLASS = virtual JBroadcaster

	Copyright © 2002-2018 by John Lindal.

 ******************************************************************************/

#include "Command.h"
#include "CommandManager.h"
#include "ProjectDocument.h"
#include "CompileDocument.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXDeleteObjectTask.h>
#include <jx-af/jcore/JThisProcess.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* Command::kFinished = "Finished::Command";

/******************************************************************************
 Constructor

 ******************************************************************************/

Command::Command
	(
	const JString&		path,
	const bool			refreshVCSStatusWhenFinished,
	const bool			beepWhenFinished,
	ProjectDocument*	projDoc
	)
	:
	itsProjDoc(projDoc),
	itsCmdPath(path),
	itsOutputDoc(nullptr),
	itsBeepFlag(beepWhenFinished),
	itsRefreshVCSStatusFlag(refreshVCSStatusWhenFinished),
	itsInQueueFlag(false),
	itsSuccessFlag(true),
	itsCancelledFlag(false),
	itsMakeDependCmd(nullptr),
	itsBuildOutputDoc(nullptr),
	itsRunOutputDoc(nullptr),
	itsParent(nullptr),
	itsCallParentProcessFinishedFlag(true)
{
	assert( JIsAbsolutePath(path) );

	itsCmdList = jnew JArray<CmdInfo>;

	if (itsProjDoc != nullptr)
	{
		ClearWhenGoingAway(itsProjDoc, &itsProjDoc);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Command::~Command()
{
	if (itsParent != nullptr && itsParent->itsBuildOutputDoc == nullptr)
	{
		itsParent->itsBuildOutputDoc = itsBuildOutputDoc;
	}
	if (itsParent != nullptr && itsParent->itsRunOutputDoc == nullptr)
	{
		itsParent->itsRunOutputDoc = itsRunOutputDoc;
	}

	// only refresh VCS status when all finished, since may be expensive

	if (itsParent != nullptr && itsRefreshVCSStatusFlag)
	{
		itsParent->itsRefreshVCSStatusFlag = true;
	}
	else if (itsRefreshVCSStatusFlag)
	{
		GetDocumentManager()->RefreshVCSStatus();
	}

	if (itsParent != nullptr && itsCallParentProcessFinishedFlag)
	{
		itsParent->ProcessFinished(itsSuccessFlag, itsCancelledFlag);
	}
	Broadcast(Finished(itsSuccessFlag, itsCancelledFlag));

	const JSize count = itsCmdList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		CmdInfo info = itsCmdList->GetItem(i);
		info.Free(true);
	}
	jdelete itsCmdList;

	if (itsParent == nullptr)
	{
		FinishWindow(&itsBuildOutputDoc);
		FinishWindow(&itsRunOutputDoc);
	}
}

/******************************************************************************
 DeleteThis (private)

 ******************************************************************************/

void
Command::DeleteThis()
{
	JXDeleteObjectTask<Command>::Delete(this);

	if (itsParent == nullptr)
	{
		FinishWindow(&itsBuildOutputDoc);
		FinishWindow(&itsRunOutputDoc);
	}
}

/******************************************************************************
 FinishWindow (private)

 ******************************************************************************/

void
Command::FinishWindow
	(
	ExecOutputDocument** doc
	)
{
	if (*doc != nullptr)
	{
		(**doc).DecrementUseCount();
		*doc = nullptr;
	}
}

/******************************************************************************
 Add

 ******************************************************************************/

bool
Command::Add
	(
	const JPtrArray<JString>&	cmdArgs,
	const JPtrArray<JString>&	fullNameList,
	const JArray<JIndex>&		lineIndexList,
	FunctionStack*				fnStack
	)
{
	const JString* firstArg = cmdArgs.GetFirstItem();
	if (firstArg->GetFirstCharacter() == '&')
	{
		assert( fnStack != nullptr );

		// check for re-used command name

		const JUtf8Byte* cmdName = firstArg->GetBytes()+1;

		const JSize cmdCount = fnStack->GetItemCount();
		for (JIndex j=1; j<=cmdCount; j++)
		{
			if (strcmp(cmdName, fnStack->Peek(j)) == 0)
			{
				ReportInfiniteLoop(*fnStack, j);
				return false;
			}
		}

		// prepare cmd for execution later

		fnStack->Push(cmdName);

		CommandManager* mgr =
			(itsProjDoc != nullptr ? itsProjDoc->GetCommandManager() : GetCommandManager());
		Command* cmdObj;
		CommandManager::CmdInfo* cmdInfo;
		if (mgr->Prepare(JString(cmdName, JString::kNoCopy),
						 itsProjDoc, fullNameList, lineIndexList,
						 &cmdObj, &cmdInfo, fnStack))
		{
			cmdObj->SetParent(this);
			itsCmdList->AppendItem(CmdInfo(nullptr, cmdObj, cmdInfo, false));
		}
		else
		{
			return false;
		}

		fnStack->Pop();
	}
	else
	{
		auto* args = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		args->CopyObjects(cmdArgs, JPtrArrayT::kDeleteAll, false);

		itsCmdList->AppendItem(CmdInfo(args, nullptr, nullptr, false));
	}

	return true;
}

/******************************************************************************
 Add

 ******************************************************************************/

void
Command::Add
	(
	Command*						subCmd,
	const CommandManager::CmdInfo&	cmdInfo
	)
{
	subCmd->SetParent(this);

	auto* info = jnew CommandManager::CmdInfo;
	*info      = cmdInfo.Copy();

	itsCmdList->AppendItem(CmdInfo(nullptr, subCmd, info, false));
}

/******************************************************************************
 ReportInfiniteLoop (private)

 ******************************************************************************/

void
Command::ReportInfiniteLoop
	(
	const FunctionStack&	fnStack,
	const JIndex			startIndex
	)
{
	JString loop;
	for (JIndex i=startIndex; i>=1; i--)
	{
		if (!loop.IsEmpty())
		{
			loop += " \xE2\x86\x92 ";
		}
		loop += fnStack.Peek(i);
	}
	loop += " \xE2\x86\x92 ";
	loop += fnStack.Peek(startIndex);

	const JUtf8Byte* map[] =
	{
		"loop", loop.GetBytes()
	};
	const JString msg = JGetString("InfiniteLoop::Command", map, sizeof(map));
	JGetUserNotification()->ReportError(msg);
}

/******************************************************************************
 MarkEndOfSequence

 ******************************************************************************/

void
Command::MarkEndOfSequence()
{
	CmdInfo info;
	itsCmdList->AppendItem(info);
}

/******************************************************************************
 Start

	Returns false if it deletes us.

 ******************************************************************************/

bool
Command::Start
	(
	const CommandManager::CmdInfo& info
	)
{
	CommandManager* mgr =
		(itsProjDoc != nullptr ? itsProjDoc->GetCommandManager() : GetCommandManager());

	if (info.isMake)
	{
		Command* p = itsParent;
		while (p != nullptr)
		{
			if (p->itsBuildOutputDoc != nullptr)
			{
				itsOutputDoc = p->itsBuildOutputDoc;
				break;
			}
			p = p->itsParent;
		}

		if (itsOutputDoc == nullptr)
		{
			itsOutputDoc = mgr->GetCompileDoc(itsProjDoc);
			itsOutputDoc->IncrementUseCount();
		}
		SetCompileDocStrings();

		itsBuildOutputDoc = itsOutputDoc;
	}
	else if (info.useWindow)
	{
		Command* p = itsParent;
		while (p != nullptr)
		{
			if (p->itsRunOutputDoc != nullptr)
			{
				itsOutputDoc = p->itsRunOutputDoc;
				break;
			}
			p = p->itsParent;
		}

		if (itsOutputDoc == nullptr)
		{
			itsOutputDoc = CommandManager::GetOutputDoc();
			itsOutputDoc->IncrementUseCount();
		}
		itsDontCloseMsg = JGetString("RunCloseMsg::Command");

		itsWindowTitle = JGetString("RunWindowTitlePrefix::Command");
		if (!info.menuText->IsEmpty())
		{
			itsWindowTitle += *(info.menuText);
		}
		else
		{
			itsWindowTitle += *(info.cmd);
		}

		itsRunOutputDoc = itsOutputDoc;
	}

	if (info.saveAll)	// ok, now that we have decided that command can be executed
	{
		GetDocumentManager()->SaveTextDocuments(false);
		JWait(1.1);		// ensure timestamp is different if any other program modifies the files and then does --revert-all-saved
	}

	// after saving all files, update Makefile

	bool waitForMakeDepend = false;
	if (info.isMake && itsProjDoc != nullptr)
	{
		waitForMakeDepend =
			itsProjDoc->GetBuildManager()->UpdateMakefile(itsOutputDoc, &itsMakeDependCmd);
	}

	if (waitForMakeDepend)
	{
		if (itsMakeDependCmd != nullptr)
		{
			itsCmdList->PrependItem(CmdInfo(nullptr, itsMakeDependCmd, nullptr, true));
			ListenTo(itsMakeDependCmd);		// many may need to hear; can't use SetParent()
			return true;
		}
		else
		{
			DeleteThis();
			return false;
		}
	}
	else if (StartProcess())
	{
		if (itsOutputDoc != nullptr && info.raiseWindowWhenStart)
		{
			itsOutputDoc->Activate();
		}

		return true;
	}
	else	// we have been deleted
	{
		return false;
	}
}

/******************************************************************************
 StartMakeProcess

 ******************************************************************************/

bool
Command::StartMakeProcess
	(
	ExecOutputDocument* outputDoc
	)
{
	assert( itsProjDoc != nullptr );

	itsOutputDoc = outputDoc;
	if (itsOutputDoc == nullptr)
	{
		itsOutputDoc = (itsProjDoc->GetCommandManager())->GetCompileDoc(itsProjDoc);
		itsOutputDoc->IncrementUseCount();

		itsBuildOutputDoc = itsOutputDoc;
	}

	SetCompileDocStrings();
	return StartProcess();
}

/******************************************************************************
 SetCompileDocStrings (private)

 ******************************************************************************/

void
Command::SetCompileDocStrings()
{
	itsWindowTitle = JGetString("CompileWindowTitlePrefix::Command");
	if (itsProjDoc != nullptr)
	{
		itsWindowTitle += itsProjDoc->GetName();
	}
	itsDontCloseMsg = JGetString("CompileCloseMsg::Command");
}

/******************************************************************************
 StartProcess (private)

	Returns false if it deletes us.

 ******************************************************************************/

bool
Command::StartProcess()
{
	// check if we are finished

	while (!itsCmdList->IsEmpty() && itsCmdList->GetFirstItem().IsEndOfSequence())
	{
		CmdInfo info = itsCmdList->GetFirstItem();
		info.Free(true);
		itsCmdList->RemoveItem(1);
	}
	if (itsCmdList->IsEmpty())
	{
		if (itsBeepFlag && itsParent == nullptr)
		{
			JXGetApplication()->GetCurrentDisplay()->Beep();
		}
		DeleteThis();
		return false;
	}

	// check if we can use the window

	itsInQueueFlag = false;
	if (itsOutputDoc != nullptr && itsOutputDoc->ProcessRunning())
	{
		itsInQueueFlag = true;
		ListenTo(itsOutputDoc);
		return true;	// wait for message from itsOutputDoc
	}

	// check if need to run a subroutine

	CmdInfo info = itsCmdList->GetItem(1);
	if (info.cmdObj != nullptr)
	{
		StopListening(itsOutputDoc);	// wait for Command to notify us
		const bool result = info.cmdObj->Start(*info.cmdInfo);
		info.Free(false);
		itsCmdList->RemoveItem(1);
		return result;
	}

	// start process

	assert( info.cmd != nullptr );

	JShouldIncludeCWDOnPath(true);

	JProcess* p;
	int toFD, fromFD;
	JError execErr = JNoError();
	if (itsOutputDoc != nullptr)
	{
		execErr = JProcess::Create(&p, itsCmdPath, *info.cmd,
								   kJCreatePipe, &toFD,
								   kJCreatePipe, &fromFD,
								   kJAttachToFromFD, nullptr);
	}
	else
	{
		JSimpleProcess* p1;
		execErr = JSimpleProcess::Create(&p1, itsCmdPath, *(info.cmd), true);
		p       = p1;
	}

	JShouldIncludeCWDOnPath(false);

	if (!execErr.OK())
	{
		execErr.ReportIfError();
		DeleteThis();
		return false;
	}

	if (itsOutputDoc != nullptr)
	{
		const JSize count = info.cmd->GetItemCount();
		JString cmd;
		for (JIndex i=1; i<=count; i++)
		{
			cmd += JPrepArgForExec(*info.cmd->GetItem(i));
			cmd += " ";
		}

		itsOutputDoc->SetConnection(p, fromFD, toFD, itsWindowTitle, itsDontCloseMsg,
									itsCmdPath, cmd, true);

		// We can't do this in Start() because we might be waiting for
		// itsMakeDependCmd.  We must not listen to both at the same time.

		ListenTo(itsOutputDoc);
	}
	else
	{
		ListenTo(p);
		JThisProcess::Ignore(p);
	}

	info.Free(true);
	itsCmdList->RemoveItem(1);
	return true;
}

/******************************************************************************
 ProcessFinished (private)

 ******************************************************************************/

void
Command::ProcessFinished
	(
	const bool success,
	const bool cancelled
	)
{
	if (!itsInQueueFlag)
	{
		itsSuccessFlag   = itsSuccessFlag && success;
		itsCancelledFlag = cancelled;
	}

	if (success || itsInQueueFlag)
	{
		if (!itsCmdList->IsEmpty() && (itsCmdList->GetItem(1)).isMakeDepend)
		{
			assert( !itsInQueueFlag );
			CmdInfo info = itsCmdList->GetItem(1);
			info.Free(false);			// don't delete Command because it is deleting itself
			itsCmdList->RemoveItem(1);
		}
		StartProcess();		// may delete us
	}
	else if (!itsCmdList->IsEmpty() && (itsCmdList->GetItem(1)).isMakeDepend)
	{
		CmdInfo info = itsCmdList->GetItem(1);
		info.Free(false);				// don't delete Command because it is deleting itself
		itsCmdList->RemoveItem(1);

		DeleteThis();
	}
	else if (!cancelled)
	{
		while (!itsCmdList->IsEmpty() &&
			   !(itsCmdList->GetItem(1)).IsEndOfSequence())
		{
			CmdInfo info = itsCmdList->GetItem(1);
			info.Free(true);
			itsCmdList->RemoveItem(1);
		}
		StartProcess();		// may delete us
	}
	else
	{
		DeleteThis();
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Command::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(JProcess::kFinished))
	{
		auto& info           = dynamic_cast<const JProcess::Finished&>(message);
		const bool cancelled = info.GetReason() != kJChildFinished;
		ProcessFinished(info.Successful() && !cancelled, cancelled);
	}
	else if (sender == itsMakeDependCmd && message.Is(Command::kFinished))
	{
		auto& info           = dynamic_cast<const Command::Finished&>(message);
		itsMakeDependCmd     = nullptr;
		const bool cancelled = info.Cancelled();
		ProcessFinished(info.Successful() && !cancelled, cancelled);
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

 ******************************************************************************/

void
Command::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == itsOutputDoc && message->Is(CommandOutputDocument::kFinished))
	{
		auto& info = dynamic_cast<ExecOutputDocument::Finished&>(*message);
		ProcessFinished(info.Successful(), info.Cancelled());
	}
	else
	{
		JBroadcaster::ReceiveWithFeedback(sender, message);
	}
}

/******************************************************************************
 CmdInfo functions

 ******************************************************************************/

/******************************************************************************
 IsEndOfSequence

 ******************************************************************************/

bool
Command::CmdInfo::IsEndOfSequence()
	const
{
	return cmd == nullptr && cmdObj == nullptr;
}

/******************************************************************************
 IsSubroutine

 ******************************************************************************/

bool
Command::CmdInfo::IsSubroutine()
	const
{
	return cmd == nullptr && cmdObj != nullptr;
}

/******************************************************************************
 Free

 ******************************************************************************/

void
Command::CmdInfo::Free
	(
	const bool deleteCmdObj
	)
{
	jdelete cmd;
	cmd = nullptr;

	if (deleteCmdObj && cmdObj != nullptr)
	{
		cmdObj->itsCallParentProcessFinishedFlag = false;
		jdelete cmdObj;
		cmdObj = nullptr;
	}

	if (cmdInfo != nullptr)
	{
		cmdInfo->Free();
		jdelete cmdInfo;
		cmdInfo = nullptr;
	}
}
