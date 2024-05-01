/******************************************************************************
 Link.cpp

	Interface to JVM debugging agent.

	BASE CLASS = Link

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "jvm/Link.h"
#include "jvm/WelcomeTask.h"
#include "jvm/SetProgramTask.h"
#include "jvm/BreakpointManager.h"
#include "jvm/ThreadNode.h"

#include "jvm/Array2DCmd.h"
#include "jvm/Plot2DCmd.h"
#include "jvm/DisplaySourceForMainCmd.h"
#include "jvm/GetCompletionsCmd.h"
#include "jvm/GetFrameCmd.h"
#include "jvm/GetStackCmd.h"
#include "jvm/GetThreadCmd.h"
#include "jvm/GetThreadsCmd.h"
#include "jvm/GetThreadGroupsCmd.h"
#include "jvm/GetThreadNameCmd.h"
#include "jvm/GetThreadParentCmd.h"
#include "jvm/GetClassInfoCmd.h"
#include "jvm/GetClassMethodsCmd.h"
#include "jvm/GetFullPathCmd.h"
#include "jvm/GetIDSizesCmd.h"
#include "jvm/GetInitArgsCmd.h"
#include "jvm/GetLocalVarsCmd.h"
#include "jvm/GetSourceFileListCmd.h"
#include "jvm/VarCmd.h"
#include "jvm/VarNode.h"

#include "CommandDirector.h"
#include "SourceDirector.h"
#include "MDIServer.h"
#include "globals.h"

#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jx/JXAssert.h>
#include <jx-af/jcore/JTreeNode.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jErrno.h>
#include <jx-af/jcore/jAssert.h>

const JIndex kJavaPort = 9001;

static const bool kFeatures[]=
{
	true,	// kSetProgram
	true,	// kSetArgs
	false,	// kSetCore
	false,	// kSetProcess
	true,	// kRunProgram
	true,	// kStopProgram
	false,	// kSetExecutionPoint
	false,	// kExecuteBackwards
	true,	// kShowBreakpointInfo
	false,	// kSetBreakpointCondition
	true,	// kSetBreakpointIgnoreCount
	false,	// kWatchExpression
	false,	// kWatchLocation
	false,	// kExamineMemory
	false,	// kDisassembleMemory
};

// JBroadcaster message types

const JUtf8Byte* jvm::Link::kIDResolved = "IDResolved::JVMLink";

/******************************************************************************
 Constructor

 *****************************************************************************/

