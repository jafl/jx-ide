/******************************************************************************
 GDBLink.cpp

	Interface to gdb.

	BASE CLASS = ::Link

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "GDBLink.h"
#include "GDBOutputScanner.h"
#include "GDBPingTask.h"
#include "GDBBreakpointManager.h"

#include "GDBAnalyzeCoreCmd.h"
#include "GDBArray2DCmd.h"
#include "GDBPlot2DCmd.h"
#include "GDBCheckCoreStatusCmd.h"
#include "GDBDisplaySourceForMainCmd.h"
#include "GDBGetStopLocationForLinkCmd.h"
#include "GDBGetStopLocationForAsmCmd.h"
#include "GDBGetCompletionsCmd.h"
#include "GDBGetFrameCmd.h"
#include "GDBGetStackCmd.h"
#include "GDBGetThreadCmd.h"
#include "GDBGetThreadsCmd.h"
#include "GDBGetMemoryCmd.h"
#include "GDBGetAssemblyCmd.h"
#include "GDBGetRegistersCmd.h"
#include "GDBGetFullPathCmd.h"
#include "GDBGetInitArgsCmd.h"
#include "GDBGetLocalVarsCmd.h"
#include "GDBGetProgramNameCmd.h"
#include "GDBGetSourceFileListCmd.h"
#include "GDBSimpleCmd.h"
#include "GDBVarCmd.h"
#include "GDBVarNode.h"

#include "ChooseProcessDialog.h"
#include "globals.h"

#include <jx-af/jx/JXAssert.h>
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jSysUtil.h>
#include <jx-af/jcore/jAssert.h>

static const bool kFeatures[]=
{
	true,	// kSetProgram
	true,	// kSetArgs
	true,	// kSetCore
	true,	// kSetProcess
	true,	// kRunProgram
	true,	// kStopProgram
	true,	// kSetExecutionPoint
	true,	// kExecuteBackwards
	true,	// kShowBreakpointInfo
	true,	// kSetBreakpointCondition
	true,	// kSetBreakpointIgnoreCount
	true,	// kWatchExpression
	true,	// kWatchLocation
	true,	// kExamineMemory
	true,	// kDisassembleMemory
};

/******************************************************************************
 Constructor

 *****************************************************************************/

