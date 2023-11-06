/******************************************************************************
 Link.cpp

	Interface to lldb.

	BASE CLASS = Link

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "LLDBLink.h"
#include "LLDBBreakpointManager.h"
#include "LLDBWelcomeTask.h"
#include "LLDBSymbolsLoadedTask.h"
#include "LLDBRunBackgroundCmdTask.h"

#include "LLDBArray2DCmd.h"
#include "LLDBPlot2DCmd.h"
#include "LLDBDisplaySourceForMainCmd.h"
#include "LLDBGetCompletionsCmd.h"
#include "LLDBGetFrameCmd.h"
#include "LLDBGetStackCmd.h"
#include "LLDBGetThreadCmd.h"
#include "LLDBGetThreadsCmd.h"
#include "LLDBGetMemoryCmd.h"
#include "LLDBGetAssemblyCmd.h"
#include "LLDBGetRegistersCmd.h"
#include "LLDBGetFullPathCmd.h"
#include "LLDBGetInitArgsCmd.h"
#include "LLDBGetLocalVarsCmd.h"
#include "LLDBGetSourceFileListCmd.h"
#include "LLDBVarCmd.h"
#include "LLDBVarNode.h"

// must be before X11 includes which #define Success
#include "lldb/API/SBCommandInterpreter.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBThread.h"
#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBBreakpointLocation.h"
#include "lldb/API/SBListener.h"
#include "lldb/API/SBEvent.h"

#include "LLDBEventTask.h"
#include "globals.h"

#include <jx-af/jx/JXAssert.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jSysUtil.h>
#include <jx-af/jcore/jAssert.h>

extern char **environ;

static const bool kFeatures[]=
{
	true,	// kSetProgram
	true,	// kSetArgs
	true,	// kSetCore -- lldb requires program to be chosen first, unlike gdb
	true,	// kSetProcess -- lldb requires program to be chosen first, unlike gdb
	true,	// kRunProgram
	true,	// kStopProgram
	true,	// kSetExecutionPoint
	false,	// kExecuteBackwards
	true,	// kShowBreakpointInfo
	true,	// kSetBreakpointCondition
	true,	// kSetBreakpointIgnoreCount
	true,	// kWatchExpression
	true,	// kWatchLocation
	true,	// kExamineMemory
	true,	// kDisassembleMemory
};

static const uint32_t kEventMask = 0xFFFFFFFF;

/******************************************************************************
 Constructor

 *****************************************************************************/