jvm::Link::Link()
	:
	::Link(kFeatures, "CommandPrompt::JVMLink", "ScriptPrompt::JVMLink", "ChooseProgramInstr::JVMLink"),
	itsAcceptor(nullptr),
	itsDebugLink(nullptr),
	itsProcess(nullptr),
	itsOutputLink(nullptr),
	itsInputLink(nullptr),
	itsLatestMsg(nullptr),
	itsJVMDeathTask(nullptr)
{
	InitFlags();

	itsBPMgr = jnew BreakpointManager(this);

	itsSourcePathList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	itsClassByIDList = jnew JArray<ClassInfo>;
	itsClassByIDList->SetCompareFunction(CompareClassIDs);

	itsClassByNameList = jnew JAliasArray<ClassInfo>(itsClassByIDList, CompareClassNames, JListT::kSortAscending);

	itsThreadRoot = jnew ThreadNode(ThreadNode::kGroupType, ThreadNode::kRootThreadGroupID);
	itsThreadTree = jnew JTree(itsThreadRoot);

	itsThreadList = jnew JPtrArray<ThreadNode>(JPtrArrayT::kForgetAll);
	itsThreadList->SetCompareFunction(ThreadNode::CompareID);

	itsCullThreadGroupsTask = jnew JXFunctionTask(10000, std::bind(&Link::CheckNextThreadGroup, this));
	itsCullThreadGroupsTask->Start();
	itsCullThreadGroupIndex = 1;

	itsFrameList = jnew JArray<FrameInfo>();

	StartDebugger();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

jvm::Link::~Link()
{
	StopDebugger();

	jdelete itsAcceptor;
	jdelete itsBPMgr;
	jdelete itsSourcePathList;
	jdelete itsClassByIDList;
	jdelete itsClassByNameList;
	jdelete itsThreadTree;
	jdelete itsThreadList;
	jdelete itsCullThreadGroupsTask;
	jdelete itsFrameList;
	jdelete itsJVMDeathTask;

	DeleteOneShotCommands();
}

/******************************************************************************
 InitFlags (private)

 *****************************************************************************/

void
jvm::Link::InitFlags()
{
	itsInitFinishedFlag     = false;
	itsProgramIsStoppedFlag = false;
	itsCurrentThreadID      = ThreadNode::kRootThreadGroupID;
}

/******************************************************************************
 DebuggerHasStarted

 ******************************************************************************/

bool
jvm::Link::DebuggerHasStarted()
	const
{
	return true;
}

/******************************************************************************
 HasProgram

 ******************************************************************************/

bool
jvm::Link::HasProgram()
	const
{
	return !itsJVMExecArgs.IsEmpty() || itsDebugLink != nullptr;
}

/******************************************************************************
 GetProgram

 ******************************************************************************/

bool
jvm::Link::GetProgram
	(
	JString* fullName
	)
	const
{
	if (itsMainClassName.IsEmpty())
	{
		*fullName = "Java Process";
	}
	else
	{
		*fullName = itsMainClassName;

		JStringIterator iter(fullName, JStringIterator::kStartAtEnd);
		if (iter.Prev("."))
		{
			iter.SkipNext();
			iter.RemoveAllPrev();
		}
	}

	return true;
}

/******************************************************************************
 HasCore

 ******************************************************************************/

bool
jvm::Link::HasCore()
	const
{
	return false;
}

/******************************************************************************
 GetCore

 ******************************************************************************/

bool
jvm::Link::GetCore
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
jvm::Link::HasLoadedSymbols()
	const
{
	return itsDebugLink != nullptr;
}

/******************************************************************************
 IsDebugging

 *****************************************************************************/

bool
jvm::Link::IsDebugging()
	const
{
	return itsDebugLink != nullptr;
}

/******************************************************************************
 ProgramIsRunning

 *****************************************************************************/

bool
jvm::Link::ProgramIsRunning()
	const
{
	return itsDebugLink != nullptr && !itsProgramIsStoppedFlag;
}

/******************************************************************************
 ProgramIsStopped

 *****************************************************************************/

bool
jvm::Link::ProgramIsStopped()
	const
{
	return itsDebugLink != nullptr && itsProgramIsStoppedFlag;
}

/******************************************************************************
 OKToSendCommands

 *****************************************************************************/

bool
jvm::Link::OKToSendCommands
	(
	const bool background
	)
	const
{
	return true;
}

/******************************************************************************
 IsDefiningScript

	This forces user's typing to be echoed directly.

 *****************************************************************************/

bool
jvm::Link::IsDefiningScript()
	const
{
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 *****************************************************************************/

void
jvm::Link::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsDebugLink && message.Is(Socket::kMessageReady))
	{
		auto* info = dynamic_cast<const Socket::MessageReady*>(&message);
		assert( info != nullptr );
		ReceiveMessageFromJVM(*info);
	}
	else if (sender == itsInputLink && message.Is(PipeT::kReadReady))
	{
		ReadFromProcess();
	}

	else if (sender == itsProcess && message.Is(JProcess::kFinished))
	{
		auto* info = dynamic_cast<const JProcess::Finished*>(&message);
		assert( info != nullptr );
		CleanUpAfterProgramFinished(info);

		jdelete itsJVMDeathTask;
		itsJVMDeathTask = nullptr;

		jdelete itsProcess;
		itsProcess = nullptr;
	}

	else if (sender == itsThreadTree)
	{
		Broadcast(ThreadListChanged());
	}

	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveMessageFromJVM (private)

 *****************************************************************************/

void
jvm::Link::ReceiveMessageFromJVM
	(
	const Socket::MessageReady& info
	)
{
	if (info.IsReply())
	{
		std::ostringstream log;
		log << "reply: " << info.GetID() << ' ' << info.GetErrorCode();
		Log(log);

		HandleCommandRunning(info.GetID());

		Command* cmd;
		if (GetRunningCommand(&cmd))
		{
			itsLatestMsg = &info;

			cmd->Finished(info.GetErrorCode() == 0);

			itsLatestMsg = nullptr;

			SetRunningCommand(nullptr);
			if (!HasForegroundCommands())
			{
				RunNextCommand();
			}
		}
	}
	else if (info.GetCommandSet() == kEventCmdSet &&
			 info.GetCommand()    == kECompositeCmd)
	{
		DispatchEventsFromJVM(info.GetData(), info.GetDataLength());
	}
}

/******************************************************************************
 DispatchEventsFromJVM (private)

 *****************************************************************************/

void
jvm::Link::DispatchEventsFromJVM
	(
	const unsigned char*	data,
	const JSize				length
	)
{
	const JIndex suspendPolicy = *data;
	data++;

	const JSize count = Socket::Unpack4(data);
	data += 4;

	for (JIndex i=1; i<=1; i++)		// cannot loop until implement unpack for *all* events
	{
		const JIndex type = *data;
		data++;

		const JIndex requestID = Socket::Unpack4(data);
		data += 4;

		std::ostringstream log;
		log << "event: " << type;
		Log(log);

		if (type == kVMDeathEvent)
		{
			WaitForJVMDeath();
		}
		else if (type == kClassUnloadEvent)
		{
			JSize count;
			const JString signature = Socket::UnpackString(data, &count);
			data += count;

			const JString name = ClassSignatureToName(signature);

			std::ostringstream log2;
			log2 << "unload class: " << name;
			Log(log2);

			ClassInfo info;
			info.name = const_cast<JString*>(&name);
			JIndex j;
			if (itsClassByNameList->SearchSorted(info, JListT::kAnyMatch, &j))
			{
				info = itsClassByNameList->GetItem(j);
				info.Clean();
				itsClassByIDList->RemoveItem(itsClassByNameList->GetItemIndex(j));
			}
		}
		else if (type == kThreadStartEvent)
		{
			const JUInt64 threadID = Socket::Unpack(itsObjectIDSize, data);
			data += itsObjectIDSize;

			std::ostringstream log2;
			log2 << "thread started: " << threadID;
			Log(log2);

			ThreadNode* node;
			if (!FindThread(threadID, &node))	// might be created by GetThreadGroupsCmd
			{
				node = jnew ThreadNode(ThreadNode::kThreadType, threadID);
			}
		}
		else if (type == kThreadDeathEvent)
		{
			const JUInt64 threadID = Socket::Unpack(itsObjectIDSize, data);
			data += itsObjectIDSize;

			std::ostringstream log2;
			log2 << "thread finished: " << threadID;
			Log(log2);

			ThreadNode* node;
			if (FindThread(threadID, &node))
			{
				jdelete node;
			}
		}
	}
}

/******************************************************************************
 SetIDSizes

 *****************************************************************************/

void
jvm::Link::SetIDSizes
	(
	const JSize fieldIDSize,
	const JSize methodIDSize,
	const JSize objectIDSize,
	const JSize refTypeIDSize,
	const JSize frameIDSize
	)
{
	itsFieldIDSize         = fieldIDSize;
	itsMethodIDSize        = methodIDSize;
	itsObjectIDSize        = objectIDSize;
	itsReferenceTypeIDSize = refTypeIDSize;
	itsFrameIDSize         = frameIDSize;

	std::ostringstream log;
	log << "field id size: " << itsFieldIDSize << std::endl;
	log << "method id size: " << itsFieldIDSize << std::endl;
	log << "object id size: " << itsFieldIDSize << std::endl;
	log << "reference type id size: " << itsFieldIDSize << std::endl;
	log << "frame id size: " << itsFieldIDSize;
	Log(log);
}

/******************************************************************************
 ThreadCreated

 *****************************************************************************/

void
jvm::Link::ThreadCreated
	(
	ThreadNode* node
	)
{
	itsThreadList->InsertSorted(node);
}

/******************************************************************************
 ThreadDeleted

 *****************************************************************************/

void
jvm::Link::ThreadDeleted
	(
	ThreadNode* node
	)
{
	JIndex i;
	if (itsThreadList->Find(node, &i))
	{
		itsThreadList->RemoveItem(i);

		if (itsCurrentThreadID == node->GetID())
		{
			itsCurrentThreadID = ThreadNode::kRootThreadGroupID;
		}
	}
}

/******************************************************************************
 FindThread

 *****************************************************************************/

bool
jvm::Link::FindThread
	(
	const JUInt64	id,
	ThreadNode**	node
	)
	const
{
	ThreadNode target(id);
	JIndex i;
	if (itsThreadList->SearchSorted(&target, JListT::kAnyMatch, &i))
	{
		*node = itsThreadList->GetItem(i);
		return true;
	}
	else
	{
		*node = nullptr;
		return false;
	}
}

/******************************************************************************
 CheckNextThreadGroup (private)

 *****************************************************************************/

void
jvm::Link::CheckNextThreadGroup()
{
	if (itsThreadList->IsEmpty())
	{
		return;
	}

	if (!itsThreadList->IndexValid(itsCullThreadGroupIndex))
	{
		itsCullThreadGroupIndex = 1;
	}

	ThreadNode* node;
	while (true)
	{
		node = itsThreadList->GetItem(itsCullThreadGroupIndex);
		if (node->GetType() == ThreadNode::kGroupType)
		{
			break;
		}

		itsCullThreadGroupIndex++;
		if (!itsThreadList->IndexValid(itsCullThreadGroupIndex))
		{
			return;
		}
	}

	Command* cmd = jnew GetThreadParentCmd(node, true);

	itsCullThreadGroupIndex++;
}

/******************************************************************************
 FlushClassList

 *****************************************************************************/

void
jvm::Link::FlushClassList()
{
	const JSize count = itsClassByIDList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		ClassInfo info = itsClassByIDList->GetItem(i);
		info.Clean();
	}

	itsClassByIDList->RemoveAll();

	Log("Flush class list");
}

