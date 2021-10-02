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

#include "XDArray2DCommand.h"
#include "XDPlot2DCommand.h"
#include "XDDisplaySourceForMain.h"
#include "XDGetCompletions.h"
#include "XDGetFrame.h"
#include "XDGetStack.h"
#include "XDGetThread.h"
#include "XDGetThreads.h"
#include "XDGetFullPath.h"
#include "XDGetInitArgs.h"
#include "XDGetLocalVars.h"
#include "XDGetSourceFileList.h"
#include "XDVarCommand.h"
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
	true,		// kSetProgram
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

XDLink::XDLink()
	:
	Link(kFeatures),
	itsAcceptor(nullptr),
	itsLink(nullptr),
	itsParsedDataRoot(nullptr)
{
	InitFlags();

	itsBPMgr = jnew XDBreakpointManager(this);
	assert( itsBPMgr != nullptr );

	itsSourcePathList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSourcePathList != nullptr );

	StartDebugger();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

XDLink::~XDLink()
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
XDLink::InitFlags()
{
	itsStackFrameIndex      = (JIndex) -1;
	itsInitFinishedFlag     = false;
	itsProgramIsStoppedFlag = false;
	itsDebuggerBusyFlag     = true;
}

/******************************************************************************
 GetPrompt

 ******************************************************************************/

const JString&
XDLink::GetPrompt()
	const
{
	return JGetString("Prompt::XDLink");
}

/******************************************************************************
 GetScriptPrompt

 ******************************************************************************/

