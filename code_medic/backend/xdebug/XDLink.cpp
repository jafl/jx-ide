/******************************************************************************
 XDLink.cpp

	Interface to Xdebug.

	BASE CLASS = Link

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#include "XDLink.h"
#include "XDSocket.h"
#include "XDWelcomeTask.h"
#include "XDSetProgramTask.h"
#include "XDCloseSocketTask.h"
#include "XDBreakpointManager.h"

#include "XDArray2DCmd.h"
#include "XDPlot2DCmd.h"
#include "XDDisplaySourceForMainCmd.h"
#include "XDGetCompletionsCmd.h"
#include "XDGetFrameCmd.h"
#include "XDGetStackCmd.h"
#include "XDGetThreadCmd.h"
#include "XDGetThreadsCmd.h"
#include "XDGetFullPathCmd.h"
#include "XDGetInitArgsCmd.h"
#include "XDGetLocalVarsCmd.h"
#include "XDGetSourceFileListCmd.h"
#include "XDVarCmd.h"
#include "XDVarNode.h"

#include "CommandDirector.h"
#include "StackDir.h"
#include "StackWidget.h"
#include "StackFrameNode.h"
#include "MDIServer.h"
#include "globals.h"

#include <libxml/parser.h>

#include <jx-af/jx/JXAssert.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

const JIndex kXdebugPort = 9000;

static const bool kFeatures[]=
{
	true,	// kSetProgram
	false,	// kSetArgs
	false,	// kSetCore
	false,	// kSetProcess
	false,	// kRunProgram
	false,	// kStopProgram
	false,	// kSetExecutionPoint
	false,	// kExecuteBackwards
	false,	// kShowBreakpointInfo
	false,	// kSetBreakpointCondition
	false,	// kSetBreakpointIgnoreCount
	false,	// kWatchExpression
	false,	// kWatchLocation
	false,	// kExamineMemory
	false,	// kDisassembleMemory
};

/******************************************************************************
 Constructor

 *****************************************************************************/

xdebug::Link::Link()
	:
	::Link(kFeatures, "CommandPrompt::XDLink", "ScriptPrompt::XDLink", "ChooseProgramInstr::XDLink"),
	itsAcceptor(nullptr),
	itsLink(nullptr),
	itsParsedDataRoot(nullptr)
{
	InitFlags();

	itsBPMgr = jnew BreakpointManager(this);
	assert( itsBPMgr != nullptr );

	itsSourcePathList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSourcePathList != nullptr );

	StartDebugger();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