/******************************************************************************
 AddClass

 *****************************************************************************/

void
jvm::Link::AddClass
	(
	const JUInt64	id,
	const JString&	signature
	)
{
	ClassInfo info;
	info.id = id;

	JIndex i;
	if (itsClassByIDList->SearchSorted(info, JListT::kAnyMatch, &i))
	{
		return;
	}

	info.name = jnew JString(ClassSignatureToName(signature));

	JString path;
	if (ClassSignatureToFile(signature, &path))
	{
		info.path = jnew JString(path);
	}
	else
	{
		info.path = nullptr;
	}

	info.methods = jnew JArray<MethodInfo>();
	info.methods->SetCompareFunction(CompareMethodIDs);

	itsClassByIDList->InsertSorted(info);

	std::ostringstream log;
	log << "AddClass: " << id << ' ' << signature;
	Log(log);

	Broadcast(IDResolved(id));

	Command* cmd = jnew GetClassMethodsCmd(id);
}

/******************************************************************************
 GetClassName

 *****************************************************************************/

bool
jvm::Link::GetClassName
	(
	const JUInt64	id,
	JString*		name
	)
{
	ClassInfo target;
	target.id = id;
	JIndex i;
	if (itsClassByIDList->SearchSorted(target, JListT::kAnyMatch, &i))
	{
		target = itsClassByIDList->GetItem(i);
		*name  = *(target.name);
		return true;
	}
	else
	{
		Command* cmd = jnew GetClassInfoCmd(id);

		name->Clear();
		return false;
	}
}

/******************************************************************************
 GetClassSourceFile

 *****************************************************************************/

bool
jvm::Link::GetClassSourceFile
	(
	const JUInt64	id,
	JString*		fullName
	)
{
	ClassInfo target;
	target.id = id;
	JIndex i;
	if (itsClassByIDList->SearchSorted(target, JListT::kAnyMatch, &i))
	{
		target = itsClassByIDList->GetItem(i);
		if (target.path != nullptr)
		{
			*fullName = *(target.path);
			return true;
		}
	}

	fullName->Clear();
	return false;
}

/******************************************************************************
 AddMethod

 *****************************************************************************/