lldb::Link::Link()
	:
	::Link(kFeatures, "", "ScriptPrompt::LLDBLink", "ChooseProgramInstr::LLDBLink"),
	SBListener("LLDBLink"),		// without a name, the listener is invalid
	itsDebugger(nullptr),
	itsStdoutStream(nullptr),
	itsStderrStream(nullptr),
	itsEventTask(nullptr),
	itsPrompt(nullptr)
{
	InitFlags();

	itsBPMgr = jnew BreakpointManager(this);

	StartDebugger(false);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

lldb::Link::~Link()
{
	StopDebugger();

	jdelete itsBPMgr;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
lldb::Link::InitFlags()
{
	itsIsAttachedFlag = false;
}

/******************************************************************************
 GetCommandPrompt (virtual)

 ******************************************************************************/

const JString&
lldb::Link::GetCommandPrompt()
	const
{
	const_cast<Link*>(this)->itsPrompt = itsDebugger->GetPrompt();
	return itsPrompt;
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

bool
lldb::Link::DebuggerHasStarted()
	const
{
	return true;
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

bool
lldb::Link::HasProgram()
	const
{
	return itsDebugger->GetNumTargets() > 0;
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

bool
lldb::Link::GetProgram
	(
	JString* fullName
	)
	const
{
	SBFileSpec f = itsDebugger->GetSelectedTarget().GetExecutable();
	if (f.Exists())
{
		*fullName = JCombinePathAndName(
			JString(f.GetDirectory(), JString::kNoCopy),
			JString(f.GetFilename(), JString::kNoCopy));
		return true;
}
	else
{
		fullName->Clear();
		return false;
}
}

/******************************************************************************
 HasCore

 ******************************************************************************/

bool
lldb::Link::HasCore()
	const
{
	return !itsCoreName.IsEmpty();
}

/******************************************************************************
 GetCore

 ******************************************************************************/

bool
lldb::Link::GetCore
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
lldb::Link::HasLoadedSymbols()
	const
{
	SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	StateType state = p.GetState();
	return (p.IsValid() &&
			state != eStateInvalid  &&
			state != eStateUnloaded &&
			state != eStateConnected);
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

bool
lldb::Link::IsDebugging()
	const
{
	SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	StateType state = p.GetState();
	return (p.IsValid() &&
			(state == eStateAttaching ||
			 state == eStateLaunching ||
			 state == eStateRunning   ||
			 state == eStateStepping  ||
			 state == eStateStopped   ||
			 state == eStateCrashed   ||
			 state == eStateSuspended));
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

bool
lldb::Link::ProgramIsRunning()
	const
{
	SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	StateType state = p.GetState();
	return (p.IsValid() &&
			(state == eStateAttaching ||
			 state == eStateLaunching ||
			 state == eStateRunning   ||
			 state == eStateStepping));
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

bool
lldb::Link::ProgramIsStopped()
	const
{
	SBProcess p     = itsDebugger->GetSelectedTarget().GetProcess();
	StateType state = p.GetState();
	return (p.IsValid() &&
			(state == eStateStopped   ||
			 state == eStateCrashed   ||
			 state == eStateSuspended));
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

bool
lldb::Link::OKToSendCommands
	(
	const bool background
	)
	const
{
	return !ProgramIsRunning();
}

/******************************************************************************
 IsDefiningScript

 *****************************************************************************/

bool
lldb::Link::IsDefiningScript()
	const
{
	return false;
}

/******************************************************************************
 HandleEvent

 *****************************************************************************/

void
lldb::Link::HandleEvent()
{
	// read from LLDB

	JUtf8Byte buf[1024];
	JSize count = fread(buf, sizeof(JUtf8Byte), 1023, itsDebugger->GetOutputFileHandle());
	if (count > 0)
	{
		buf[ count ] = '\0';
		Broadcast(UserOutput(JString(buf, JString::kNoCopy), false));
	}

	count = fread(buf, sizeof(JUtf8Byte), 1023, itsDebugger->GetErrorFileHandle());
	if (count > 0)
	{
		buf[ count ] = '\0';
		Broadcast(UserOutput(JString(buf, JString::kNoCopy), true));
	}

	// read from process

	SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid())
	{
		count = p.GetSTDOUT(buf, 1023);
		if (count > 0)
		{
			buf[ count ] = '\0';
			JUtf8Byte* b = buf;

			const JSize len = itsLastProgramInput.GetByteCount();
			if (len < count &&
				JString::CompareMaxNBytes(itsLastProgramInput.GetBytes(), buf, JMin(count, len)) == 0 &&
				(buf[len] == '\n' || buf[len] == '\r'))
			{
				b += len;
				while (*b == '\n' || *b == '\r')
				{
					b++;
				}
			}
			itsLastProgramInput.Clear();

			if (*b != 0)
			{
				Broadcast(UserOutput(JString(b, JString::kNoCopy), false, true));
			}
		}

		count = p.GetSTDERR(buf, 1023);
		if (count > 0)
		{
			buf[ count ] = '\0';
			Broadcast(UserOutput(JString(buf, JString::kNoCopy), true, true));
		}
	}

	// check for events

	SBEvent e;
	if (GetNextEvent(e))
	{
		HandleEvent(e);
	}
}

/******************************************************************************
 HandleEvent (private)

 *****************************************************************************/

void
lldb::Link::HandleEvent
	(
	const SBEvent& e
	)
{
	if (!e.IsValid())
	{
		return;
	}

	const JUtf8Byte* eventClass = e.GetBroadcasterClass();
	const uint32_t eventType    = e.GetType();

	JString msg(eventClass);
	msg += ":";
	msg += JString((JUInt64) eventType);

	if (SBProcess::EventIsProcessEvent(e))
	{
		const StateType state = SBProcess::GetStateFromEvent(e);
		msg += "; process state: " + JString((JUInt64) state) + ", " + JString((JUInt64) SBProcess::GetRestartedFromEvent(e));

		if (state == eStateRunning ||
			state == eStateStepping)
		{
			CancelBackgroundCommands();
			Broadcast(DebuggerBusy());
			Broadcast(ProgramRunning());
		}
		else if (state == eStateStopped ||
				 state == eStateCrashed)
		{
			Broadcast(DebuggerReadyForInput());
			RunNextCommand();

			JString s;
			if (ProgramStopped(&s))
			{
//				Send("call (JXGetAssertHandler())->UnlockDisplays()");
				msg += s;
			}
		}
		else if (state == eStateExited ||
				 state == eStateDetached)
		{
			CleanUpAfterProgramFinished();
		}
	}
	else if (SBThread::EventIsThreadEvent(e))
	{
		SBThread t = SBThread::GetThreadFromEvent(e);
		SBFrame f  = SBThread::GetStackFrameFromEvent(e);

		if (eventType & SBThread::eBroadcastBitThreadSelected)
		{
			// sync with SwitchToThread()
			Broadcast(ThreadChanged());
			ProgramStopped();
		}
		else if ((eventType & SBThread::eBroadcastBitStackChanged) ||
				 (eventType & SBThread::eBroadcastBitSelectedFrameChanged))
		{
			// sync with SwitchToFrame()
			Broadcast(FrameChanged());
			ProgramStopped();
		}
	}
	else if (SBBreakpoint::EventIsBreakpointEvent(e))
	{
		msg += "; bkpt event type: " + JString((JUInt64) SBBreakpoint::GetBreakpointEventTypeFromEvent(e));
		Broadcast(BreakpointsChanged());
	}
	else if (SBWatchpoint::EventIsWatchpointEvent(e))
	{
		msg += "; watch event type: " + JString((JUInt64) SBWatchpoint::GetWatchpointEventTypeFromEvent(e));
		Broadcast(BreakpointsChanged());
	}
	else if (SBCommandInterpreter::EventIsCommandInterpreterEvent(e))
	{
		if (eventType & SBCommandInterpreter::eBroadcastBitQuitCommandReceived)
		{
			JXGetApplication()->Quit();
		}
	}

	Broadcast(DebugOutput(msg, kLogType));
}

/******************************************************************************
 ReceiveMessageLine (private static)

 *****************************************************************************/

j_lldb_cookie_fn_return
lldb::Link::ReceiveMessageLine
	(
	void*				baton,
	const char*			line,
	j_lldb_cookie_size	count
	)
{
	if (count > 0)
	{
		const JString msg(line, count, JString::kNoCopy);
		static_cast<Link*>(baton)->Broadcast(UserOutput(msg, false));
	}
	return count;
}

/******************************************************************************
 ReceiveErrorLine (private static)

 *****************************************************************************/

j_lldb_cookie_fn_return
lldb::Link::ReceiveErrorLine
	(
	void*				baton,
	const char*			line,
	j_lldb_cookie_size	count
	)
{
	if (count > 0)
	{
		const JString msg(line, count, JString::kNoCopy);
		static_cast<Link*>(baton)->Broadcast(UserOutput(msg, true));
	}
	return count;
}

/******************************************************************************
 LogMessage (private static)

 *****************************************************************************/

void
lldb::Link::LogMessage
	(
	const JUtf8Byte*	msg,
	void*				baton
	)
{
	static_cast<Link*>(baton)->Broadcast(DebugOutput(JString(msg, JString::kNoCopy), kLogType));
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
lldb::Link::SetProgram
	(
	const JString& fullName
	)
{
	DetachOrKill(true);

	SBTarget t = itsDebugger->CreateTarget(fullName.GetBytes());
	if (t.IsValid())
	{
		StartListeningForEvents(t.GetBroadcaster(), kEventMask);

		auto* task = jnew SymbolsLoadedTask(fullName);
		task->Go();
	}
}

/******************************************************************************
 SymbolsLoaded

	XXX: we never get eBroadcastBitSymbolsLoaded

 *****************************************************************************/

void
lldb::Link::SymbolsLoaded
	(
	const JString& fullName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	Broadcast(PrepareToLoadSymbols());
	Broadcast(::Link::SymbolsLoaded(true, name));
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
lldb::Link::ReloadProgram()
{
	JString fullName;
	if (GetProgram(&fullName))
	{
		SetProgram(fullName);
	}
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
lldb::Link::SetCore
	(
	const JString& fullName
	)
{
	DetachOrKill(false);

	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		t.LoadCore(fullName.GetBytes());
		itsCoreName = fullName;
		Broadcast(CoreLoaded());
	}
	else
	{
		JString cmdStr("target create --core ");
		cmdStr += fullName;
		SendRaw(cmdStr);
		SendRaw(JString("frame select 1", JString::kNoCopy));
		SendRaw(JString("frame select 0", JString::kNoCopy));
	}
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
lldb::Link::AttachToProcess
	(
	const ::pid_t pid
	)
{
	DetachOrKill(true);

	SBTarget t = itsDebugger->CreateTarget("");
	if (t.IsValid())
	{
		SBAttachInfo info(pid);
		info.SetListener(*this);

		SBError e;
		t.Attach(info, e);

		if (e.Fail())
		{
			Broadcast(UserOutput(JString(e.GetCString(), JString::kNoCopy), true));
		}
		else if (t.IsValid())
		{
			itsIsAttachedFlag = true;
			StartListeningForEvents(t.GetBroadcaster(), kEventMask);

			SBFileSpec f     = t.GetExecutable();
			const JString fullName = JCombinePathAndName(
				JString(f.GetDirectory(), JString::kNoCopy),
				JString(f.GetFilename(), JString::kNoCopy));

			auto* task = jnew SymbolsLoadedTask(fullName);
			task->Go();

			Broadcast(AttachedToProcess());
		}
	}
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
lldb::Link::RunProgram
	(
	const JString& args
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		DetachOrKill(false);

		JUtf8Byte** lldbArgs = nullptr;

		JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
		if (!args.IsEmpty())
		{
			JParseArgsForExec(args, &argList);

			lldbArgs = jnew JUtf8Byte*[ argList.GetElementCount()+1 ];
			assert( lldbArgs != nullptr );

			for (JIndex i=1; i<=argList.GetElementCount(); i++)
			{
				lldbArgs[ i-1 ] = const_cast<JUtf8Byte*>(argList.GetElement(i)->GetBytes());
			}

			lldbArgs[ argList.GetElementCount() ] = nullptr;
		}

		SBError error;
		t.Launch(*this, (const char**) lldbArgs, (const char**) environ,
				 nullptr, nullptr, nullptr, ".", 0, false, error);

		jdelete [] lldbArgs;
	}
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

BreakpointManager*
lldb::Link::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
lldb::Link::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
	SendRaw(JString("breakpoint list", JString::kNoCopy));
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
lldb::Link::SetBreakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex,
	const bool		temporary
	)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		SBBreakpoint b = t.BreakpointCreateByLocation(name.GetBytes(), lineIndex);
		b.SetOneShot(temporary);
	}
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
lldb::Link::SetBreakpoint
	(
	const JString&	address,
	const bool		temporary
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		JUtf8Byte* end;
		const lldb::addr_t a = strtoull(address.GetBytes(), &end, 0);
		SBBreakpoint b = t.BreakpointCreateByAddress(a);
		b.SetOneShot(temporary);
	}
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
lldb::Link::RemoveBreakpoint
	(
	const JIndex debuggerIndex
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		t.BreakpointDelete(debuggerIndex);
	}
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
lldb::Link::RemoveAllBreakpointsOnLine
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (!t.IsValid())
	{
		return;
	}

	JString path, name;
	JSplitPathAndName(fileName, &path, &name);

	const JSize bpCount = t.GetNumBreakpoints();
	for (long i=bpCount-1; i>=0; i--)
	{
		SBBreakpoint b = t.GetBreakpointAtIndex(i);

		const JSize locCount = b.GetNumLocations();
		for (JUnsignedOffset j=0; j<locCount; j++)
		{
			SBAddress a   = b.GetLocationAtIndex(j).GetAddress();
			SBLineEntry e = a.GetLineEntry();
			if (e.GetLine() == lineIndex &&
				JString::Compare(e.GetFileSpec().GetFilename(), name) == 0)
			{
				t.BreakpointDelete(b.GetID());
			}
		}
	}
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
lldb::Link::RemoveAllBreakpointsAtAddress
	(
	const JString& addrStr
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (!t.IsValid())
	{
		return;
	}

	JUtf8Byte* end;
	const lldb::addr_t addr = strtoull(addrStr.GetBytes(), &end, 0);

	const JSize bpCount = t.GetNumBreakpoints();
	for (long i=bpCount-1; i>=0; i--)
	{
		SBBreakpoint b = t.GetBreakpointAtIndex(i);

		const JSize locCount = b.GetNumLocations();
		for (JUnsignedOffset j=0; j<locCount; j++)
		{
			SBAddress a = b.GetLocationAtIndex(j).GetAddress();
			if (a.GetLoadAddress(t) == addr)
			{
				t.BreakpointDelete(b.GetID());
			}
		}
	}
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
lldb::Link::RemoveAllBreakpoints()
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		t.DeleteAllBreakpoints();
	}
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
lldb::Link::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const bool		enabled,
	const bool		once
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		SBBreakpoint b = t.FindBreakpointByID(debuggerIndex);
		b.SetEnabled(enabled);
		b.SetOneShot(once);
	}
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
lldb::Link::SetBreakpointCondition
	(
	const JIndex	debuggerIndex,
	const JString&	condition
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		t.FindBreakpointByID(debuggerIndex).SetCondition(condition.GetBytes());
	}
}

/******************************************************************************
 RemoveBreakpointCondition

 *****************************************************************************/

void
lldb::Link::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		t.FindBreakpointByID(debuggerIndex).SetCondition("");
	}
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
lldb::Link::SetBreakpointIgnoreCount
	(
	const JIndex	debuggerIndex,
	const JSize		count
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		t.FindBreakpointByID(debuggerIndex).SetIgnoreCount(count);
	}
}

/******************************************************************************
 WatchExpression

 *****************************************************************************/

void
lldb::Link::WatchExpression
	(
	const JString& expr
	)
{
	Watch(expr, false);
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
lldb::Link::WatchLocation
	(
	const JString& expr
	)
{
	Watch(expr, true);
}

/******************************************************************************
 Watch (private)

 *****************************************************************************/

void
lldb::Link::Watch
	(
	const JString&	expr,
	const bool	resolveAddress
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		SBValue v = t.EvaluateExpression(expr.GetBytes());
		if (v.IsValid())
		{
			v.Watch(resolveAddress, false, true);
		}
	}
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
lldb::Link::SwitchToThread
	(
	const JUInt64 id
	)
{
	SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid() && ProgramIsStopped())
	{
		p.SetSelectedThreadByID(id);	// does not broadcast

		// sync with HandleEvent()
		Broadcast(ThreadChanged());
		ProgramStopped();
	}
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
lldb::Link::SwitchToFrame
	(
	const JUInt64 id
	)
{
	SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
	{
		t.SetSelectedFrame(id);		// does not broadcast

		// sync with HandleEvent()
		Broadcast(FrameChanged());
		ProgramStopped();
	}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
lldb::Link::StepOver()
{
	SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
	{
		t.StepOver();
	}
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
lldb::Link::StepInto()
{
	SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
	{
		t.StepInto();
	}
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
lldb::Link::StepOut()
{
	SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
	{
		t.StepOut();
	}
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
lldb::Link::Continue()
{
	SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid() && ProgramIsStopped())
	{
		p.Continue();
	}
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
lldb::Link::RunUntil
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	if (ProgramIsStopped())
	{
		SetBreakpoint(fileName, lineIndex, true);
		Continue();
	}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
lldb::Link::SetExecutionPoint
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	if (ProgramIsStopped())
	{
		JString cmd("_regexp-jump ");
		cmd += fileName;
		cmd += ":";
		cmd += JString((JUInt64) lineIndex);
		itsDebugger->HandleCommand(cmd.GetBytes());

		ProgramStopped();
	}
}

/******************************************************************************
 StepOverAssembly

 *****************************************************************************/

void
lldb::Link::StepOverAssembly()
{
	SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
	{
		t.StepInstruction(true);
	}
}

/******************************************************************************
 StepIntoAssembly

 *****************************************************************************/

void
lldb::Link::StepIntoAssembly()
{
	SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
	{
		t.StepInstruction(false);
	}
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
lldb::Link::RunUntil
	(
	const JString& addr
	)
{
	SBThread t = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread();
	if (t.IsValid() && ProgramIsStopped())
	{
		JUtf8Byte* end;
		const lldb::addr_t a = strtoull(addr.GetBytes(), &end, 0);
		t.RunToAddress(a);
	}
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
lldb::Link::SetExecutionPoint
	(
	const JString& addr
	)
{
	if (ProgramIsStopped())
	{
		JString cmd("_regexp-jump *");
		cmd += addr;
		itsDebugger->HandleCommand(cmd.GetBytes());

		ProgramStopped();
	}
}

/******************************************************************************
 BackupOver

 *****************************************************************************/

void
lldb::Link::BackupOver()
{
}

/******************************************************************************
 BackupInto

 *****************************************************************************/

void
lldb::Link::BackupInto()
{
}

/******************************************************************************
 BackupOut

 *****************************************************************************/

void
lldb::Link::BackupOut()
{
}

/******************************************************************************
 BackupContinue

 *****************************************************************************/

void
lldb::Link::BackupContinue()
{
}

/******************************************************************************
 SetValue

 *****************************************************************************/

void
lldb::Link::SetValue
	(
	const JString& name,
	const JString& value
	)
{
	SBFrame f = itsDebugger->GetSelectedTarget().GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
	{
		JString expr = name;
		expr        += " = ";
		expr        += value;
		f.EvaluateExpression(expr.GetBytes());

		Broadcast(ValueChanged());
	}
}

/******************************************************************************
 CreateArray2DCmd

 *****************************************************************************/

::Array2DCmd*
lldb::Link::CreateArray2DCmd
	(
	Array2DDir*			dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
{
	auto* cmd = jnew Array2DCmd(dir, table, data);
	return cmd;
}

/******************************************************************************
 CreatePlot2DCmd

 *****************************************************************************/

::Plot2DCmd*
lldb::Link::CreatePlot2DCmd
	(
	Plot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
{
	auto* cmd = jnew Plot2DCmd(dir, x, y);
	return cmd;
}

/******************************************************************************
 CreateDisplaySourceForMainCmd

 *****************************************************************************/

::DisplaySourceForMainCmd*
lldb::Link::CreateDisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
{
	auto* cmd = jnew DisplaySourceForMainCmd(sourceDir);
	return cmd;
}

/******************************************************************************
 CreateGetCompletionsCmd

 *****************************************************************************/

::GetCompletionsCmd*
lldb::Link::CreateGetCompletionsCmd
	(
	CommandInput*	input,
	CommandOutputDisplay*	history
	)
{
	auto* cmd = jnew GetCompletionsCmd(input, history);
	return cmd;
}

/******************************************************************************
 CreateGetFrameCmd

 *****************************************************************************/

::GetFrameCmd*
lldb::Link::CreateGetFrameCmd
	(
	StackWidget* widget
	)
{
	auto* cmd = jnew GetFrameCmd(widget);
	return cmd;
}

/******************************************************************************
 CreateGetStackCmd

 *****************************************************************************/

::GetStackCmd*
lldb::Link::CreateGetStackCmd
	(
	JTree*			tree,
	StackWidget*	widget
	)
{
	auto* cmd = jnew GetStackCmd(tree, widget);
	return cmd;
}

/******************************************************************************
 CreateGetThreadCmd

 *****************************************************************************/

::GetThreadCmd*
lldb::Link::CreateGetThreadCmd
	(
	ThreadsWidget* widget
	)
{
	auto* cmd = jnew GetThreadCmd(widget);
	return cmd;
}

/******************************************************************************
 CreateGetThreadsCmd

 *****************************************************************************/

::GetThreadsCmd*
lldb::Link::CreateGetThreadsCmd
	(
	JTree*			tree,
	ThreadsWidget*	widget
	)
{
	auto* cmd = jnew GetThreadsCmd(tree, widget);
	return cmd;
}

/******************************************************************************
 CreateGetFullPathCmd

 *****************************************************************************/

::GetFullPathCmd*
lldb::Link::CreateGetFullPathCmd
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	auto* cmd = jnew GetFullPathCmd(fileName, lineIndex);
	return cmd;
}

/******************************************************************************
 CreateGetInitArgsCmd

 *****************************************************************************/

::GetInitArgsCmd*
lldb::Link::CreateGetInitArgsCmd
	(
	JXInputField* argInput
	)
{
	auto* cmd = jnew GetInitArgsCmd(argInput);
	return cmd;
}

/******************************************************************************
 CreateGetLocalVarsCmd

 *****************************************************************************/

::GetLocalVarsCmd*
lldb::Link::CreateGetLocalVarsCmd
	(
	::VarNode* rootNode
	)
{
	auto* cmd = jnew GetLocalVarsCmd(rootNode);
	return cmd;
}

/******************************************************************************
 CreateGetSourceFileListCmd

 *****************************************************************************/

::GetSourceFileListCmd*
lldb::Link::CreateGetSourceFileListCmd
	(
	FileListDir* fileList
	)
{
	auto* cmd = jnew GetSourceFileListCmd(fileList);
	return cmd;
}

/******************************************************************************
 CreateVarValueCmd

 *****************************************************************************/

::VarCmd*
lldb::Link::CreateVarValueCmd
	(
	const JString& expr
	)
{
	auto* cmd = jnew VarCmd(expr);
	return cmd;
}

/******************************************************************************
 CreateVarContentCmd

 *****************************************************************************/

::VarCmd*
lldb::Link::CreateVarContentCmd
	(
	const JString& expr
	)
{
	JString s("*(");
	s += expr;
	s += ")";

	auto* cmd = jnew VarCmd(s);
	return cmd;
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

::VarNode*
lldb::Link::CreateVarNode
	(
	const bool shouldUpdate		// false for Local Variables
	)
{
	auto* node = jnew VarNode(shouldUpdate);
	return node;
}

::VarNode*
lldb::Link::CreateVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
{
	auto* node = jnew VarNode(parent, name, value);
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
lldb::Link::Build1DArrayExpression
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
lldb::Link::Build2DArrayExpression
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
lldb::Link::CreateGetMemoryCmd
	(
	MemoryDir* dir
	)
{
	auto* cmd = jnew GetMemoryCmd(dir);
	return cmd;
}

/******************************************************************************
 CreateGetAssemblyCmd

 *****************************************************************************/

::GetAssemblyCmd*
lldb::Link::CreateGetAssemblyCmd
	(
	SourceDirector* dir
	)
{
	auto* cmd = jnew GetAssemblyCmd(dir);
	return cmd;
}

/******************************************************************************
 CreateGetRegistersCmd

 *****************************************************************************/

::GetRegistersCmd*
lldb::Link::CreateGetRegistersCmd
	(
	RegistersDir* dir
	)
{
	auto* cmd = jnew GetRegistersCmd(dir);
	return cmd;
}

/******************************************************************************
 SendRaw

	Sends the given text as text to whatever is currently accepting text.

 *****************************************************************************/

void
lldb::Link::SendRaw
	(
	const JString& text
	)
{
	if (ProgramIsRunning())
	{
		SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
		p.PutSTDIN(text.GetBytes(), text.GetByteCount());
		p.PutSTDIN("\n", 1);
		itsLastProgramInput = text;
	}
	else
	{
		itsDebugger->HandleCommand(text.GetBytes());
		Broadcast(DebugOutput(text, kCommandType));
	}
}

/******************************************************************************
 SendMedicCommand (virtual protected)

 *****************************************************************************/

void
lldb::Link::SendMedicCommand
	(
	Command* command
	)
{
	command->Starting();

	JXUrgentTask* task = jnew RunBackgroundCmdTask(command);
	task->Go();
}

/******************************************************************************
 SendMedicCommandSync

 *****************************************************************************/

void
lldb::Link::SendMedicCommandSync
	(
	Command* command
	)
{
	HandleCommandRunning(command->GetTransactionID());
	command->Finished(true);	// may delete object
	SetRunningCommand(nullptr);

	if (!HasForegroundCommands())
	{
		RunNextCommand();
	}
}

/******************************************************************************
 ProgramStarted (private)

 *****************************************************************************/

void
lldb::Link::ProgramStarted
	(
	const pid_t pid
	)
{
	std::ostringstream log;
	log << "Program started; pid=" << pid;
	Log(log);
}

/******************************************************************************
 ProgramStopped (private)

 *****************************************************************************/

bool
lldb::Link::ProgramStopped
	(
	JString* msg
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	SBFrame f  = t.GetProcess().GetSelectedThread().GetSelectedFrame();
	if (f.IsValid())
	{
		const SBLineEntry line = f.GetLineEntry();
		const SBFileSpec file  = line.GetFileSpec();
		JString fullName;
		if (file.IsValid())
		{
			fullName = JCombinePathAndName(
				JString(file.GetDirectory(), JString::kNoCopy),
				JString(file.GetFilename(), JString::kNoCopy));
		}
		Location location(fullName, line.IsValid() ? line.GetLine() : 0);

		if (f.GetFunctionName() != nullptr)
		{
			location.SetFunctionName(JString(f.GetFunctionName(), JString::kNoCopy));
		}

		const SBAddress addr = f.GetPCAddress();
		if (addr.IsValid())
		{
			const JString a(addr.GetLoadAddress(t), JString::kBase16);
			location.SetMemoryAddress(a);
		}
		Broadcast(::Link::ProgramStopped(location));

		if (msg != nullptr && file.IsValid())
		{
			*msg =  "; file: " + location.GetFileName() +
					", line: " + JString((JUInt64) location.GetLineNumber()) +
					", func: " + location.GetFunctionName() +
					", addr: " + location.GetMemoryAddress();
		}
		return file.IsValid();
	}
	else
	{
		if (msg != nullptr)
		{
			msg->Clear();
		}
		return false;
	}
}

/******************************************************************************
 CleanUpAfterProgramFinished (private)

 *****************************************************************************/

void
lldb::Link::CleanUpAfterProgramFinished()
{
	if (itsIsAttachedFlag)
	{
		Broadcast(DetachedFromProcess());
		itsIsAttachedFlag = false;
	}

	SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	JString reasonStr;
	if (p.IsValid() && p.GetExitDescription() != nullptr)
	{
		reasonStr  = p.GetExitDescription();
		reasonStr += "\n\n";
	}
	else
	{
		int result;
		const JChildExitReason cer = JDecodeChildExitReason(p.GetExitStatus(), &result);
		reasonStr                  = JPrintChildExitReason(cer, result) + "\n\n";
	}

	Broadcast(UserOutput(reasonStr, false));
	Broadcast(ProgramFinished());

	Broadcast(DebuggerReadyForInput());
	RunNextCommand();
}

/******************************************************************************
 StopProgram

 *****************************************************************************/

void
lldb::Link::StopProgram()
{
	Log("stopping program");

	SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid())
	{
		p.Stop();
	}
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
lldb::Link::KillProgram()
{
	SBProcess p = itsDebugger->GetSelectedTarget().GetProcess();
	if (p.IsValid())
	{
		p.Kill();
	}
}

/******************************************************************************
 DetachOrKill (private)

 *****************************************************************************/

void
lldb::Link::DetachOrKill
	(
	const bool destroyTarget
	)
{
	SBTarget t = itsDebugger->GetSelectedTarget();
	if (t.IsValid())
	{
		SBProcess p = t.GetProcess();
		if (itsIsAttachedFlag)
		{
			p.Detach();
			itsIsAttachedFlag = false;
		}
		else
		{
			p.Kill();
		}

		if (destroyTarget)
		{
			itsDebugger->DeleteTarget(t);
			InitFlags();
		}
	}
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

bool
lldb::Link::OKToDetachOrKill()
	const
{
	if (itsIsAttachedFlag)
	{
		return JGetUserNotification()->AskUserYes(JGetString("WarnDetachProgram::LLDBLink"));
	}
	else if (IsDebugging())
	{
		return JGetUserNotification()->AskUserYes(JGetString("WarnKillProgram::LLDBLink"));
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 StartDebugger (private)

 *****************************************************************************/

bool
lldb::Link::StartDebugger
	(
	const bool restart
	)
{
	assert( itsDebugger == nullptr );

	itsDebugger = jnew SBDebugger(SBDebugger::Create(true, LogMessage, this));
	if (itsDebugger->IsValid())
	{
		StartListeningForEvents(itsDebugger->GetCommandInterpreter().GetBroadcaster(), kEventMask);
		StartListeningForEventClass(*itsDebugger, SBThread::GetBroadcasterClassName(), kEventMask);

		assert( itsStdoutStream == nullptr );
		assert( itsStderrStream == nullptr );

#ifdef _J_MACOS
		itsStdoutStream = fwopen(this, ReceiveMessageLine);
		assert( itsStdoutStream != nullptr );

		itsStderrStream = fwopen(this, ReceiveErrorLine);
		assert( itsStderrStream != nullptr );
#else
		cookie_io_functions_t my_fns = { nullptr, ReceiveMessageLine, nullptr, nullptr };

		itsStdoutStream = fopencookie(this, "w", my_fns);
		assert( itsStdoutStream != nullptr );

		my_fns.write    = ReceiveErrorLine;
		itsStderrStream = fopencookie(this, "w", my_fns);
		assert( itsStderrStream != nullptr );
#endif

		setvbuf(itsStdoutStream, nullptr, _IOLBF, 0);
		itsDebugger->SetOutputFileHandle(itsStdoutStream, false);

		setvbuf(itsStderrStream, nullptr, _IOLBF, 0);
		itsDebugger->SetErrorFileHandle(itsStderrStream, false);

		const JUtf8Byte* map[] =
		{
			"debugger", SBDebugger::GetVersionString()
		};
		const JString msg = JGetString("Welcome::LLDBLink", map, sizeof(map));

		auto* welcomeTask = jnew WelcomeTask(msg, restart);
		welcomeTask->Go();

		itsEventTask = jnew EventTask(this);
		itsEventTask->Start();

		return true;
	}
	else
	{
		JGetStringManager()->ReportError("UnableToStartDebugger::LLDBLink", JString::empty);
		return false;
	}
}

/******************************************************************************
 StopDebugger (private)

 *****************************************************************************/

void
lldb::Link::StopDebugger()
{
	DetachOrKill(true);

	fclose(itsStdoutStream);
	itsStdoutStream = nullptr;

	fclose(itsStderrStream);
	itsStderrStream = nullptr;

	jdelete itsEventTask;
	itsEventTask = nullptr;

	SBDebugger::Destroy(*itsDebugger);
	jdelete itsDebugger;
	itsDebugger = nullptr;

	InitFlags();
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

bool
lldb::Link::RestartDebugger()
{
	const bool symbolsWereLoaded = HasLoadedSymbols();

	StopDebugger();
	return StartDebugger(symbolsWereLoaded);
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

bool
lldb::Link::ChangeDebugger()
{
	return true;
}
