/******************************************************************************
 Link.h

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#ifndef _H_JVMLink
#define _H_JVMLink

#include <Link.h>
#include "jvm/Socket.h"
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/JAliasArray.h>

#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include "Pipe.h"

class JProcess;
class JXFunctionTask;

namespace jvm {

class Socket;
class BreakpointManager;
class ThreadNode;

class Link : public ::Link
{
public:

	enum
	{
		kVirtualMachineCmdSet = 1,
		kReferenceTypeCmdSet,
		kClassTypeCmdSet,
		kArrayTypeCmdSet,
		kInterfaceTypeCmdSet,
		kMethodCmdSet,
		kFieldCmdSet = 8,
		kObjectReferenceCmdSet,
		kStringReferenceCmdSet,
		kThreadReferenceCmdSet,
		kThreadGroupReferenceCmdSet,
		kArrayReferenceCmdSet,
		kClassLoaderReferenceCmdSet,
		kEventRequestCmdSet,
		kStackFrameCmdSet,
		kClassObjectReferenceCmdSet,
		kEventCmdSet = 64,

		// kReferenceTypeCmdSet

		kRTSignatureCmd = 1,
		kRTClassLoaderCmd,
		kRTModifiersCmd,
		kRTFieldsCmd,
		kRTMethodsCmd,
		kRTGetValuesCmd,
		kRTSourceFileCmd,
		kRTNestedTypesCmd,
		kRTStatusCmd,
		kRTInterfacesCmd,
		kRTClassObjectCmd,

		// kVirtualMachineCmdSet

		kVMVersionCmd = 1,
		kVMClassesBySignatureCmd,
		kVMAllClassesCmd,
		kVMAllThreadsCmd,
		kVMTopLevelThreadGroupsCmd,
		kVMDisposeCmd,
		kVMIDSizesCmd,
		kVMSuspendCmd,
		kVMResumeCmd,
		kVMExitCmd,

		// kThreadReferenceCmdSet

		kTNameCmd = 1,
		kTSuspendCmd,
		kTResumeCmd,
		kTStatusCmd,
		kTThreadGroupCmd,
		kTFramesCmd,
		kTFrameCountCmd,
		kTOwnedMonitorsCmd,
		kTCurrentContendedMonitorCmd,
		kTStopCmd,
		kTInterruptCmd,
		kTSuspendCountCmd,
		kTOwnedMonitorsStackDepthInfoCmd,
		kTForceEarlyReturnCmd,

		// kThreadGroupReferenceCmdSet

		kTGNameCmd = 1,
		kTGParentCmd,
		kTGChildrenCmd,

		// kEventRequestCmdSet

		kERSetCmd = 1,
		kERClearCmd,
		kERClearAllBreakpointsCmd,

		// kEventCmdSet

		kECompositeCmd = 100,

		// class status

		kClassVerifiedFlag    = 1,
		kClassPreparedFlag    = 2,
		kClassInitializedFlag = 4,
		kClassErrorFlag       = 8,

		// event types

		kSingleStepEvent = 1,
		kBreakpointEvent,
		kFramePopEvent,
		kExceptionEvent,
		kUserDefinedEvent,
		kThreadStartEvent,
		kThreadDeathEvent,
		kClassPrepareEvent,
		kClassUnloadEvent,
//		kClassLoadEvent,
		kFieldAccessEvent = 20,
		kFieldModificationEvent,
		kExceptionCatchEvent = 30,
		kMethodEntryEvent = 40,
		kMethodExitEvent,
		kMethodExitWithReturnValueEvent,
		kMonitorContendedEnterEvent,
		kMonitorContendedEnteredEvent,
		kMonitorWaitEvent,
		kMonitorWaitedEvent,
		kVMStartEvent = 90,
		kVMDeathEvent = 99,

		// suspend policy

		kSuspendNone = 0,
		kSuspendEventThread,
		kSuspendAllThreads,

		// errors

		kVMDeadErrorCode = 112
	};

	enum ReferenceType
	{
		kClassType = 1,
		kInterfaceType,
		kArrayType
	};

public:

	Link();

	~Link() override;

	bool	DebuggerHasStarted() const override;
	bool	HasLoadedSymbols() const override;
	bool	IsDebugging() const override;
	bool	IsDefiningScript() const override;

	bool	ChangeDebugger() override;
	bool	RestartDebugger() override;

	bool	HasProgram() const override;
	bool	GetProgram(JString* fullName) const override;
	void	SetProgram(const JString& fullName) override;
	void	ReloadProgram() override;
	bool	HasCore() const override;
	bool	GetCore(JString* fullName) const override;
	void	SetCore(const JString& fullName) override;
	void	AttachToProcess(const pid_t pid) override;

	void	RunProgram(const JString& args) override;
	void	StopProgram() override;
	void	KillProgram() override;
	bool	ProgramIsRunning() const override;
	bool	ProgramIsStopped() const override;
	bool	OKToDetachOrKill() const override;

	bool	OKToSendCommands(const bool background) const override;
	bool	Send(Command* cmd) override;

	::BreakpointManager*	GetBreakpointManager() override;

	void	ShowBreakpointInfo(const JIndex debuggerIndex) override;
	void	SetBreakpoint(const JString& fileName, const JIndex lineIndex,
							  const bool temporary = false) override;
	void	SetBreakpoint(const JString& address,
							  const bool temporary = false) override;
	void	RemoveBreakpoint(const JIndex debuggerIndex) override;
	void	RemoveAllBreakpointsOnLine(const JString& fileName,
									   const JIndex lineIndex) override;
	void	RemoveAllBreakpointsAtAddress(const JString& addr) override;
	void	RemoveAllBreakpoints() override;
	void	SetBreakpointEnabled(const JIndex debuggerIndex, const bool enabled,
								 const bool once = false) override;
	void	SetBreakpointCondition(const JIndex debuggerIndex,
								   const JString& condition) override;
	void	RemoveBreakpointCondition(const JIndex debuggerIndex) override;
	void	SetBreakpointIgnoreCount(const JIndex debuggerIndex, const JSize count) override;

	void	WatchExpression(const JString& expr) override;
	void	WatchLocation(const JString& expr) override;

	void	SwitchToThread(const JIndex id) override;
	void	SwitchToFrame(const JIndex id) override;
	void	StepOver() override;
	void	StepInto() override;
	void	StepOut() override;
	void	Continue() override;
	void	RunUntil(const JString& fileName, const JIndex lineIndex) override;
	void	SetExecutionPoint(const JString& fileName, const JIndex lineIndex) override;

	void	SetValue(const JString& name, const JString& value) override;

	JSize	GetFieldIDSize() const;
	JSize	GetMethodIDSize() const;
	JSize	GetObjectIDSize() const;
	JSize	GetReferenceTypeIDSize() const;
	JSize	GetFrameIDSize() const;

	ThreadNode*	GetThreadRoot();
	JUInt64		GetCurrentThreadID() const;
	void		ThreadCreated(ThreadNode* node);
	void		ThreadDeleted(ThreadNode* node);
	bool		FindThread(const JUInt64 id, ThreadNode** node) const;

	void	FlushClassList();
	void	AddClass(const JUInt64 id, const JString& signature);
	void	AddMethod(const JUInt64 classID, const JUInt64 methodID, const JString& name);

	bool	GetClassName(const JUInt64 id, JString* name);
	bool	GetClassSourceFile(const JUInt64 id, JString* fullName);
	bool	GetMethodName(const JUInt64 classID, const JUInt64 methodID, JString* name);

	static JString	ClassSignatureToResourcePath(const JString& signature);
	static JString	ClassSignatureToName(const JString& signature);
	static JString	ClassNameToResourcePath(const JString& name);

	const JPtrArray<JString>&	GetSourcePathList() const;

	void	FlushFrameList();
	void	AddFrame(const JUInt64 id, const JUInt64 classID,
					 const JUInt64 methodID, const JUInt64 codeOffset);

	// Command factory

	::Array2DCmd*				CreateArray2DCmd(Array2DDir* dir,
												 JXStringTable* table,
												 JStringTableData* data) override;
	::Plot2DCmd*				CreatePlot2DCmd(Plot2DDir* dir,
												JArray<JFloat>* x,
												JArray<JFloat>* y) override;
	::DisplaySourceForMainCmd*	CreateDisplaySourceForMainCmd(SourceDirector* sourceDir) override;
	::GetCompletionsCmd*		CreateGetCompletionsCmd(CommandInput* input,
														CommandOutputDisplay* history) override;
	::GetFrameCmd*				CreateGetFrameCmd(StackWidget* widget) override;
	::GetStackCmd*				CreateGetStackCmd(JTree* tree, StackWidget* widget) override;
	::GetThreadCmd*				CreateGetThreadCmd(ThreadsWidget* widget) override;
	::GetThreadsCmd*			CreateGetThreadsCmd(JTree* tree, ThreadsWidget* widget) override;
	::GetFullPathCmd*			CreateGetFullPathCmd(const JString& fileName,
													 const JIndex lineIndex = 0) override;
	::GetInitArgsCmd*			CreateGetInitArgsCmd(JXInputField* argInput) override;
	::GetLocalVarsCmd*			CreateGetLocalVarsCmd(::VarNode* rootNode) override;
	::GetSourceFileListCmd*		CreateGetSourceFileListCmd(FileListDir* fileList) override;
	::VarCmd*					CreateVarValueCmd(const JString& expr) override;
	::VarCmd*					CreateVarContentCmd(const JString& expr) override;
	::VarNode*					CreateVarNode(const bool shouldUpdate = true) override;
	::VarNode*					CreateVarNode(JTreeNode* parent, const JString& name,
											  const JString& fullName, const JString& value) override;
	JString						Build1DArrayExpression(const JString& expr,
													   const JInteger index) override;
	JString						Build2DArrayExpression(const JString& expr,
													   const JInteger rowIndex,
													   const JInteger colIndex) override;
	::GetMemoryCmd*				CreateGetMemoryCmd(MemoryDir* dir) override;
	::GetAssemblyCmd*			CreateGetAssemblyCmd(SourceDirector* dir) override;
	::GetRegistersCmd*			CreateGetRegistersCmd(RegistersDir* dir) override;

	// called by JVM commands

	void	Send(const Command* command, const JIndex cmdSet, const JIndex cmd,
				 const unsigned char* data, const JSize count);
	bool	GetLatestMessageFromJVM(const Socket::MessageReady** msg) const;

	// called by JVMDSocket

	void	InitDebugger();
	void	ConnectionEstablished(Socket* socket);
	void	ConnectionFinished(Socket* socket);

	// called by JVMWelcomeTask

	void	BroadcastWelcome(const JString& msg, const bool error);

	// called by JVMSetProgramTask

	void	BroadcastProgramSet();

	// called by JVMGetIDSizesCmd

	void	SetIDSizes(const JSize fieldIDSize, const JSize methodIDSize,
					   const JSize objectIDSize, const JSize refTypeIDSize,
					   const JSize frameIDSize);

	// only when user types input for program being debugged

	void	SendRaw(const JString& text) override;

protected:

	void	SendMedicCommand(Command* command) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	using Acceptor    = ACE_Acceptor<Socket, ACE_SOCK_ACCEPTOR>;
	using ProcessLink = Pipe<ACE_LSOCK_STREAM>;

public:

	struct MethodInfo
	{
		JUInt64		id;
		JString*	name;

		// remember to update AddMethod()

		void Clean()
		{
			jdelete name;
			name = nullptr;
		}
	};

	struct ClassInfo
	{
		JUInt64				id;
//		ReferenceType		type;	// can't get this via kReferenceTypeCmdSet
		JString*			name;
		JString*			path;	// can be be null
		JArray<MethodInfo>*	methods;

		// remember to update AddClass()

		void Clean()
		{
			jdelete name;
			name = nullptr;

			jdelete path;
			path = nullptr;

			const JSize count = methods->GetItemCount();
			for (JIndex i=1; i<=count; i++)
				{
				methods->GetItem(i).Clean();
				}

			jdelete methods;
			methods = nullptr;
		}
	};

	struct FrameInfo
	{
		JUInt64		id;
		JUInt64		classID;
		JUInt64		methodID;
		JUInt64		codeOffset;

		// remember to update AddFrame()
	};

private:

	Acceptor*			itsAcceptor;
	Socket*				itsDebugLink;	// nullptr if not connected to JVM
	JProcess*			itsProcess;		// nullptr unless we started the JVM
	ProcessLink*		itsOutputLink;	// nullptr unless we started the JVM
	ProcessLink*		itsInputLink;	// nullptr unless we started the JVM
	BreakpointManager*	itsBPMgr;

	JString itsJVMCWD;
	JString	itsJVMCmd;
	JString	itsJVMExecArgs;
	JString	itsJVMProcessArgs;

	bool	itsInitFinishedFlag;		// debugger has been fully initialized
	JString	itsProgramConfigFileName;	// .medic config file
	JString	itsMainClassName;			// can be empty
	bool	itsProgramIsStoppedFlag;	// the JVM is stopped

	const Socket::MessageReady*	itsLatestMsg;

	JSize	itsFieldIDSize;
	JSize	itsMethodIDSize;
	JSize	itsObjectIDSize;
	JSize	itsReferenceTypeIDSize;
	JSize	itsFrameIDSize;

	JPtrArray<JString>*		itsSourcePathList;
	JArray<ClassInfo>*		itsClassByIDList;
	JAliasArray<ClassInfo>*	itsClassByNameList;

	JTree*					itsThreadTree;
	ThreadNode*				itsThreadRoot;
	JUInt64					itsCurrentThreadID;
	JPtrArray<ThreadNode>*	itsThreadList;
	JXFunctionTask*			itsCullThreadGroupsTask;
	JIndex					itsCullThreadGroupIndex;

	JArray<FrameInfo>*	itsFrameList;

	JXFunctionTask*	itsJVMDeathTask;

private:

	bool	StartDebugger();
	void	InitFlags();
	void	ReceiveMessageFromJVM(const Socket::MessageReady& info);
	void	DispatchEventsFromJVM(const unsigned char* data, const JSize length);
	void	ReadFromProcess();
	void	StopDebugger();
	void	DeleteProcessLink();

	void	DetachOrKill();

	void	WaitForJVMDeath();
	void	CleanUpAfterProgramFinished(const JProcess::Finished* info);

	void	CheckNextThreadGroup();

	bool	ClassSignatureToFile(const JString& signature, JString* fullName) const;
	bool	FindSourceForClass(const JString& signature, JString* fullName);
	bool	GetFrame(const JUInt64 id, JIndex* index) const;

	static std::weak_ordering	CompareClassIDs(const ClassInfo& c1, const ClassInfo& c2);
	static std::weak_ordering	CompareClassNames(const ClassInfo& c1, const ClassInfo& c2);
	static std::weak_ordering	CompareMethodIDs(const MethodInfo& m1, const MethodInfo& m2);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kIDResolved;

	class IDResolved : public ::JBroadcaster::Message
	{
	public:

		IDResolved(const JUInt64 id)
			:
			JBroadcaster::Message(kIDResolved),
			itsID(id)
			{ };

		JUInt64
		GetID()
			const
		{
			return itsID;
		};

	private:

		const JUInt64 itsID;
	};
};

};

/******************************************************************************
 BroadcastWelcome

 *****************************************************************************/