void
jvm::Link::AddMethod
	(
	const JUInt64	classID,
	const JUInt64	methodID,
	const JString&	name
	)
{
	ClassInfo target;
	target.id = classID;
	JIndex i;
	const bool found = itsClassByIDList->SearchSorted(target, JListT::kAnyMatch, &i);
	assert( found );

	target = itsClassByIDList->GetItem(i);

	MethodInfo info;
	info.id   = methodID;
	info.name = jnew JString(name);

	target.methods->InsertSorted(info);

	std::ostringstream log;
	log << "AddMethod: " << *(target.name) << ": " << methodID << ' ' << name;
	Log(log);

	Broadcast(IDResolved(methodID));
}

/******************************************************************************
 GetMethodName

 *****************************************************************************/

bool
jvm::Link::GetMethodName
	(
	const JUInt64	classID,
	const JUInt64	methodID,
	JString*		name
	)
{
	ClassInfo target1;
	target1.id = classID;
	JIndex i;
	if (!itsClassByIDList->SearchSorted(target1, JListT::kAnyMatch, &i))
	{
		name->Clear();
		return false;
	}

	target1 = itsClassByIDList->GetItem(i);

	MethodInfo target2;
	target2.id = methodID;
	if (target1.methods->SearchSorted(target2, JListT::kAnyMatch, &i))
	{
		target2 = target1.methods->GetItem(i);
		*name   = *(target2.name);
		return true;
	}
	else
	{
		name->Clear();
		return false;
	}
}

/******************************************************************************
 ClassSignatureToResourcePath (static)

 ******************************************************************************/

JString
jvm::Link::ClassSignatureToResourcePath
	(
	const JString& signature
	)
{
	JString path = signature;

	JStringIterator iter(&path);
	if (path.GetFirstCharacter() == 'L')
	{
		iter.RemoveNext();
	}

	if (iter.Next("$"))
	{
		iter.SkipPrev();
		iter.RemoveAllNext();
	}
	else if (path.GetLastCharacter() == ';')
	{
		iter.MoveTo(JStringIterator::kStartAtEnd, 0);
		iter.RemovePrev();
	}

	return path;
}

/******************************************************************************
 ClassSignatureToName (static)

 ******************************************************************************/

JString
jvm::Link::ClassSignatureToName
	(
	const JString& signature
	)
{
	JString name = ClassSignatureToResourcePath(signature);

	JStringIterator iter(&name);
	while (iter.Next(ACE_DIRECTORY_SEPARATOR_STR))
	{
		iter.ReplaceLastMatch("."); 
	}

	return name;
}

/******************************************************************************
 ClassNameToResourcePath (static)

 ******************************************************************************/

JString
jvm::Link::ClassNameToResourcePath
	(
	const JString& name
	)
{
	JString file = name;

	JStringIterator iter(&file);
	while (iter.Next("."))
	{
		iter.ReplaceLastMatch(ACE_DIRECTORY_SEPARATOR_STR); 
	}

	return file;
}

/******************************************************************************
 ClassSignatureToFile (private)

 ******************************************************************************/

bool
jvm::Link::ClassSignatureToFile
	(
	const JString&	signature,
	JString*		fullName
	)
	const
{
	JString file = ClassSignatureToResourcePath(signature);
	file        += ".java";

	const JSize pathCount = itsSourcePathList->GetItemCount();
	for (JIndex i=1; i<=pathCount; i++)
	{
		const JString* path = itsSourcePathList->GetItem(i);
		*fullName           = JCombinePathAndName(*path, file);
		if (JFileReadable(*fullName))
		{
			return true;
		}
	}

	fullName->Clear();
	return false;
}

/******************************************************************************
 CompareClassIDs (private static)

 ******************************************************************************/

std::weak_ordering
jvm::Link::CompareClassIDs
	(
	const ClassInfo& c1,
	const ClassInfo& c2
	)
{
	return c1.id <=> c2.id;
}

/******************************************************************************
 CompareClassNames (private static)

 ******************************************************************************/

std::weak_ordering
jvm::Link::CompareClassNames
	(
	const ClassInfo& c1,
	const ClassInfo& c2
	)
{
	return JCompareStringsCaseSensitive(c1.name, c2.name);
}

/******************************************************************************
 CompareMethodIDs (private static)

 ******************************************************************************/

std::weak_ordering
jvm::Link::CompareMethodIDs
	(
	const MethodInfo& m1,
	const MethodInfo& m2
	)
{
	return m1.id <=> m2.id;
}

/******************************************************************************
 FlushFrameList

 *****************************************************************************/

void
jvm::Link::FlushFrameList()
{
	itsFrameList->RemoveAll();
}

/******************************************************************************
 AddFrame

 *****************************************************************************/

void
jvm::Link::AddFrame
	(
	const JUInt64 id,
	const JUInt64 classID,
	const JUInt64 methodID,
	const JUInt64 codeOffset
	)
{
	FrameInfo info;
	info.id         = id;
	info.classID    = classID;
	info.methodID   = methodID;
	info.codeOffset = codeOffset;

	itsFrameList->AppendItem(info);
}

/******************************************************************************
 GetFrame (private)

	Normally not enough frames to make it worth doing binary search.

 *****************************************************************************/

bool
jvm::Link::GetFrame
	(
	const JUInt64	id,
	JIndex*			index
	)
	const
{
	const JSize count = itsFrameList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		FrameInfo info = itsFrameList->GetItem(i);
		if (info.id == id)
		{
			*index = i;
			return true;
		}
	}

	*index = 0;
	return false;
}

/******************************************************************************
 ReadFromProcess (private)

 *****************************************************************************/

void
jvm::Link::ReadFromProcess()
{
	JString data;
	itsInputLink->Read(&data);
	Broadcast(UserOutput(data, false, true));
}

/******************************************************************************
 SetProgram

 *****************************************************************************/