xdebug::Link::~Link()
{
	StopDebugger();

	jdelete itsAcceptor;
	jdelete itsBPMgr;
	jdelete itsSourcePathList;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
xdebug::Link::InitFlags()
{
	itsStackFrameIndex      = (JIndex) -1;
	itsInitFinishedFlag     = false;
	itsProgramIsStoppedFlag = false;
	itsDebuggerBusyFlag     = true;
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

bool
xdebug::Link::DebuggerHasStarted()
	const
{
	return true;
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

bool
xdebug::Link::HasProgram()
	const
{
	return !itsProgramName.IsEmpty() || itsLink != nullptr;
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

bool
xdebug::Link::GetProgram
	(
	JString* fullName
	)
	const
{
	*fullName = (itsProgramName.IsEmpty() ? JString("PHP", JString::kNoCopy) : itsProgramName);
	return true;
}

/******************************************************************************
 HasCore

 ******************************************************************************/

bool
xdebug::Link::HasCore()
	const
{
	return false;
}

/******************************************************************************
 GetCore

 ******************************************************************************/

bool
xdebug::Link::GetCore
	(
	JString* fullName
	)
	const
{
	fullName->Clear();
	return false;
}

/******************************************************************************
 HasLoadedSymbols

 ******************************************************************************/

bool
xdebug::Link::HasLoadedSymbols()
	const
{
	return itsLink != nullptr;
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

bool
xdebug::Link::IsDebugging()
	const
{
	return itsLink != nullptr;
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

bool
xdebug::Link::ProgramIsRunning()
	const
{
	return itsLink != nullptr && !itsProgramIsStoppedFlag;
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

bool
xdebug::Link::ProgramIsStopped()
	const
{
	return itsLink != nullptr && itsProgramIsStoppedFlag;
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

bool
xdebug::Link::OKToSendCommands
	(
	const bool background
	)
	const
{
	return true;
}

/******************************************************************************
 IsDefiningScript

 *****************************************************************************/

bool
xdebug::Link::IsDefiningScript()
	const
{
	return false;
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
xdebug::Link::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
	{
		ReceiveMessageFromDebugger();
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveMessageFromDebugger (private)

 *****************************************************************************/

void
xdebug::Link::ReceiveMessageFromDebugger()
{
	itsLink->StopTimer();

	JString data;
	const bool ok = itsLink->GetNextMessage(&data);
	assert( ok );

	if (data.IsEmpty() || data.GetFirstCharacter() != '<')
	{
		return;
	}

	Broadcast(DebugOutput(data, kOutputType));

	if (itsInitFinishedFlag)
	{
		if (!itsProgramIsStoppedFlag)
		{
			itsProgramIsStoppedFlag = true;
			Broadcast(ProgramStopped(Location(JString::empty, 1)));
		}

		itsDebuggerBusyFlag = false;
		Broadcast(DebuggerReadyForInput());
	}

	xmlDoc* doc = xmlReadMemory(data.GetBytes(), data.GetByteCount(),
								nullptr, nullptr, XML_PARSE_NOCDATA);
	if (doc != nullptr)
	{
		xmlNode* root = xmlDocGetRootElement(doc);

		if (root != nullptr && strcmp((char*) root->name, "init") == 0)
		{
			itsIDEKey         = JGetXMLNodeAttr(root, "idekey");
			const JString uri = JGetXMLNodeAttr(root, "fileuri");

			const JUtf8Byte* map[] =
			{
				"idekey", itsIDEKey.GetBytes(),
				"uri",    uri.GetBytes()
			};
			JString msg = JGetString("ConnectionInfo::XDLink", map, sizeof(map));
			Broadcast(UserOutput(msg, false));

			Send("feature_set -n show_hidden -v 1");
			Send("step_into");

			JString programName;
			GetProgram(&programName);

			Broadcast(AttachedToProcess());
			Broadcast(SymbolsLoaded(uri == itsScriptURI, programName));

			itsInitFinishedFlag = true;
			itsScriptURI        = uri;
		}
		else if (root != nullptr && strcmp((char*) root->name, "response") == 0)
		{
			const JString status = JGetXMLNodeAttr(root, "status");
			const JString reason = JGetXMLNodeAttr(root, "reason");
			if (status == "break" && reason == "error" &&
				root->children != nullptr && root->children->children != nullptr &&
				strcmp((char*) root->children->name, "error") == 0 &&
				root->children->children->type == XML_TEXT_NODE)
			{
				JString msg((char*) root->children->children->content);
				const JString encoding = JGetXMLNodeAttr(root->children, "encoding");
				if (encoding == "base64")
				{
					msg.DecodeBase64(&msg);
				}
				msg += "\n";
				Broadcast(UserOutput(msg, true));
			}

			const JString idStr = JGetXMLNodeAttr(root, "transaction_id");
			JUInt id;
			if (idStr.ConvertToUInt(&id))
			{
				HandleCommandRunning(id);
			}

			Command* cmd;
			if (GetRunningCommand(&cmd))
			{
				itsParsedDataRoot = root;

				cmd->Finished(root->children == nullptr || strcmp((char*) root->children->name, "error") != 0);

				itsParsedDataRoot = nullptr;

				SetRunningCommand(nullptr);
				if (!HasForegroundCommands())
				{
					RunNextCommand();
				}
			}

			if (status == "stopping" || status == "stopped")
			{
				CancelAllCommands();

				auto* task = jnew CloseSocketTask(itsLink);
				assert( task != nullptr );
				task->Go();
			}
		}

		xmlFreeDoc(doc);
	}
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
xdebug::Link::SetProgram
	(
	const JString& fileName
	)
{
	Send("detach");

//	StopDebugger();		// avoid broadcasting DebuggerRestarted
//	StartDebugger();

	itsProgramConfigFileName.Clear();
	itsSourcePathList->DeleteAll();

	JString fullName;
	if (!JConvertToAbsolutePath(fileName, JString::empty, &fullName) ||
		!JFileReadable(fullName))
	{
		const JString error = JGetString("ConfigFileUnreadable::XDLink");
		Broadcast(UserOutput(error, true));
		return;
	}
	else if (MDIServer::IsBinary(fullName))
	{
		const JString error = JGetString("ConfigFileIsBinary::XDLink");
		Broadcast(UserOutput(error, true));
		return;
	}

	JString line;
	if (!MDIServer::GetLanguage(fullName, &line) ||
		JString::Compare(line, "php", JString::kIgnoreCase) != 0)
	{
		const JString error = JGetString("ConfigFileWrongLanguage::XDLink");
		Broadcast(UserOutput(error, true));
		return;
	}

	JString path, name, suffix;
	JSplitPathAndName(fullName, &path, &name);
	JSplitRootAndSuffix(name, &itsProgramName, &suffix);

	itsProgramConfigFileName = fullName;

	std::ifstream input(fullName.GetBytes());
	while (true)
	{
		line = JReadLine(input);
		line.TrimWhitespace();

		if (line.BeginsWith("source-path:"))
		{
			JStringIterator iter(&line);
			iter.RemoveNext(12);
			iter.Invalidate();

			line.TrimWhitespace();

			name = JCombinePathAndName(path, line);
			itsSourcePathList->Append(name);
		}
		else if (!line.IsEmpty() && !line.BeginsWith("code-medic:"))
		{
			line.Prepend("Unknown option: ");
			line.Append("\n");
			Broadcast(UserOutput(line, true));
		}

		if (!input.good())
		{
			break;
		}
	}

	auto* task = jnew SetProgramTask();
	assert( task != nullptr );
	task->Go();
}

/******************************************************************************
 BroadcastProgramSet

 *****************************************************************************/

void
xdebug::Link::BroadcastProgramSet()
{
	JString programName;
	GetProgram(&programName);

	Broadcast(SymbolsLoaded(false, programName));
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
xdebug::Link::ReloadProgram()
{
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
xdebug::Link::SetCore
	(
	const JString& fullName
	)
{
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
xdebug::Link::AttachToProcess
	(
	const pid_t pid
	)
{
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
xdebug::Link::RunProgram
	(
	const JString& args
	)
{
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

BreakpointManager*
xdebug::Link::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
xdebug::Link::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
xdebug::Link::SetBreakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex,
	const bool		temporary
	)
{
	JString cmd("breakpoint_set -t line -f ");
	cmd += fileName;
	cmd += " -n ";
	cmd += JString((JUInt64) lineIndex);

	if (temporary)
	{
		cmd += " -r 1";
	}

	Send(cmd);

	Broadcast(BreakpointsChanged());
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
xdebug::Link::SetBreakpoint
	(
	const JString&	address,
	const bool		temporary
	)
{
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
xdebug::Link::RemoveBreakpoint
	(
	const JIndex debuggerIndex
	)
{
	JString cmd("breakpoint_remove -d ");
	cmd += JString((JUInt64) debuggerIndex);
	Send(cmd);

	Broadcast(BreakpointsChanged());
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
xdebug::Link::RemoveAllBreakpointsOnLine
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	bool changed = false;

	JPtrArray<Breakpoint> list(JPtrArrayT::kForgetAll);
	JString cmd;
	if (itsBPMgr->GetBreakpoints(fileName, &list))
	{
		for (JIndex i=1; i<=list.GetElementCount(); i++)
		{
			Breakpoint* bp = list.GetElement(i);
			if (bp->GetLineNumber() == lineIndex)
			{
				cmd  = "breakpoint_remove -d ";
				cmd += JString((JUInt64) bp->GetDebuggerIndex());
				Send(cmd);
				changed = true;
			}
		}
	}

	if (changed)
	{
		Broadcast(BreakpointsChanged());
	}
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
xdebug::Link::RemoveAllBreakpointsAtAddress
	(
	const JString& addr
	)
{
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
xdebug::Link::RemoveAllBreakpoints()
{
	bool changed = false;

	const JPtrArray<Breakpoint>& list = itsBPMgr->GetBreakpoints();
	JString cmd;
	for (JIndex i=1; i<=list.GetElementCount(); i++)
	{
		const Breakpoint* bp = list.GetElement(i);

		cmd	 = "breakpoint_remove -d ";
		cmd += JString((JUInt64) bp->GetDebuggerIndex());
		Send(cmd);
		changed = true;
	}

	if (changed)
	{
		Broadcast(BreakpointsChanged());
	}
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
xdebug::Link::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const bool	enabled,
	const bool	once
	)
{
	JString cmd("breakpoint_update -d ");
	cmd += JString((JUInt64) debuggerIndex);
	cmd += " -s ";
	cmd += (enabled ? "enabled" : "disabled");
	Send(cmd);

	Broadcast(BreakpointsChanged());
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
xdebug::Link::SetBreakpointCondition
	(
	const JIndex	debuggerIndex,
	const JString&	condition
	)
{
}

/******************************************************************************
 RemoveBreakpointCondition

 *****************************************************************************/

void
xdebug::Link::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
xdebug::Link::SetBreakpointIgnoreCount
	(
	const JIndex	debuggerIndex,
	const JSize		count
	)
{
}

/******************************************************************************
 WatchExpression

 *****************************************************************************/

void
xdebug::Link::WatchExpression
	(
	const JString& expr
	)
{
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
xdebug::Link::WatchLocation
	(
	const JString& expr
	)
{
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
xdebug::Link::SwitchToThread
	(
	const JUInt64 id
	)
{
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
xdebug::Link::SwitchToFrame
	(
	const JUInt64 id
	)
{
	if (id != itsStackFrameIndex)
	{
		itsStackFrameIndex = id;
		Broadcast(FrameChanged());
	}

	const StackFrameNode* frame;
	JString fileName;
	JIndex lineIndex;
	if (GetCommandDirector()->GetStackDir()->GetStackWidget()->GetStackFrame(id, &frame) &&
		frame->GetFile(&fileName, &lineIndex))
	{
		if (fileName.BeginsWith("file://"))
		{
			JStringIterator iter(&fileName);
			iter.RemoveNext(7);
		}
		Broadcast(ProgramStopped(Location(fileName, lineIndex)));
	}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
xdebug::Link::StepOver()
{
	Send("step_over");
	itsProgramIsStoppedFlag = false;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
xdebug::Link::StepInto()
{
	Send("step_into");
	itsProgramIsStoppedFlag = false;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
xdebug::Link::StepOut()
{
	Send("step_out");
	itsProgramIsStoppedFlag = false;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
xdebug::Link::Continue()
{
	Send("run");
	itsProgramIsStoppedFlag = false;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
xdebug::Link::RunUntil
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	SetBreakpoint(fileName, lineIndex, true);
	Continue();
}

/******************************************************************************
 SetExecutionPoint

 *****************************************************************************/

void
xdebug::Link::SetExecutionPoint
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
}

/******************************************************************************
 SetValue

 *****************************************************************************/

void
xdebug::Link::SetValue
	(
	const JString& name,
	const JString& value
	)
{
	if (ProgramIsStopped())
	{
		const JString encValue = JString(value).EncodeBase64();

		JString cmd("property_set @i -n ");
		cmd += name;
		cmd += " -l ";
		cmd += JString((JUInt64) encValue.GetByteCount());
		cmd += " -- ";
		cmd += encValue;
		Send(cmd);

		Broadcast(ValueChanged());
	}
}

/******************************************************************************
 CreateArray2DCmd

 *****************************************************************************/

::Array2DCmd*
xdebug::Link::CreateArray2DCmd
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
xdebug::Link::CreatePlot2DCmd
	(
	Plot2DDir*		dir,
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
xdebug::Link::CreateDisplaySourceForMainCmd
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
xdebug::Link::CreateGetCompletionsCmd
	(
	CommandInput*		input,
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
xdebug::Link::CreateGetFrameCmd
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
xdebug::Link::CreateGetStackCmd
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
xdebug::Link::CreateGetThreadCmd
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
xdebug::Link::CreateGetThreadsCmd
	(
	JTree*			tree,
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
xdebug::Link::CreateGetFullPathCmd
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
xdebug::Link::CreateGetInitArgsCmd
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
xdebug::Link::CreateGetLocalVarsCmd
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
xdebug::Link::CreateGetSourceFileListCmd
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
xdebug::Link::CreateVarValueCmd
	(
	const JString& expr
	)
{
	JString s("property_get -n ");
	s += expr;
	s += " -d ";
	s += JString((JUInt64)itsStackFrameIndex);

	auto* cmd = jnew VarCmd(s);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarContentCmd

 *****************************************************************************/

::VarCmd*
xdebug::Link::CreateVarContentCmd
	(
	const JString& expr
	)
{
	return CreateVarValueCmd(expr);
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

::VarNode*
xdebug::Link::CreateVarNode
	(
	const bool shouldUpdate		// false for Local Variables
	)
{
	auto* node = jnew VarNode(shouldUpdate);
	assert( node != nullptr );
	return node;
}

::VarNode*
xdebug::Link::CreateVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
{
	auto* node = jnew VarNode(parent, name, fullName, value);
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
xdebug::Link::Build1DArrayExpression
	(
	const JString&	origExpr,
	const JInteger	index
	)
{
	JString expr = origExpr;

	const JString indexStr(index, 0);	// must use floating point conversion
	if (expr.Contains("$i"))
	{
		// double literal $'s

		JStringIterator iter(&expr);
		JUtf8Character c;
		while (iter.Next("$"))
		{
			if (!iter.Next(&c, kJIteratorStay) || c != 'i')
			{
				iter.Insert("$");
				iter.SkipNext();
			}
		}
		iter.Invalidate();

		const JUtf8Byte* map[] =
		{
			"i", indexStr.GetBytes()
		};
		JGetStringManager()->Replace(&expr, map, sizeof(map));
	}
	else
	{
		expr += "[";
		expr += indexStr;
		expr += "]";
	}

	return expr;
}

/******************************************************************************
 Build2DArrayExpression

 *****************************************************************************/

JString
xdebug::Link::Build2DArrayExpression
	(
	const JString&	origExpr,
	const JInteger	rowIndex,
	const JInteger	colIndex
	)
{
	JString expr = origExpr;

	const bool usesI = expr.Contains("$i");		// row
	const bool usesJ = expr.Contains("$j");		// col

	const JString iStr(rowIndex, 0);	// must use floating point conversion
	const JString jStr(colIndex, 0);	// must use floating point conversion

	// We have to do both at the same time because otherwise we lose a $.

	if (usesI || usesJ)
	{
		// double literal $'s

		JStringIterator iter(&expr);
		JUtf8Character c;
		while (iter.Next("$"))
		{
			if (!iter.Next(&c, kJIteratorStay) || (c != 'i' && c != 'j'))
			{
				iter.Insert("$");
				iter.SkipNext();
			}
		}
		iter.Invalidate();

		const JUtf8Byte* map[] =
		{
			"i", iStr.GetBytes(),
			"j", jStr.GetBytes()
		};
		JGetStringManager()->Replace(&expr, map, sizeof(map));
	}

	if (!usesI || !usesJ)
	{
		if (expr.GetFirstCharacter() != '(' ||
			expr.GetLastCharacter()  != ')')
		{
			expr.Prepend("(");
			expr.Append(")");
		}

		if (!usesI)
		{
			expr += "[";
			expr += iStr;
			expr += "]";
		}
		if (!usesJ)
		{
			expr += "[";
			expr += jStr;
			expr += "]";
		}
	}

	return expr;
}

/******************************************************************************
 CreateGetMemoryCmd

 *****************************************************************************/

::GetMemoryCmd*
xdebug::Link::CreateGetMemoryCmd
	(
	MemoryDir* dir
	)
{
	return nullptr;
}

/******************************************************************************
 CreateGetAssemblyCmd

 *****************************************************************************/

::GetAssemblyCmd*
xdebug::Link::CreateGetAssemblyCmd
	(
	SourceDirector* dir
	)
{
	return nullptr;
}

/******************************************************************************
 CreateGetRegistersCmd

 *****************************************************************************/

::GetRegistersCmd*
xdebug::Link::CreateGetRegistersCmd
	(
	RegistersDir* dir
	)
{
	return nullptr;
}

/******************************************************************************
 Send

	Sends the given text as command(s) to xdebug.

 *****************************************************************************/

void
xdebug::Link::Send
	(
	const JUtf8Byte* text
	)
{
	if (itsLink != nullptr)
	{
		if (ProgramIsRunning())
		{
			StopProgram();
		}

		JString arg(" -i not_command");

		JString s(text);
		JStringIterator iter(&s);
		if (iter.Next("@i"))
		{
			iter.ReplaceLastMatch(arg);
		}
		else
		{
			s += arg;
		}
		iter.Invalidate();

		SendRaw(s);
	}
}

/******************************************************************************
 SendRaw

	Sends the given text as text to whatever is currently accepting text.

 *****************************************************************************/

void
xdebug::Link::SendRaw
	(
	const JString& text
	)
{
	if (itsLink != nullptr)
	{
		JString s = text;
		s.TrimWhitespace();

		JStringIterator iter(&s);
		while (iter.Next("  "))
		{
			iter.RemovePrev();
			iter.SkipPrev();
		}
		iter.Invalidate();

		itsLink->SendMessage(s);
		itsLink->StartTimer();

		if (!itsDebuggerBusyFlag)
		{
			itsDebuggerBusyFlag = true;
			Broadcast(DebuggerBusy());
		}

		Broadcast(DebugOutput(s, kCommandType));
	}
}

/******************************************************************************
 SendMedicCommand (virtual protected)

 *****************************************************************************/

void
xdebug::Link::SendMedicCommand
	(
	Command* command
	)
{
	command->Starting();

	JString arg(" -i ");
	arg += JString((JUInt64) command->GetTransactionID());

	JString s = command->GetCommand();

	JStringIterator iter(&s);
	if (iter.Next("@i"))
	{
		iter.ReplaceLastMatch(arg);
	}
	else
	{
		s += arg;
	}
	iter.Invalidate();

	SendRaw(s);
}

/******************************************************************************
 StopProgram

	xdebug 1.0.4 doesn't support break.  It returns an error and treats it
	as "continue"

 *****************************************************************************/

void
xdebug::Link::StopProgram()
{
//	SendRaw("break -i break");
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
xdebug::Link::KillProgram()
{
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

bool
xdebug::Link::OKToDetachOrKill()
	const
{
	return true;
}

/******************************************************************************
 StartDebugger (private)

	We cannot send anything to xdebug until it has successfully started.

 *****************************************************************************/

bool
xdebug::Link::StartDebugger()
{
	if (itsAcceptor == nullptr)
	{
		itsAcceptor = jnew Acceptor;
		assert( itsAcceptor != nullptr );
	}

	const JString portStr((JUInt64) kXdebugPort);
	if (itsAcceptor->open(ACE_INET_Addr(kXdebugPort)) == -1)
	{
		const JString errStr((JUInt64) jerrno());

		const JUtf8Byte* map[] =
		{
			"port",  portStr.GetBytes(),
			"errno", errStr.GetBytes()
		};
		JString msg = JGetString("ListenError::XDLink", map, sizeof(map));

		auto* task = jnew WelcomeTask(msg, true);
		assert( task != nullptr );
		task->Go();
		return false;
	}
	else
	{
		const JUtf8Byte* map[] =
		{
			"port", portStr.GetBytes()
		};
		JString msg = JGetString("Welcome::XDLink", map, sizeof(map));

		auto* task = jnew WelcomeTask(msg, false);
		assert( task != nullptr );
		task->Go();
		return true;
	}
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

bool
xdebug::Link::ChangeDebugger()
{
	return true;
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

bool
xdebug::Link::RestartDebugger()
{
	StopDebugger();
	const bool ok = StartDebugger();

	if (ok)
	{
		Broadcast(DebuggerRestarted());
	}

	return ok;
}

/******************************************************************************
 StopDebugger (private)

 *****************************************************************************/

void
xdebug::Link::StopDebugger()
{
	Send("detach");

	jdelete itsLink;
	itsLink = nullptr;

	CancelAllCommands();

	InitFlags();
}

/******************************************************************************
 ConnectionEstablished

 *****************************************************************************/

void
xdebug::Link::ConnectionEstablished
	(
	Socket* socket
	)
{
	InitFlags();

	itsLink = socket;
	ListenTo(itsLink);

	itsIDEKey.Clear();

	itsAcceptor->close();

	Broadcast(DebuggerStarted());
	Broadcast(UserOutput(JGetString("Connected::XDLink"), false));
}

/******************************************************************************
 ConnectionFinished

 *****************************************************************************/

void
xdebug::Link::ConnectionFinished
	(
	Socket* socket
	)
{
	assert( socket == itsLink );

	itsLink = nullptr;
	itsIDEKey.Clear();

	RestartDebugger();
}
