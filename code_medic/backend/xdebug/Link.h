/******************************************************************************
 Link.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDLink
#define _H_XDLink

#include <Link.h>
#include <jx-af/jcore/jXMLUtil.h>	// for xmlNode

#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>

namespace xdebug {

class BreakpointManager;
class Socket;

class Link : public ::Link
{
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
	void	Send(const JUtf8Byte* text);
	void	Send(const JString& text);

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

	JIndex	GetStackFrameIndex() const;

	const JPtrArray<JString>&	GetSourcePathList() const;

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

	// called by xdebug commands

	bool	GetParsedData(xmlNode** root);

	// called by Socket

	void	ConnectionEstablished(Socket* socket);
	void	ConnectionFinished(Socket* socket);

	// called by WelcomeTask

	void	BroadcastWelcome(const JString& msg, const bool error);

	// called by SetProgramTask

	void	BroadcastProgramSet();

	// only when user types input for program being debugged

	void	SendRaw(const JString& text) override;

protected:

	void	SendMedicCommand(Command* command) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	using Acceptor = ACE_Acceptor<Socket, ACE_SOCK_ACCEPTOR>;

private:

	Acceptor*			itsAcceptor;
	Socket*				itsLink;		// nullptr if not connected to debugger
	BreakpointManager*	itsBPMgr;

	JString	itsIDEKey;
	JString	itsScriptURI;
	JIndex	itsStackFrameIndex;
	bool	itsInitFinishedFlag;		// debugger has been fully initialized
	bool	itsProgramIsStoppedFlag;	// the process is stopped
	bool	itsDebuggerBusyFlag;		// debugger is busy
	JString	itsProgramConfigFileName;	// .medic config file
	JString	itsProgramName;				// can be empty

	xmlNode*	itsParsedDataRoot;

	JPtrArray<JString>*		itsSourcePathList;

private:

	bool	StartDebugger();
	void	InitFlags();
	void	StopDebugger();
	void	ReceiveMessageFromDebugger();

	void	SendProgramStopped(const Location& location);
};

};


/******************************************************************************
 BroadcastWelcome

 *****************************************************************************/

inline void
xdebug::Link::BroadcastWelcome
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
xdebug::Link::GetSourcePathList()
	const
{
	return *itsSourcePathList;
}

/******************************************************************************
 GetParsedData

 *****************************************************************************/

inline bool
xdebug::Link::GetParsedData
	(
	xmlNode** root
	)
{
	*root = itsParsedDataRoot;
	return itsParsedDataRoot != nullptr;
}

/******************************************************************************
 GetStackFrameIndex

 *****************************************************************************/

inline JIndex
xdebug::Link::GetStackFrameIndex()
	const
{
	return itsStackFrameIndex;
}

/******************************************************************************
 Send

	Sends the given text as command(s) to xdebug.

 *****************************************************************************/

inline void
xdebug::Link::Send
	(
	const JString& text
	)
{
	Send(text.GetBytes());
}

#endif