inline void
jvm::Link::BroadcastWelcome
	(
	const JString&	msg,
	const bool		error
	)
{
	Broadcast(DebuggerBusy());
	Broadcast(UserOutput(msg, error));
}

/******************************************************************************
 GetSourcePathList

 *****************************************************************************/

inline const JPtrArray<JString>&
jvm::Link::GetSourcePathList()
	const
{
	return *itsSourcePathList;
}

/******************************************************************************
 GetLatestMessageFromJVM

 *****************************************************************************/

inline bool
jvm::Link::GetLatestMessageFromJVM
	(
	const Socket::MessageReady** msg
	)
	const
{
	*msg = itsLatestMsg;
	return itsLatestMsg != nullptr;
}

/******************************************************************************
 Get*IDSize

 *****************************************************************************/

inline JSize
jvm::Link::GetFieldIDSize()
	const
{
	return itsFieldIDSize;
}

inline JSize
jvm::Link::GetMethodIDSize()
	const
{
	return itsMethodIDSize;
}

inline JSize
jvm::Link::GetObjectIDSize()
	const
{
	return itsObjectIDSize;
}

inline JSize
jvm::Link::GetReferenceTypeIDSize()
	const
{
	return itsReferenceTypeIDSize;
}

inline JSize
jvm::Link::GetFrameIDSize()
	const
{
	return itsFrameIDSize;
}

/******************************************************************************
 GetThreadRoot

 *****************************************************************************/

inline jvm::ThreadNode*
jvm::Link::GetThreadRoot()
{
	return itsThreadRoot;
}

/******************************************************************************
 GetCurrentThreadID

 *****************************************************************************/

inline JUInt64
jvm::Link::GetCurrentThreadID()
	const
{
	return itsCurrentThreadID;
}

#endif