const JString&
XDLink::GetScriptPrompt()
	const
{
	return JGetString("ScriptPrompt::XDLink");
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

bool
XDLink::DebuggerHasStarted()
	const
{
	return true;
}

/******************************************************************************
 GetChooseProgramInstructions

 ******************************************************************************/

JString
XDLink::GetChooseProgramInstructions()
	const
{
	return JGetString("ChooseProgramInstr::XDLink");
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

bool
XDLink::HasProgram()
	const
{
	return !itsProgramName.IsEmpty() || itsLink != nullptr;
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

bool
XDLink::GetProgram
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
XDLink::HasCore()
	const
{
	return false;
}

/******************************************************************************
 GetCore

 ******************************************************************************/

bool
XDLink::GetCore
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
XDLink::HasLoadedSymbols()
	const
{
	return itsLink != nullptr;
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

bool
XDLink::IsDebugging()
	const
{
	return itsLink != nullptr;
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

bool
XDLink::ProgramIsRunning()
	const
{
	return itsLink != nullptr && !itsProgramIsStoppedFlag;
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

bool
XDLink::ProgramIsStopped()
	const
{
	return itsLink != nullptr && itsProgramIsStoppedFlag;
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

bool
XDLink::OKToSendCommands
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
XDLink::IsDefiningScript()
	const
{
	return false;
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
XDLink::Receive
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
XDLink::ReceiveMessageFromDebugger()
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

				auto* task = jnew XDCloseSocketTask(itsLink);
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
XDLink::SetProgram
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

	auto* task = jnew XDSetProgramTask();
	assert( task != nullptr );
	task->Go();
}

/******************************************************************************
 BroadcastProgramSet

 *****************************************************************************/

void
XDLink::BroadcastProgramSet()
{
	JString programName;
	GetProgram(&programName);

	Broadcast(SymbolsLoaded(false, programName));
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
XDLink::ReloadProgram()
{
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
XDLink::SetCore
	(
	const JString& fullName
	)
{
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
XDLink::AttachToProcess
	(
	const pid_t pid
	)
{
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
XDLink::RunProgram
	(
	const JString& args
	)
{
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

BreakpointManager*
XDLink::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
XDLink::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
XDLink::SetBreakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex,
	const bool	temporary
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
XDLink::SetBreakpoint
	(
	const JString&	address,
	const bool	temporary
	)
{
}

/******************************************************************************
 RemoveBreakpoint

 *****************************************************************************/

void
XDLink::RemoveBreakpoint
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
XDLink::RemoveAllBreakpointsOnLine
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
XDLink::RemoveAllBreakpointsAtAddress
	(
	const JString& addr
	)
{
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
XDLink::RemoveAllBreakpoints()
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
XDLink::SetBreakpointEnabled
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
XDLink::SetBreakpointCondition
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
XDLink::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
XDLink::SetBreakpointIgnoreCount
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
XDLink::WatchExpression
	(
	const JString& expr
	)
{
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
XDLink::WatchLocation
	(
	const JString& expr
	)
{
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
XDLink::SwitchToThread
	(
	const JUInt64 id
	)
{
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
XDLink::SwitchToFrame
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
XDLink::StepOver()
{
	Send("step_over");
	itsProgramIsStoppedFlag = false;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
XDLink::StepInto()
{
	Send("step_into");
	itsProgramIsStoppedFlag = false;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
XDLink::StepOut()
{
	Send("step_out");
	itsProgramIsStoppedFlag = false;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
XDLink::Continue()
{
	Send("run");
	itsProgramIsStoppedFlag = false;
	Broadcast(ProgramRunning());
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
XDLink::RunUntil
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
XDLink::SetExecutionPoint
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
XDLink::SetValue
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

Array2DCmd*
XDLink::CreateArray2DCmd
	(
	Array2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
{
	Array2DCmd* cmd = jnew XDArray2DCommand(dir, table, data);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreatePlot2DCmd

 *****************************************************************************/

Plot2DCommand*
XDLink::CreatePlot2DCmd
	(
	Plot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
{
	Plot2DCommand* cmd = jnew XDPlot2DCommand(dir, x, y);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateDisplaySourceForMainCmd

 *****************************************************************************/

DisplaySourceForMainCmd*
XDLink::CreateDisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
{
	DisplaySourceForMainCmd* cmd = jnew XDDisplaySourceForMain(sourceDir);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetCompletionsCmd

 *****************************************************************************/

GetCompletionsCmd*
XDLink::CreateGetCompletionsCmd
	(
	CommandInput*	input,
	CommandOutputDisplay*	history
	)
{
	GetCompletionsCmd* cmd = jnew XDGetCompletions(input, history);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFrameCmd

 *****************************************************************************/

GetFrameCmd*
XDLink::CreateGetFrameCmd
	(
	StackWidget* widget
	)
{
	GetFrameCmd* cmd = jnew XDGetFrame(widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetStackCmd

 *****************************************************************************/

GetStack*
XDLink::CreateGetStackCmd
	(
	JTree*			tree,
	StackWidget*	widget
	)
{
	GetStack* cmd = jnew XDGetStack(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetThreadCmd

 *****************************************************************************/

GetThread*
XDLink::CreateGetThreadCmd
	(
	ThreadsWidget* widget
	)
{
	GetThread* cmd = jnew XDGetThread(widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetThreadsCmd

 *****************************************************************************/

GetThreads*
XDLink::CreateGetThreadsCmd
	(
	JTree*				tree,
	ThreadsWidget*	widget
	)
{
	GetThreads* cmd = jnew XDGetThreads(tree, widget);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetFullPathCmd

 *****************************************************************************/

GetFullPathCmd*
XDLink::CreateGetFullPathCmd
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	GetFullPathCmd* cmd = jnew XDGetFullPath(fileName, lineIndex);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetInitArgsCmd

 *****************************************************************************/

GetInitArgs*
XDLink::CreateGetInitArgsCmd
	(
	JXInputField* argInput
	)
{
	GetInitArgs* cmd = jnew XDGetInitArgs(argInput);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetLocalVarsCmd

 *****************************************************************************/

GetLocalVars*
XDLink::CreateGetLocalVarsCmd
	(
	VarNode* rootNode
	)
{
	GetLocalVars* cmd = jnew XDGetLocalVars(rootNode);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateGetSourceFileListCmd

 *****************************************************************************/

GetSourceFileList*
XDLink::CreateGetSourceFileListCmd
	(
	FileListDir* fileList
	)
{
	GetSourceFileList* cmd = jnew XDGetSourceFileList(fileList);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarValueCmd

 *****************************************************************************/

VarCommand*
XDLink::CreateVarValueCmd
	(
	const JString& expr
	)
{
	JString s("property_get -n ");
	s += expr;
	s += " -d ";
	s += JString((JUInt64)itsStackFrameIndex);

	VarCommand* cmd = jnew XDVarCommand(s);
	assert( cmd != nullptr );
	return cmd;
}

/******************************************************************************
 CreateVarContentCmd

 *****************************************************************************/

VarCommand*
XDLink::CreateVarContentCmd
	(
	const JString& expr
	)
{
	return CreateVarValueCmd(expr);
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

VarNode*
XDLink::CreateVarNode
	(
	const bool shouldUpdate		// false for Local Variables
	)
{
	VarNode* node = jnew XDVarNode(shouldUpdate);
	assert( node != nullptr );
	return node;
}

VarNode*
XDLink::CreateVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	fullName,
	const JString&	value
	)
{
	VarNode* node = jnew XDVarNode(parent, name, fullName, value);
	assert( node != nullptr );
	return node;
}

/******************************************************************************
 Build1DArrayExpression

 *****************************************************************************/

JString
XDLink::Build1DArrayExpression
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
XDLink::Build2DArrayExpression
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

GetMemory*
XDLink::CreateGetMemoryCmd
	(
	MemoryDir* dir
	)
{
	return nullptr;
}

/******************************************************************************
 CreateGetAssemblyCmd

 *****************************************************************************/

GetAssemblyCmd*
XDLink::CreateGetAssemblyCmd
	(
	SourceDirector* dir
	)
{
	return nullptr;
}

/******************************************************************************
 CreateGetRegistersCmd

 *****************************************************************************/

GetRegisters*
XDLink::CreateGetRegistersCmd
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
XDLink::Send
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
XDLink::SendRaw
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
XDLink::SendMedicCommand
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
XDLink::StopProgram()
{
//	SendRaw("break -i break");
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
XDLink::KillProgram()
{
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

bool
XDLink::OKToDetachOrKill()
	const
{
	return true;
}

/******************************************************************************
 StartDebugger (private)

	We cannot send anything to xdebug until it has successfully started.

 *****************************************************************************/

bool
XDLink::StartDebugger()
{
	if (itsAcceptor == nullptr)
	{
		itsAcceptor = jnew XDAcceptor;
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

		auto* task = jnew XDWelcomeTask(msg, true);
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

		auto* task = jnew XDWelcomeTask(msg, false);
		assert( task != nullptr );
		task->Go();
		return true;
	}
}

/******************************************************************************
 ChangeDebugger

 *****************************************************************************/

bool
XDLink::ChangeDebugger()
{
	return true;
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

bool
XDLink::RestartDebugger()
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
XDLink::StopDebugger()
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
XDLink::ConnectionEstablished
	(
	XDSocket* socket
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
XDLink::ConnectionFinished
	(
	XDSocket* socket
	)
{
	assert( socket == itsLink );

	itsLink = nullptr;
	itsIDEKey.Clear();

	RestartDebugger();
}