gdb::Link::Link()
	:
	::Link(kFeatures),
	itsDebuggerProcess(nullptr),
	itsChildProcess(nullptr),
	itsOutputLink(nullptr),
	itsInputLink(nullptr)
{
	InitFlags();

	itsScanner = jnew Output::Scanner;
	assert( itsScanner != nullptr );

	itsBPMgr = jnew BreakpointManager(this);
	assert( itsBPMgr != nullptr );

	itsGetStopLocation = jnew GetStopLocationForLinkCmd();
	assert( itsGetStopLocation != nullptr );

	itsGetStopLocation2 = jnew GetStopLocationForAsmCmd();
	assert( itsGetStopLocation2 != nullptr );

	itsPingTask = jnew PingTask();
	assert( itsPingTask != nullptr );

	StartDebugger();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

gdb::Link::~Link()
{
	StopDebugger();

	jdelete itsScanner;
	jdelete itsBPMgr;
	jdelete itsGetStopLocation;
	jdelete itsGetStopLocation2;
	jdelete itsPingTask;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
gdb::Link::InitFlags()
{
	itsHasStartedFlag           = false;
	itsInitFinishedFlag         = false;
	itsSymbolsLoadedFlag        = false;
	itsDebuggerBusyFlag         = true;
	itsIsDebuggingFlag          = false;
	itsIsAttachedFlag           = false;
	itsProgramIsStoppedFlag     = true;
	itsFirstBreakFlag           = false;
	itsPrintingOutputFlag       = true;	// print welcome message
	itsDefiningScriptFlag       = false;
	itsWaitingToQuitFlag        = false;
	itsContinueCount            = 0;
	itsPingID                   = 0;
}

/******************************************************************************
 GetPrompt

 ******************************************************************************/

const JString&
gdb::Link::GetPrompt()
	const
{
	return JGetString("Prompt::GDBLink");
}

/******************************************************************************
 GetScriptPrompt

 ******************************************************************************/

const JString&
gdb::Link::GetScriptPrompt()
	const
{
	return JGetString("ScriptPrompt::GDBLink");
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

bool
gdb::Link::DebuggerHasStarted()
	const
{
	return itsHasStartedFlag;
}

/******************************************************************************
 GetChooseProgramInstructions

 ******************************************************************************/

JString
gdb::Link::GetChooseProgramInstructions()
	const
{
	return JGetString("ChooseProgramInstr::GDBLink");
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

bool
gdb::Link::HasProgram()
	const
{
	return !itsProgramName.IsEmpty();
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

bool
gdb::Link::GetProgram
	(
	JString* fullName
	)
	const
{
	*fullName = itsProgramName;
	return !itsProgramName.IsEmpty();
}

/******************************************************************************
 HasCore

 ******************************************************************************/

bool
gdb::Link::HasCore()
	const
{
	return !itsCoreName.IsEmpty();
}

/******************************************************************************
 GetCore

 ******************************************************************************/

bool
gdb::Link::GetCore
	(
	JString* fullName
	)
	const
{
	*fullName = itsCoreName;
	return !itsCoreName.IsEmpty();
}

/******************************************************************************
 HasLoadedSymbols

 ******************************************************************************/

bool
gdb::Link::HasLoadedSymbols()
	const
{
	return itsSymbolsLoadedFlag;
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

bool
gdb::Link::IsDebugging()
	const
{
	return itsIsDebuggingFlag;
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

bool
gdb::Link::ProgramIsRunning()
	const
{
	return itsIsDebuggingFlag && !itsProgramIsStoppedFlag;
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

bool
gdb::Link::ProgramIsStopped()
	const
{
	return itsIsDebuggingFlag && itsProgramIsStoppedFlag;
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

bool
gdb::Link::OKToSendCommands
	(
	const bool background
	)
	const
{
	return itsContinueCount == 0;
}

/******************************************************************************
 IsDefiningScript

 *****************************************************************************/

bool
gdb::Link::IsDefiningScript()
	const
{
	return itsDefiningScriptFlag;
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
gdb::Link::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsInputLink && message.Is(PipeT::kReadReady))
	{
		ReadFromDebugger();
	}
	else if (sender == itsDebuggerProcess && message.Is(JProcess::kFinished))
	{
		if (!itsWaitingToQuitFlag &&
			JGetUserNotification()->AskUserYes(JGetString("DebuggerCrashed::GDBLink")))
		{
			RestartDebugger();
		}
	}
	else if (sender == itsChildProcess && message.Is(JProcess::kFinished))
	{
		ProgramFinished1();
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 ReadFromDebugger (private)

 *****************************************************************************/

void
gdb::Link::ReadFromDebugger()
{
	JString data;
	itsInputLink->Read(&data);
	Broadcast(DebugOutput(data, kOutputType));

	itsScanner->AppendInput(data);
	while (true)
	{
		const Output::Scanner::Token token = itsScanner->NextToken();
		if (token.type == Output::Scanner::kEOF)
		{
			break;
		}

		// can't send init cmds until gdb has started

		if (!itsHasStartedFlag)
		{
			InitDebugger();
		}

		// check whether or not debugger is busy
		// (We set itsProgramIsStoppedFlag here so *all* commands know the
		//  correct status.)

		const bool wasStopped = itsProgramIsStoppedFlag;
		if (token.type == Output::Scanner::kReadyForInput)
		{
			itsPingTask->Stop();

			if (0 < token.data.number && token.data.number < itsPingID)
			{
				continue;
			}

			itsProgramIsStoppedFlag = true;
//			itsPrintingOutputFlag   = true;
			itsDefiningScriptFlag   = false;

			itsDebuggerBusyFlag = false;
			Broadcast(DebuggerReadyForInput());
			RunNextCommand();

			// If we stopped the program in order to send commands to the
			// debugger, we need to continue once everything settles down.
			// Unless it is invoked by a Command, itsContinueCount has to
			// start at 2 because we have to ignore the prompt received
			// immediately after sending SIGINT.

			if (itsContinueCount > 0 && !HasForegroundCommands())
			{
				itsContinueCount--;

				if (itsContinueCount == 0)
				{
					Send(JString("continue", JString::kNoCopy));
				}
			}

			// if no source file for current location, won't get
			// ProgramStoppedAtLocation message

			else if (itsIsDebuggingFlag && !wasStopped)
			{
				Location location;
				SendProgramStopped(location);
			}

			continue;
		}
		else if (!itsDebuggerBusyFlag)
		{
			itsDebuggerBusyFlag = true;
			Broadcast(DebuggerBusy());
		}

		// process token

		if (token.type == Output::Scanner::kProgramOutput)
		{
			if (itsPrintingOutputFlag)
			{
				// We cannot tell the difference between gdb and program output
				Broadcast(UserOutput(*(token.data.pString), false, false));
			}
		}
		else if (token.type == Output::Scanner::kErrorOutput)
		{
			if (itsPrintingOutputFlag)
			{
				Broadcast(UserOutput(*(token.data.pString), true));
			}
		}

		else if (token.type == Output::Scanner::kBeginMedicCmd ||
				 token.type == Output::Scanner::kBeginMedicIgnoreCmd)
		{
			HandleCommandRunning(token.data.number);
			itsPrintingOutputFlag = false;
		}
		else if (token.type == Output::Scanner::kCommandOutput)
		{
			Command* cmd;
			if (GetRunningCommand(&cmd))
			{
				cmd->Accumulate(*(token.data.pString));
			}
		}
		else if (token.type == Output::Scanner::kCommandResult)
		{
			Command* cmd;
			if (GetRunningCommand(&cmd))
			{
				cmd->SaveResult(*(token.data.pString));
			}
		}
		else if (token.type == Output::Scanner::kEndMedicCmd ||
				 token.type == Output::Scanner::kEndMedicIgnoreCmd)
		{
			Command* cmd;
			if (GetRunningCommand(&cmd))
			{
				cmd->Finished(true);	// may delete object
				SetRunningCommand(nullptr);

				if (!HasForegroundCommands())
				{
					RunNextCommand();
				}
			}

			itsPrintingOutputFlag = true;
		}

		else if (token.type == Output::Scanner::kBreakpointsChanged)
		{
			if (!itsFirstBreakFlag)		// ignore tbreak in hook-run
			{
				Broadcast(BreakpointsChanged());
			}
		}
		else if (token.type == Output::Scanner::kFrameChanged)
		{
			Broadcast(FrameChanged());	// sync with kFrameChangedAndProgramStoppedAtLocation
		}
		else if (token.type == Output::Scanner::kThreadChanged)
		{
			Broadcast(ThreadChanged());
		}
		else if (token.type == Output::Scanner::kValueChanged)
		{
			Broadcast(ValueChanged());
		}

		else if (token.type == Output::Scanner::kPrepareToLoadSymbols)
		{
			itsIsDebuggingFlag   = false;
			itsSymbolsLoadedFlag = false;
			itsProgramName.Clear();
			ClearFileNameMap();
			Broadcast(PrepareToLoadSymbols());
		}
		else if (token.type == Output::Scanner::kFinishedLoadingSymbolsFromProgram)
		{
			itsSymbolsLoadedFlag = true;

			// We can't use a lexer to extract the file name from gdb's
			// output because the pattern "Reading symbols from [^\n]+..."
			// will slurp up "(no debugging symbols found)..." as well.

			auto* cmd = jnew GetProgramNameCmd;
			assert( cmd != nullptr );

			if (token.data.pString != nullptr)
			{
				Broadcast(UserOutput(*(token.data.pString), false));
			}
		}
		else if (token.type == Output::Scanner::kNoSymbolsInProgram)
		{
			if (!itsIsAttachedFlag)
			{
				if (token.data.pString != nullptr)
				{
					Broadcast(UserOutput(*(token.data.pString), true));
				}

				JString name;
				Broadcast(SymbolsLoaded(false, name));
			}
		}
		else if (token.type == Output::Scanner::kSymbolsReloaded)
		{
			Broadcast(PrepareToLoadSymbols());
			Broadcast(SymbolsReloaded());
		}

		else if (token.type == Output::Scanner::kCoreChanged)
		{
			// We have to check whether a core was loaded or cleared.

			auto* cmd = jnew CheckCoreStatusCmd;
			assert( cmd != nullptr );
		}

		else if (token.type == Output::Scanner::kAttachedToProcess)
		{
			itsIsAttachedFlag  = true;
			itsIsDebuggingFlag = true;
			ProgramStarted(token.data.number);
			Broadcast(AttachedToProcess());
		}
		else if (token.type == Output::Scanner::kDetachingFromProcess)
		{
			ProgramFinished1();
		}

		else if (token.type == Output::Scanner::kProgramStarting)
		{
			itsIsDebuggingFlag      = true;
			itsProgramIsStoppedFlag = false;
			itsFirstBreakFlag       = true;
			itsPrintingOutputFlag   = false;	// ignore tbreak output
		}
		else if (token.type == Output::Scanner::kBeginGetPID)
		{
			itsPrintingOutputFlag = false;	// ignore "info prog"
		}
		else if (token.type == Output::Scanner::kProgramPID)
		{
			ProgramStarted(token.data.number);
		}
		else if (token.type == Output::Scanner::kEndGetPID)
		{
			if (itsChildProcess == nullptr)	// ask user for PID
			{
				auto* dialog =
					jnew ChooseProcessDialog(JXGetApplication(), false);
				assert( dialog != nullptr );
				dialog->BeginDialog();
			}
		}

		else if (token.type == Output::Scanner::kProgramStoppedAtLocation)
		{
			itsProgramIsStoppedFlag = true;
			if (token.data.pLocation != nullptr)
			{
				SendProgramStopped(*(token.data.pLocation));
//				Send("call (JXGetAssertHandler())->UnlockDisplays()");
			}
			else
			{
				itsGetStopLocation->Send();
			}

			if (token.data.pString != nullptr)
			{
				Broadcast(UserOutput(*(token.data.pString), false));
			}
		}
		else if (token.type == Output::Scanner::kFrameChangedAndProgramStoppedAtLocation)
		{
			Broadcast(FrameChanged());	// sync with kFrameChanged

			itsProgramIsStoppedFlag = true;
			itsGetStopLocation->Send();
		}

		else if (token.type == Output::Scanner::kBeginScriptDefinition)
		{
			itsDefiningScriptFlag = true;
			Broadcast(DebuggerDefiningScript());
		}

		else if (token.type == Output::Scanner::kPlugInMessage)
		{
			Broadcast(PlugInMessage(*(token.data.pString)));
		}

		else if (token.type == Output::Scanner::kProgramRunning)
		{
			itsProgramIsStoppedFlag = false;
			CancelBackgroundCommands();
			Broadcast(ProgramRunning());
		}
		else if (token.type == Output::Scanner::kProgramFinished)
		{
			ProgramFinished1();

			if (token.data.pString != nullptr)
			{
				Broadcast(UserOutput(*(token.data.pString), false));
			}
		}
		else if (token.type == Output::Scanner::kProgramKilled)
		{
			ProgramFinished1();
		}

		else if (token.type == Output::Scanner::kDebuggerFinished)
		{
			JXGetApplication()->Quit();
			itsWaitingToQuitFlag = true;
		}
	}
}

/******************************************************************************
 SaveProgramName

	Callback for GetProgramNameCmd.

 *****************************************************************************/

void
gdb::Link::SaveProgramName
	(
	const JString& fileName
	)
{
	itsProgramName = fileName;

	JString path, name;
	if (!itsProgramName.IsEmpty())
	{
		JSplitPathAndName(itsProgramName, &path, &name);
	}

	Broadcast(SymbolsLoaded(true, name));
}

/******************************************************************************
 SaveCoreName

	Callback for CheckCoreStatusCmd.

 *****************************************************************************/

void
gdb::Link::SaveCoreName
	(
	const JString& fileName
	)
{
	itsCoreName = fileName;
	if (!itsCoreName.IsEmpty())
	{
		itsIsDebuggingFlag = false;
		Broadcast(CoreLoaded());
	}
	else
	{
		// debugging status set elsewhere
		Broadcast(CoreCleared());
	}
}

/******************************************************************************
 FirstBreakImpossible

 *****************************************************************************/

void
gdb::Link::FirstBreakImpossible()
{
	if (itsFirstBreakFlag)
	{
		// parallel to SendProgramStopped()

		itsFirstBreakFlag     = false;
		itsPrintingOutputFlag = true;
	}
}

/******************************************************************************
 SendProgramStopped

 *****************************************************************************/

void
gdb::Link::SendProgramStopped
	(
	const Location& location
	)
{
	if (itsFirstBreakFlag)					// tbreak to get pid
	{
		// parallel to FirstBreakImpossible()

		itsFirstBreakFlag     = false;
		itsPrintingOutputFlag = true;		// "info prog" is finished

		Broadcast(ProgramFirstStop());

		if (itsBPMgr->HasBreakpointAt(location))
		{
			if (itsContinueCount == 0)
			{
				PrivateSendProgramStopped(location);
			}
		}
		else
		{
			Send(JString("continue", JString::kNoCopy));
		}
	}
	else if (itsContinueCount == 0)
	{
		PrivateSendProgramStopped(location);
	}
}

/******************************************************************************
 PrivateSendProgramStopped (private)

 *****************************************************************************/

void
gdb::Link::PrivateSendProgramStopped
	(
	const Location& location
	)
{
	Broadcast(ProgramStopped(location));

	if (location.GetFunctionName().IsEmpty() ||
		location.GetMemoryAddress().IsEmpty())
{
		itsGetStopLocation2->Send();
}

	RunNextCommand();
}

/******************************************************************************
 SendProgramStopped2

 *****************************************************************************/

void
gdb::Link::SendProgramStopped2
	(
	const Location& location
	)
{
	Broadcast(ProgramStopped2(location));
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
gdb::Link::SetProgram
	(
	const JString& fullName
	)
{
	if (itsHasStartedFlag)
	{
		if (itsInitFinishedFlag && !JSameDirEntry(fullName, itsProgramName))
		{
			Send(JString("delete", JString::kNoCopy));
		}
		DetachOrKill();
		Send(JString("core-file", JString::kNoCopy));

		Send("file " + JPrepArgForExec(fullName));
	}

	itsProgramName = fullName;
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
gdb::Link::ReloadProgram()
{
	if (HasProgram())
	{
		SetProgram(itsProgramName);
	}
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
gdb::Link::SetCore
	(
	const JString& fullName
	)
{
	if (itsHasStartedFlag)
	{
		DetachOrKill();

		// can't use JPrepArgForExec() because gdb doesn't like the quotes

		const JString cmdStr = "core-file " + fullName;
		if (itsProgramName.IsEmpty())
		{
			auto* cmd = jnew AnalyzeCoreCmd(cmdStr);
			assert( cmd != nullptr );
			cmd->Send();
		}
		else
		{
			Send(cmdStr);
		}
	}

	itsCoreName = fullName;
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
gdb::Link::AttachToProcess
	(
	const pid_t pid
	)
{
	Send(JString("core-file", JString::kNoCopy));
	DetachOrKill();

	Send("attach " + JString((JUInt64) pid));
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
gdb::Link::RunProgram
	(
	const JString& args
	)
{
	Send(JString("core-file", JString::kNoCopy));
	DetachOrKill();

	Send("set args " + args);

	Send(JString("run", JString::kNoCopy));
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

BreakpointManager*
gdb::Link::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
gdb::Link::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	JString cmd("echo \\n\ninfo breakpoint ");
	cmd += JString((JUInt64) debuggerIndex);
	cmd += "\necho \\n";
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
gdb::Link::SetBreakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex,
	const bool	temporary
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	JString cmd("echo \\032\\032:Medic breakpoints changed:\n");
	cmd += (temporary ? "-break-insert -t " : "-break-insert ");
	cmd += name + ":" + JString((JUInt64) lineIndex);
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
gdb::Link::SetBreakpoint
	(
	const JString&	address,
	const bool	temporary
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	JString cmd("echo \\032\\032:Medic breakpoints changed:\n");
	cmd += (temporary ? "-break-insert -t " : "-break-insert ");
	cmd += "*";
	cmd += address;
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
gdb::Link::RemoveBreakpoint
	(
	const JIndex debuggerIndex
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	const JString cmd = "delete " + JString((JUInt64) debuggerIndex);
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
gdb::Link::RemoveAllBreakpointsOnLine
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	const JString cmd = "clear " + name + ":" + JString((JUInt64) lineIndex);
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
gdb::Link::RemoveAllBreakpointsAtAddress
	(
	const JString& addr
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	const JString cmd = "clear *" + JString(addr);
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
gdb::Link::RemoveAllBreakpoints()
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	SendWhenStopped(JString("delete", JString::kNoCopy));
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
gdb::Link::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const bool	enabled,
	const bool	once
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	const JString cmd =
		(enabled ? "enable " : "disable ") + 
		((once ? "once " : "" ) + JString((JUInt64) debuggerIndex));
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
gdb::Link::SetBreakpointCondition
	(
	const JIndex	debuggerIndex,
	const JString&	condition
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	const JString cmd = "condition " + JString((JUInt64) debuggerIndex) + " " + condition;
	SendWhenStopped(cmd);
}

/******************************************************************************
 RemoveBreakpointCondition

 *****************************************************************************/

void
gdb::Link::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	const JString cmd = "condition " + JString((JUInt64) debuggerIndex);
	SendWhenStopped(cmd);
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
gdb::Link::SetBreakpointIgnoreCount
	(
	const JIndex	debuggerIndex,
	const JSize		count
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	const JString cmd = "ignore " + JString((JUInt64) debuggerIndex) + " " + JString((JUInt64) count);
	SendWhenStopped(cmd);
}

/******************************************************************************
 WatchExpression

 *****************************************************************************/

void
gdb::Link::WatchExpression
	(
	const JString& expr
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	JString cmd("watch ");
	cmd += expr;
	SendWhenStopped(cmd);
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
gdb::Link::WatchLocation
	(
	const JString& expr
	)
{
	if (!itsProgramIsStoppedFlag)
	{
		itsContinueCount = 2;
	}

	JString cmd("watch -location ");
	cmd += expr;
	SendWhenStopped(cmd);
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
gdb::Link::SwitchToThread
	(
	const JUInt64 id
	)
{
	if (ProgramIsStopped() || HasCore())
	{
		itsPrintingOutputFlag = false;
		Send("thread " + JString(id));
	}
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
gdb::Link::SwitchToFrame
	(
	const JUInt64 id
	)
{
	if (ProgramIsStopped() || HasCore())
	{
		itsPrintingOutputFlag = false;
		Send("frame " + JString(id));
	}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
gdb::Link::StepOver()
{
	Send(JString("next", JString::kNoCopy));
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
gdb::Link::StepInto()
{
	Send(JString("step", JString::kNoCopy));
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
gdb::Link::StepOut()
{
	Send(JString("finish", JString::kNoCopy));
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
gdb::Link::Continue()
{
	Send(JString("continue", JString::kNoCopy));
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
gdb::Link::RunUntil
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	if (ProgramIsStopped())
	{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);

		Send("until " + name + ":" + JString((JUInt64) lineIndex));
	}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
gdb::Link::SetExecutionPoint
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	if (ProgramIsStopped())
	{
		JString path, name;
		JSplitPathAndName(fileName, &path, &name);

		const JString loc = name + ":" + JString((JUInt64) lineIndex);

		SendRaw("tbreak " + loc);
		SendRaw("jump " + loc);
	}
}

/******************************************************************************
 StepOverAssembly

 *****************************************************************************/

void
gdb::Link::StepOverAssembly()
{
	Send(JString("nexti", JString::kNoCopy));
}

/******************************************************************************
 StepIntoAssembly

 *****************************************************************************/

void
gdb::Link::StepIntoAssembly()
{
	Send(JString("stepi", JString::kNoCopy));
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
gdb::Link::RunUntil
	(
	const JString& addr
	)
{
	if (ProgramIsStopped())
	{
		Send("until *" + addr);
	}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
gdb::Link::SetExecutionPoint
	(
	const JString& addr
	)
{
	if (ProgramIsStopped())
	{
		JString loc = "*" + addr;

		SendRaw("tbreak " + loc);
		SendRaw("jump " + loc);
	}
}

/******************************************************************************
 BackupOver

 *****************************************************************************/

void
gdb::Link::BackupOver()
{
	Send(JString("reverse-next", JString::kNoCopy));
}

/******************************************************************************
 BackupInto

 *****************************************************************************/

void
gdb::Link::BackupInto()
{
	Send(JString("reverse-step", JString::kNoCopy));
}

/******************************************************************************
 BackupOut

 *****************************************************************************/

void
gdb::Link::BackupOut()
{
	Send(JString("reverse-finish", JString::kNoCopy));
}

/******************************************************************************
 BackupContinue

 *****************************************************************************/

void
gdb::Link::BackupContinue()
{
	Send(JString("reverse-continue", JString::kNoCopy));
}

/******************************************************************************
 SetValue

 *****************************************************************************/

void
gdb::Link::SetValue
	(
	const JString& name,
	const JString& value
	)
{
	if (ProgramIsStopped())
	{
		Send("set variable " + name + " = " + value);

		// We have to broadcast manually because only "set x=y" triggers
		// our hook, not "set variable x=y".  This is actually a good thing
		// because it means we won't get stuck in an infinite loop when we
		// use "set" to change the display options before requesting data.
		// (But it's silly that "set variable" doesn't trigger it!)

		Broadcast(ValueChanged());
	}
}

/******************************************************************************
 CreateArray2DCmd

 *****************************************************************************/

::Array2DCmd*
gdb::Link::CreateArray2DCmd
	(
	Array2DDir*			dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
{
	auto* cmd = jnew Array2DCmd(dir, table, data);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreatePlot2DCmd

 *****************************************************************************/

::Plot2DCmd*
gdb::Link::CreatePlot2DCmd
	(
	Plot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
{
	auto* cmd = jnew Plot2DCmd(dir, x, y);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateDisplaySourceForMainCmd

 *****************************************************************************/

::DisplaySourceForMainCmd*
gdb::Link::CreateDisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
{
	auto* cmd = jnew DisplaySourceForMainCmd(sourceDir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetCompletionsCmd

 *****************************************************************************/

::GetCompletionsCmd*
gdb::Link::CreateGetCompletionsCmd
	(
	CommandInput*	input,
	CommandOutputDisplay*	history
	)
{
	auto* cmd = jnew GetCompletionsCmd(input, history);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFrameCmd

 *****************************************************************************/

::GetFrameCmd*
gdb::Link::CreateGetFrameCmd
	(
	StackWidget* widget
	)
{
	auto* cmd = jnew GetFrameCmd(widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetStackCmd

 *****************************************************************************/

::GetStackCmd*
gdb::Link::CreateGetStackCmd
	(
	JTree*			tree,
	StackWidget*	widget
	)
{
	auto* cmd = jnew GetStackCmd(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetThreadCmd

 *****************************************************************************/

::GetThreadCmd*
gdb::Link::CreateGetThreadCmd
	(
	ThreadsWidget* widget
	)
{
	auto* cmd = jnew GetThreadCmd(widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetThreadsCmd

 *****************************************************************************/

::GetThreadsCmd*
gdb::Link::CreateGetThreadsCmd
	(
	JTree*				tree,
	ThreadsWidget*	widget
	)
{
	auto* cmd = jnew GetThreadsCmd(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFullPathCmd

 *****************************************************************************/

::GetFullPathCmd*
gdb::Link::CreateGetFullPathCmd
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	auto* cmd = jnew GetFullPathCmd(fileName, lineIndex);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetInitArgsCmd

 *****************************************************************************/

::GetInitArgsCmd*
gdb::Link::CreateGetInitArgsCmd
	(
	JXInputField* argInput
	)
{
	auto* cmd = jnew GetInitArgsCmd(argInput);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetLocalVarsCmd

 *****************************************************************************/

::GetLocalVarsCmd*
gdb::Link::CreateGetLocalVarsCmd
	(
	::VarNode* rootNode
	)
{
	auto* cmd = jnew GetLocalVarsCmd(rootNode);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetSourceFileListCmd

 *****************************************************************************/

::GetSourceFileListCmd*
gdb::Link::CreateGetSourceFileListCmd
	(
	FileListDir* fileList
	)
{
	auto* cmd = jnew GetSourceFileListCmd(fileList);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarValueCmd

 *****************************************************************************/

::VarCmd*
gdb::Link::CreateVarValueCmd
	(
	const JString& expr
	)
{
	auto* cmd = jnew VarCmd("print " + expr);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarContentCmd

 *****************************************************************************/

::VarCmd*
gdb::Link::CreateVarContentCmd
	(
	const JString& expr
	)
{
	auto* cmd = jnew VarCmd("print *(" + expr + ")");
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

::VarNode*
gdb::Link::CreateVarNode
	(
	const bool shouldUpdate		// false for Local Variables
	)
{
	auto* node = jnew VarNode(shouldUpdate);
	assert( node != nullptr );
	return node;
}

::VarNode*
gdb::Link::CreateVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
{
	auto* node = jnew VarNode(parent, name, value);
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
gdb::Link::Build1DArrayExpression
	(
	const JString&	expr,
	const JInteger	index
	)
{
	return Build1DArrayExpressionForCFamilyLanguage(expr, index);
}

/******************************************************************************
 Build2DArrayExpression

 *****************************************************************************/

JString
gdb::Link::Build2DArrayExpression
	(
	const JString&	expr,
	const JInteger	rowIndex,
	const JInteger	colIndex
	)
{
	return Build2DArrayExpressionForCFamilyLanguage(expr, rowIndex, colIndex);
}

/******************************************************************************
 CreateGetMemoryCmd

 *****************************************************************************/

::GetMemoryCmd*
gdb::Link::CreateGetMemoryCmd
	(
	MemoryDir* dir
	)
{
	auto* cmd = jnew GetMemoryCmd(dir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetAssemblyCmd

 *****************************************************************************/

::GetAssemblyCmd*
gdb::Link::CreateGetAssemblyCmd
	(
	SourceDirector* dir
	)
{
	auto* cmd = jnew GetAssemblyCmd(dir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetRegistersCmd

 *****************************************************************************/

::GetRegistersCmd*
gdb::Link::CreateGetRegistersCmd
	(
	RegistersDir* dir
	)
{
	auto* cmd = jnew GetRegistersCmd(dir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 ParseList (static)

	Reads data of the format "...","...","...".  It terminates when it
	finds a closing brace.

 *****************************************************************************/

bool
gdb::Link::ParseList
	(
	std::istringstream&	stream,
	JPtrArray<JString>*	list,
	const JUtf8Byte		terminator
	)
{
	list->CleanOut();

	JString value;
	while (true)
	{
		stream >> value;
		if (!stream.good())
		{
			return false;
		}

		list->Append(value);

		int c = stream.peek();
		if (c == terminator)
		{
			return true;
		}
		if (c == ',')
		{
			stream.ignore();
		}
		else
		{
			return false;
		}
	}
}

/******************************************************************************
 ParseMap (static)

	Reads data of the format x="...",y="...",z="...".  It terminates when
	it finds a closing brace.

 *****************************************************************************/

bool
gdb::Link::ParseMap
	(
	std::istringstream&		stream,
	JStringPtrMap<JString>*	map
	)
{
	map->CleanOut();

	JString key, value;
	bool found;
	while (true)
	{
		key = JReadUntil(stream, '=', &found);
		if (!found)
		{
			return false;
		}

		const JUtf8Byte next = stream.peek();
		if (next == '{' || next == '[')
		{
			stream.ignore();	// skip {
			JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
			if (!ParseList(stream, &list, next == '[' ? ']' : '}'))
			{
				return false;
			}
			stream.ignore();	// skip }

			value.Clear();
			const JSize count = list.GetElementCount();
			for (JIndex i=1; i<=count; i++)
			{
				if (i > 1)
				{
					value.Append("\1");
				}
				value.Append(*(list.GetElement(i)));
			}
		}
		else
		{
			stream >> value;
			if (!stream.good())
			{
				return false;
			}
		}

		map->SetNewElement(key, value);

		int c = stream.peek();
		if (c == '}')
		{
			return true;
		}
		if (c == ',')
		{
			stream.ignore();
		}
		else
		{
			return false;
		}
	}
}

/******************************************************************************
 ParseMapArray (static)

	Reads data of the format {...},{...},{...}.  It terminates when it
	finds a closing square bracket.

 *****************************************************************************/

bool
gdb::Link::ParseMapArray
	(
	std::istringstream&						stream,
	JPtrArray< JStringPtrMap<JString> >*	list
	)
{
	list->CleanOut();

	while (true)
	{
		int c = stream.peek();
		if (c == ']')
		{
			return true;
		}
		else if (c != '{')
		{
			return false;
		}
		stream.ignore();

		auto* map = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
		assert( map != nullptr );
		if (ParseMap(stream, map))
		{
			list->Append(map);
		}
		else
		{
			jdelete map;
			return false;
		}
		stream.ignore();

		c = stream.peek();
		if (c == ']')
		{
			return true;
		}
		if (c == ',')
		{
			stream.ignore();
		}
		else
		{
			return false;
		}
	}
}

/******************************************************************************
 SendPing

	Pings gdb to see if it is ready.  Used when gdb is in asynch mode (macOS).

 *****************************************************************************/

void
gdb::Link::SendPing()
{
	itsPingID++;
	if (itsPingID == 0)
	{
		itsPingID++;
	}
	const JString idStr((JUInt64) itsPingID);
	const JUtf8Byte* map[] =
	{
		"id", idStr.GetBytes()
	};
	const JString cmd = JGetString("PingCommand::GDBLink", map, sizeof(map));
	SendRaw(cmd);
}

/******************************************************************************
 SendWhenStopped

	Sends the given text as command(s) to gdb after the program stops.

 *****************************************************************************/

void
gdb::Link::SendWhenStopped
	(
	const JString& text
	)
{
	Send(text);
}

/******************************************************************************
 Send

	Sends the given text as command(s) to gdb.

 *****************************************************************************/

void
gdb::Link::Send
	(
	const JString& text
	)
{
	if (itsOutputLink != nullptr)
	{
		if (ProgramIsRunning())
		{
			StopProgram();
		}

		SendRaw(text);
	}
}

/******************************************************************************
 SendRaw

	Sends the given text as text to whatever is currently accepting text.

 *****************************************************************************/

void
gdb::Link::SendRaw
	(
	const JString& text
	)
{
	if (itsOutputLink != nullptr)
	{
		itsOutputLink->Write(text);
		Broadcast(DebugOutput(text, kCommandType));

		if (!itsDebuggerBusyFlag)
		{
			itsDebuggerBusyFlag = true;
			Broadcast(DebuggerBusy());
		}
	}
}

/******************************************************************************
 SendMedicCommand (virtual protected)

 *****************************************************************************/

void
gdb::Link::SendMedicCommand
	(
	Command* command
	)
{
	if (itsOutputLink == nullptr)
	{
		return;
	}

	command->Starting();

	if (!itsFirstBreakFlag && !itsProgramIsStoppedFlag)
	{
		itsContinueCount = 1;	// only at final prompt will itsForegroundQ be empty
	}

	const JString id((JUInt64) command->GetTransactionID());

	const JUtf8Byte *startId, *endId;
	if (command->WillIgnoreResult())
	{
		startId = "StartIgnoreResultCommand::GDBLink";
		endId   = "EndIgnoreResultCommand::GDBLink";
	}
	else
	{
		startId = "StartCommand::GDBLink";
		endId   = "EndCommand::GDBLink";
	}

	const JUtf8Byte* map[] =
	{
		"id", id.GetBytes()
	};
	JString cmd = JGetString(startId, map, sizeof(map));
	Send(cmd);	// switching this to SendRaw() will cripple opening of source files while the program is running

	SendRaw(command->GetCommand());

	cmd = JGetString(endId, map, sizeof(map));
	SendRaw(cmd);
}

/******************************************************************************
 ProgramStarted

	It would be nice to detect "program finished" by *only* listening to
	itsChildProcess, but this doesn't work for remote debugging.

 *****************************************************************************/

void
gdb::Link::ProgramStarted
	(
	const pid_t pid
	)
{
	jdelete itsChildProcess;
	itsChildProcess = nullptr;

	if (pid != 0)
	{
		itsChildProcess = jnew JProcess(pid);
		assert( itsChildProcess != nullptr );
		ListenTo(itsChildProcess);

		std::ostringstream log;
		log << "Program started; pid=" << pid;
		Log(log);
	}
}

/******************************************************************************
 ProgramFinished1 (private)

	It would be nice to detect "program finished" by *only* listening to
	itsChildProcess, but this doesn't work for remote debugging.

 *****************************************************************************/

void
gdb::Link::ProgramFinished1()
{
	jdelete itsChildProcess;
	itsChildProcess = nullptr;

	if (itsIsAttachedFlag)
	{
		Broadcast(DetachedFromProcess());
		itsIsAttachedFlag = false;
	}

	if (itsIsDebuggingFlag)
	{
		itsIsDebuggingFlag = false;
		Broadcast(ProgramFinished());
	}
}

/******************************************************************************
 StopProgram

 *****************************************************************************/

void
gdb::Link::StopProgram()
{
	if (itsFirstBreakFlag)
	{
		// wait for tbreak
	}
	else if (itsChildProcess != nullptr)
	{
		Log("stopping program");
		itsChildProcess->SendSignal(SIGINT);
	}
//	else if (itsDebuggerProcess != nullptr)	// remote debugging
//	{
//		itsDebuggerProcess->SendSignal(SIGINT);
//	}
	else
	{
		auto* dlog =
			jnew ChooseProcessDialog(JXGetApplication(), false, true);
		assert( dlog != nullptr );
		dlog->BeginDialog();
	}
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
gdb::Link::KillProgram()
{
	SendWhenStopped(JString("kill", JString::kNoCopy));
}

/******************************************************************************
 DetachOrKill (private)

 *****************************************************************************/

void
gdb::Link::DetachOrKill()
{
	if (itsIsAttachedFlag)
	{
		Send(JString("detach", JString::kNoCopy));
	}
	else if (itsChildProcess != nullptr)
	{
		Send(JString("kill", JString::kNoCopy));
	}
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

bool
gdb::Link::OKToDetachOrKill()
	const
{
	if (itsIsAttachedFlag)
	{
		return JGetUserNotification()->AskUserYes(JGetString("WarnDetachProgram::GDBLink"));
	}
	else if (itsIsDebuggingFlag)
	{
		return JGetUserNotification()->AskUserYes(JGetString("WarnKillProgram::GDBLink"));
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 StartDebugger (private)

	We cannot send anything to gdb until it has successfully started.

 *****************************************************************************/

bool
gdb::Link::StartDebugger()
{
	assert( itsDebuggerProcess == nullptr && itsChildProcess == nullptr );

	itsScanner->Reset();

	itsDebuggerCmd    = GetPrefsManager()->GetGDBCommand();
	const JString cmd = itsDebuggerCmd + " --fullname --interpreter=mi2";

	int toFD, fromFD;
	const JError err = JProcess::Create(&itsDebuggerProcess, cmd,
										kJCreatePipe, &toFD,
										kJCreatePipe, &fromFD,
										kJAttachToFromFD, nullptr);
	if (err.OK())
	{
		itsOutputLink = new ProcessLink(toFD);
		assert( itsOutputLink != nullptr );

		itsInputLink = new ProcessLink(fromFD);
		assert( itsInputLink != nullptr );
		ListenTo(itsInputLink);

		ListenTo(itsDebuggerProcess);

		itsWaitingToQuitFlag = false;
		return true;
	}
	else
	{
		JGetStringManager()->ReportError("UnableToStartDebugger::GDBLink", err);
		return false;
	}
}

/******************************************************************************
 InitDebugger (private)

	We send the init commands after receiving first prompt so gdb startup
	messages are printed to cmd window.

 *****************************************************************************/

void
gdb::Link::InitDebugger()
{
	Send(JGetString("InitCommands::GDBLink"));

	itsHasStartedFlag = true;
	Broadcast(DebuggerStarted());

	if (!itsProgramName.IsEmpty())
	{
		SetProgram(itsProgramName);
	}
	if (!itsCoreName.IsEmpty())
	{
		SetCore(itsCoreName);
	}

	itsInitFinishedFlag = true;
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

bool
gdb::Link::ChangeDebugger()
{
	PrefsManager* mgr = GetPrefsManager();
	if (itsDebuggerCmd != mgr->GetGDBCommand())
	{
		return RestartDebugger();
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

bool
gdb::Link::RestartDebugger()
{
	const bool symbolsWereLoaded = itsSymbolsLoadedFlag;

	StopDebugger();
	const bool ok = StartDebugger();

	if (ok && symbolsWereLoaded)
	{
		Broadcast(DebuggerRestarted());
	}

	return ok;
}

/******************************************************************************
 StopDebugger (private)

 *****************************************************************************/

void
gdb::Link::StopDebugger()
{
	DetachOrKill();
	Send(JString("quit", JString::kNoCopy));

	jdelete itsDebuggerProcess;
	itsDebuggerProcess = nullptr;

	jdelete itsChildProcess;
	itsChildProcess = nullptr;

	delete itsOutputLink;
	itsOutputLink = nullptr;

	delete itsInputLink;
	itsInputLink = nullptr;

	CancelAllCommands();

	InitFlags();
}