void
jvm::Link::SetProgram
	(
	const JString& fileName
	)
{
	DetachOrKill();

//	StopDebugger();		// avoid broadcasting DebuggerRestarted
//	StartDebugger();

	itsProgramConfigFileName.Clear();
	itsMainClassName.Clear();
	itsJVMExecArgs.Clear();
	itsSourcePathList->DeleteAll();

	// delete all directories in itsJarPathList

	JString fullName;
	if (!JConvertToAbsolutePath(fileName, JString::empty, &fullName) ||
		!JFileReadable(fullName))
	{
		const JString error = JGetString("ConfigFileUnreadable::JVMLink");
		Broadcast(UserOutput(error, true));
		return;
	}
	else if (MDIServer::IsBinary(fullName))
	{
		const JString error = JGetString("ConfigFileIsBinary::JVMLink");
		Broadcast(UserOutput(error, true));
		return;
	}

	JString line;
	if (!MDIServer::GetLanguage(fullName, &line) ||
		JString::Compare(line, "java", JString::kIgnoreCase) != 0)
	{
		const JString error = JGetString("ConfigFileWrongLanguage::JVMLink");
		Broadcast(UserOutput(error, true));
		return;
	}

	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	itsJVMCWD = path;

	itsProgramConfigFileName = fullName;

	std::ifstream input(fullName.GetBytes());
	while (true)
	{
		line = JReadLine(input);
		line.TrimWhitespace();

		if (line.StartsWith("main-class:"))
		{
			JStringIterator iter(&line);
			iter.RemoveNext(11);
			line.TrimWhitespace();
			itsMainClassName = line;
		}
		else if (line.StartsWith("java-path:"))
		{
			JStringIterator iter(&line);
			iter.RemoveNext(10);
			line.TrimWhitespace();
			itsJVMCWD = line;
		}
		else if (line.StartsWith("java-exec:"))
		{
			JStringIterator iter(&line);
			iter.RemoveNext(10);
			line.TrimWhitespace();
			itsJVMExecArgs = line;
		}
		else if (line.StartsWith("source-path:"))
		{
			JStringIterator iter(&line);
			iter.RemoveNext(12);
			line.TrimWhitespace();

			name = JCombinePathAndName(path, line);
			itsSourcePathList->Append(name);
		}
		else if (line.StartsWith("source-jar:"))
		{
			JStringIterator iter(&line);
			iter.RemoveNext(11);
			line.TrimWhitespace();
			// unpack to /tmp
			// add to itsJarPathList (for deletion)
			// add to itsSourcePathList
		}
		else if (!line.IsEmpty() && !line.StartsWith("code-medic:"))
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

	if (itsJVMExecArgs.IsEmpty())
	{
		itsJVMExecArgs = itsMainClassName;
	}

	auto* task = jnew SetProgramTask();
	task->Go();
}

/******************************************************************************
 BroadcastProgramSet

 *****************************************************************************/

void
jvm::Link::BroadcastProgramSet()
{
	JString programName;
	GetProgram(&programName);

	Broadcast(SymbolsLoaded(false, programName));
}

/******************************************************************************
 ReloadProgram

 *****************************************************************************/

void
jvm::Link::ReloadProgram()
{
	SetProgram(itsProgramConfigFileName);
}

/******************************************************************************
 SetCore

 *****************************************************************************/

void
jvm::Link::SetCore
	(
	const JString& fullName
	)
{
}

/******************************************************************************
 AttachToProcess

 *****************************************************************************/

void
jvm::Link::AttachToProcess
	(
	const pid_t pid
	)
{
}

/******************************************************************************
 RunProgram

 *****************************************************************************/

void
jvm::Link::RunProgram
	(
	const JString& args
	)
{
	DetachOrKill();

	itsJVMCmd         = GetPrefsManager()->GetJVMCommand();
	itsJVMProcessArgs = args;

	JString cmd = itsJVMCmd;

	if (!itsJVMCmd.Contains("-agentlib:jdwp")      &&
		!itsJVMExecArgs.Contains("-agentlib:jdwp") &&
		!itsJVMCmd.Contains("-Xrunjdwp")           &&
		!itsJVMExecArgs.Contains("-Xrunjdwp"))
	{
		cmd += " -agentlib:jdwp=transport=dt_socket,address=localhost:";
		cmd += JString(kJavaPort);
	}

	cmd += " ";
	cmd += itsJVMExecArgs;
	cmd += " ";
	cmd += itsJVMProcessArgs;

	int toFD, fromFD;
	const JError err = JProcess::Create(&itsProcess, itsJVMCWD, cmd,
										kJCreatePipe, &toFD,
										kJCreatePipe, &fromFD,
										kJAttachToFromFD, nullptr);
	if (err.OK())
	{
		ListenTo(itsProcess);

		itsOutputLink = new ProcessLink(toFD);
		assert( itsOutputLink != nullptr );

		itsInputLink = new ProcessLink(fromFD);
		assert( itsInputLink != nullptr );
		ListenTo(itsInputLink);
	}
	else
	{
		err.ReportIfError();
	}
}

/******************************************************************************
 GetBreakpointManager

 *****************************************************************************/

BreakpointManager*
jvm::Link::GetBreakpointManager()
{
	return itsBPMgr;
}

/******************************************************************************
 ShowBreakpointInfo

 *****************************************************************************/

void
jvm::Link::ShowBreakpointInfo
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
jvm::Link::SetBreakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex,
	const bool	temporary
	)
{
	JArray<unsigned char> data;
	data.AppendItem(0x02);			// breakpoint
	data.AppendItem(0x02);			// all
	data.AppendItem(0x01);			// 1 condition
		data.AppendItem(0x07);		// LocationOnly
			data.AppendItem(0x01);	// CLASS
			// classID
			// methodID
			// code index (8 bytes)

//	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd,
//					   data.GetCArray(), data.GetItemCount());
}

/******************************************************************************
 SetBreakpoint

 *****************************************************************************/

void
jvm::Link::SetBreakpoint
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
jvm::Link::RemoveBreakpoint
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 RemoveAllBreakpointsOnLine

 *****************************************************************************/

void
jvm::Link::RemoveAllBreakpointsOnLine
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
}

/******************************************************************************
 RemoveAllBreakpointsAtAddress

 *****************************************************************************/

void
jvm::Link::RemoveAllBreakpointsAtAddress
	(
	const JString& addr
	)
{
}

/******************************************************************************
 RemoveAllBreakpoints

 *****************************************************************************/

void
jvm::Link::RemoveAllBreakpoints()
{
	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERClearAllBreakpointsCmd, nullptr, 0);
}

/******************************************************************************
 SetBreakpointEnabled

 *****************************************************************************/

void
jvm::Link::SetBreakpointEnabled
	(
	const JIndex	debuggerIndex,
	const bool	enabled,
	const bool	once
	)
{
}

/******************************************************************************
 SetBreakpointCondition

 *****************************************************************************/

void
jvm::Link::SetBreakpointCondition
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
jvm::Link::RemoveBreakpointCondition
	(
	const JIndex debuggerIndex
	)
{
}

/******************************************************************************
 SetBreakpointIgnoreCount

 *****************************************************************************/

void
jvm::Link::SetBreakpointIgnoreCount
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
jvm::Link::WatchExpression
	(
	const JString& expr
	)
{
}

/******************************************************************************
 WatchLocation

 *****************************************************************************/

void
jvm::Link::WatchLocation
	(
	const JString& expr
	)
{
}

/******************************************************************************
 SwitchToThread

 *****************************************************************************/

void
jvm::Link::SwitchToThread
	(
	const JIndex id
	)
{
	if (id != itsCurrentThreadID)
	{
		itsCurrentThreadID = id;
		Broadcast(ThreadChanged());
	}
}

/******************************************************************************
 SwitchToFrame

 *****************************************************************************/

void
jvm::Link::SwitchToFrame
	(
	const JIndex id
	)
{
	JIndex i;
	if (GetFrame(id, &i))
	{
		FrameInfo info = itsFrameList->GetItem(i);

		JString fullName;
		if (GetClassSourceFile(info.classID, &fullName))
		{
			Broadcast(ProgramStopped(Location(fullName, 1)));
		}
	}
}

/******************************************************************************
 StepOver

 *****************************************************************************/

void
jvm::Link::StepOver()
{
}

/******************************************************************************
 StepInto

 *****************************************************************************/

void
jvm::Link::StepInto()
{
}

/******************************************************************************
 StepOut

 *****************************************************************************/

void
jvm::Link::StepOut()
{
}

/******************************************************************************
 Continue

 *****************************************************************************/

void
jvm::Link::Continue()
{
	itsProgramIsStoppedFlag = false;
	itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMResumeCmd, nullptr, 0);
	Broadcast(ProgramRunning());
}

/******************************************************************************
 RunUntil

 *****************************************************************************/

void
jvm::Link::RunUntil
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
jvm::Link::SetExecutionPoint
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
jvm::Link::SetValue
	(
	const JString& name,
	const JString& value
	)
{
}

/******************************************************************************
 CreateArray2DCmd

 *****************************************************************************/

::Array2DCmd*
jvm::Link::CreateArray2DCmd
	(
	Array2DDir*			dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
{
	return jnew Array2DCmd(dir, table, data);
}

/******************************************************************************
 CreatePlot2DCmd

 *****************************************************************************/

::Plot2DCmd*
jvm::Link::CreatePlot2DCmd
	(
	Plot2DDir*		dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
{
	return jnew Plot2DCmd(dir, x, y);
}

/******************************************************************************
 CreateDisplaySourceForMainCmd

 *****************************************************************************/

::DisplaySourceForMainCmd*
jvm::Link::CreateDisplaySourceForMainCmd
	(
	SourceDirector* sourceDir
	)
{
	return jnew DisplaySourceForMainCmd(sourceDir);
}

/******************************************************************************
 CreateGetCompletionsCmd

 *****************************************************************************/

::GetCompletionsCmd*
jvm::Link::CreateGetCompletionsCmd
	(
	CommandInput*	input,
	CommandOutputDisplay*	history
	)
{
	return jnew GetCompletionsCmd(input, history);
}

/******************************************************************************
 CreateGetFrameCmd

 *****************************************************************************/

::GetFrameCmd*
jvm::Link::CreateGetFrameCmd
	(
	StackWidget* widget
	)
{
	return jnew GetFrameCmd(widget);
}

/******************************************************************************
 CreateGetStackCmd

 *****************************************************************************/

::GetStackCmd*
jvm::Link::CreateGetStackCmd
	(
	JTree*			tree,
	StackWidget*	widget
	)
{
	return jnew GetStackCmd(tree, widget);
}

/******************************************************************************
 CreateGetThreadCmd

 *****************************************************************************/

::GetThreadCmd*
jvm::Link::CreateGetThreadCmd
	(
	ThreadsWidget* widget
	)
{
	return jnew GetThreadCmd(widget);
}

/******************************************************************************
 CreateGetThreadsCmd

 *****************************************************************************/

::GetThreadsCmd*
jvm::Link::CreateGetThreadsCmd
	(
	JTree*			tree,
	ThreadsWidget*	widget
	)
{
	return jnew GetThreadsCmd(tree, widget);
}

/******************************************************************************
 CreateGetFullPathCmd

 *****************************************************************************/

::GetFullPathCmd*
jvm::Link::CreateGetFullPathCmd
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
{
	return jnew GetFullPathCmd(fileName, lineIndex);
}

/******************************************************************************
 CreateGetInitArgsCmd

 *****************************************************************************/

::GetInitArgsCmd*
jvm::Link::CreateGetInitArgsCmd
	(
	JXInputField* argInput
	)
{
	return jnew GetInitArgsCmd(argInput);
}

/******************************************************************************
 CreateGetLocalVarsCmd

 *****************************************************************************/

::GetLocalVarsCmd*
jvm::Link::CreateGetLocalVarsCmd
	(
	::VarNode* rootNode
	)
{
	return jnew GetLocalVarsCmd(rootNode);
}

/******************************************************************************
 CreateGetSourceFileListCmd

 *****************************************************************************/

::GetSourceFileListCmd*
jvm::Link::CreateGetSourceFileListCmd
	(
	FileListDir* fileList
	)
{
	return jnew GetSourceFileListCmd(fileList);
}

/******************************************************************************
 CreateVarValueCmd

 *****************************************************************************/

::VarCmd*
jvm::Link::CreateVarValueCmd
	(
	const JString& expr
	)
{
	JString s("print ");
	s += expr;

	return jnew VarCmd(s);
}

/******************************************************************************
 CreateVarContentCmd

 *****************************************************************************/

::VarCmd*
jvm::Link::CreateVarContentCmd
	(
	const JString& expr
	)
{
	JString s("print *(");
	s += expr;
	s += ")";

	return jnew VarCmd(s);
}

/******************************************************************************
 CreateVarNode

 *****************************************************************************/

::VarNode*
jvm::Link::CreateVarNode
	(
	const bool shouldUpdate		// false for Local Variables
	)
{
	auto* node = jnew VarNode(shouldUpdate);
	return node;
}

::VarNode*
jvm::Link::CreateVarNode
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
jvm::Link::Build1DArrayExpression
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
jvm::Link::Build2DArrayExpression
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
jvm::Link::CreateGetMemoryCmd
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
jvm::Link::CreateGetAssemblyCmd
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
jvm::Link::CreateGetRegistersCmd
	(
	RegistersDir* dir
	)
{
	return nullptr;
}

/******************************************************************************
 SendRaw

	Sends the given text to the process being debugged.

 *****************************************************************************/

void
jvm::Link::SendRaw
	(
	const JString& text
	)
{
	if (itsOutputLink != nullptr)
	{
		itsOutputLink->Write(text);
	}
}

/******************************************************************************
 Send

	Process NOP immediately.

 *****************************************************************************/

bool
jvm::Link::Send
	(
	Command* command
	)
{
	if (command->GetCommand() == "NOP")
	{
		command->SetTransactionID(GetNextTransactionID());
		SendMedicCommand(command);
		return true;
	}
	else
	{
		return ::Link::Send(command);
	}
}

/******************************************************************************
 SendMedicCommand (virtual protected)

 *****************************************************************************/

void
jvm::Link::SendMedicCommand
	(
	Command* command
	)
{
	command->Starting();

	if (command->GetCommand() == "NOP")
	{
		command->Finished(true);
		Cancel(command);

		if (!HasForegroundCommands())
		{
			RunNextCommand();
		}
	}
}

/******************************************************************************
 Send

 ******************************************************************************/

void
jvm::Link::Send
	(
	const Command*		command,
	const JIndex			cmdSet,
	const JIndex			cmd,
	const unsigned char*	data,
	const JSize				count
	)
{
	if (itsDebugLink != nullptr)
	{
		itsDebugLink->Send(command->GetTransactionID(), cmdSet, cmd, data, count);
	}
}

/******************************************************************************
 WaitForJVMDeath

 ******************************************************************************/

void
jvm::Link::WaitForJVMDeath()
{
	assert( itsJVMDeathTask != nullptr );

	itsJVMDeathTask = jnew JXFunctionTask(1000, [this]()
	{
		CleanUpAfterProgramFinished(nullptr);
		itsJVMDeathTask = nullptr;
	},
	true);
	itsJVMDeathTask->Start();
}

/******************************************************************************
 CleanUpAfterProgramFinished (private)

	It would be nice to detect "program finished" by *only* listening to
	itsProcess, but this doesn't work for remote debugging.

 *****************************************************************************/

void
jvm::Link::CleanUpAfterProgramFinished
	(
	const JProcess::Finished* info
	)
{
	if (IsDebugging())
	{
		StopDebugger();

		JString reasonStr;
		if (info != nullptr)
		{
			int result;
			const JChildExitReason reason = info->GetReason(&result);
			reasonStr = JPrintChildExitReason(reason, result);
		}
		else
		{
			reasonStr = JGetString("ProgramFinished::JVMLink");
		}
		reasonStr += "\n\n";

		Broadcast(UserOutput(reasonStr, false));
		Broadcast(ProgramFinished());

		StartDebugger();
	}
	else
	{
		jdelete itsProcess;
		itsProcess = nullptr;
	}
}

/******************************************************************************
 StopProgram

 *****************************************************************************/

void
jvm::Link::StopProgram()
{
	itsProgramIsStoppedFlag = true;
	itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMSuspendCmd, nullptr, 0);
	Broadcast(ProgramStopped(Location(JString::empty, 1)));
}

/******************************************************************************
 KillProgram

 *****************************************************************************/

void
jvm::Link::KillProgram()
{
	if (itsProcess != nullptr)
	{
		itsProcess->Kill();
	}
	else if (itsDebugLink != nullptr)
	{
		unsigned char data[4];
		Socket::Pack4(1, data);
		itsDebugLink->Send(GetNextTransactionID(), kVirtualMachineCmdSet, kVMExitCmd, data, sizeof(data));

		WaitForJVMDeath();
	}
}

/******************************************************************************
 DetachOrKill (private)

 *****************************************************************************/

void
jvm::Link::DetachOrKill()
{
	KillProgram();

	DeleteProcessLink();	// doesn't hurt to do it always

	jdelete itsDebugLink;
	itsDebugLink = nullptr;

	FlushClassList();
}

/******************************************************************************
 OKToDetachOrKill

 *****************************************************************************/

bool
jvm::Link::OKToDetachOrKill()
	const
{
	if (itsProcess != nullptr)
	{
		return JGetUserNotification()->AskUserYes(JGetString("WarnKillProgram::JVMLink"));
	}
	else if (itsDebugLink != nullptr)
	{
		return JGetUserNotification()->AskUserYes(JGetString("WarnDetachProgram::JVMLink"));
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
jvm::Link::StartDebugger()
{
	if (itsAcceptor == nullptr)
	{
		itsAcceptor = jnew Acceptor;
	}

	const JString portStr(kJavaPort);
	if (itsAcceptor->open(ACE_INET_Addr(kJavaPort)) == -1)
	{
		const JString errStr(jerrno());

		const JUtf8Byte* map[] =
		{
			"port",  portStr.GetBytes(),
			"errno", errStr.GetBytes()
		};
		const JString msg = JGetString("ListenError::JVMLink", map, sizeof(map));

		auto* task = jnew WelcomeTask(msg, true);
		task->Go();
		return false;
	}
	else
	{
		const JUtf8Byte* map[] =
		{
			"port", portStr.GetBytes()
		};
		JString msg = JGetString("Welcome::JVMLink", map, sizeof(map));

		auto* task = jnew WelcomeTask(msg, false);
		task->Go();
		return true;
	}
}

/******************************************************************************
 InitDebugger

 *****************************************************************************/

void
jvm::Link::InitDebugger()
{
	itsInitFinishedFlag = true;
}

/******************************************************************************
 ChangeDebugger

	This actually changes the JVM command for launching programs.

 *****************************************************************************/

bool
jvm::Link::ChangeDebugger()
{
	PrefsManager* mgr = GetPrefsManager();
	if (itsJVMCmd != mgr->GetJVMCommand() && itsProcess != nullptr)
	{
		const bool ok = RestartDebugger();
		if (ok)
		{
			RunProgram(itsJVMProcessArgs);
		}

		return ok;
	}

	return true;
}

/******************************************************************************
 RestartDebugger

 *****************************************************************************/

bool
jvm::Link::RestartDebugger()
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
jvm::Link::StopDebugger()
{
	StopListening(itsThreadTree);

	DetachOrKill();
	CancelAllCommands();

	InitFlags();
}

/******************************************************************************
 ConnectionEstablished

 *****************************************************************************/

void
jvm::Link::ConnectionEstablished
	(
	Socket* socket
	)
{
	InitFlags();
	itsProgramIsStoppedFlag = true;

	itsDebugLink = socket;
	ListenTo(itsDebugLink);

	itsAcceptor->close();

	Command* cmd = jnew GetIDSizesCmd();

	// listen for class unload

	unsigned char data[ 1+1+4 ];
	data[0] = kClassUnloadEvent;
	data[1] = kSuspendNone;
	Socket::Pack4(0, data+2);

	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd, data, sizeof(data));

	// listen for thread creation/death

	data[0] = kThreadStartEvent;
	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd, data, sizeof(data));

	data[0] = kThreadDeathEvent;
	itsDebugLink->Send(GetNextTransactionID(), kEventRequestCmdSet, kERSetCmd, data, sizeof(data));

	// build initial tree of threads

	StopListening(itsThreadTree);
	itsThreadList->CleanOut();
	itsThreadRoot->DeleteAllChildren();
	ListenTo(itsThreadTree);

	cmd = jnew GetThreadGroupsCmd(itsThreadRoot, nullptr);

	// trigger commands

	JString programName;
	GetProgram(&programName);

	Broadcast(DebuggerReadyForInput());
	Broadcast(UserOutput(JGetString("Connected::JVMLink"), false));
	Broadcast(SymbolsLoaded(true, programName));
	Broadcast(ProgramStopped(Location(JString::empty, 1)));

	// show main()

	if (!itsMainClassName.IsEmpty())
	{
		JString fullName = ClassNameToResourcePath(itsMainClassName);
		if (ClassSignatureToFile(fullName, &fullName))
		{
			((GetCommandDirector())->GetCurrentSourceDir())->DisplayFile(fullName);
		}
	}
}

/******************************************************************************
 ConnectionFinished

 *****************************************************************************/

void
jvm::Link::ConnectionFinished
	(
	Socket* socket
	)
{
	assert( socket == itsDebugLink );

	itsDebugLink = nullptr;
	DeleteProcessLink();

	RestartDebugger();
}

/******************************************************************************
 DeleteProcessLink (private)

 ******************************************************************************/

void
jvm::Link::DeleteProcessLink()
{
	delete itsOutputLink;
	itsOutputLink = nullptr;

	delete itsInputLink;
	itsInputLink = nullptr;
}
