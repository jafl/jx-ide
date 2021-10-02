/******************************************************************************
 XDLink.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_XDLink
#define _H_XDLink

#include "Link.h"
#include <jx-af/jcore/jXMLUtil.h>	// need defn of xmlNode

#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>

class XDBreakpointManager;
class XDSocket;

class XDLink : public Link
{
public:

	XDLink();

	virtual	~XDLink();

	virtual bool	DebuggerHasStarted() const override;
	virtual bool	HasLoadedSymbols() const override;
	virtual bool	IsDebugging() const override;
	virtual bool	IsDefiningScript() const override;

	virtual bool	ChangeDebugger() override;
	virtual bool	RestartDebugger() override;

	virtual JString		GetChooseProgramInstructions() const override;
	virtual bool	HasProgram() const override;
	virtual bool	GetProgram(JString* fullName) const override;
	virtual void		SetProgram(const JString& fullName) override;
	virtual void		ReloadProgram() override;
	virtual bool	HasCore() const override;
	virtual bool	GetCore(JString* fullName) const override;
	virtual void		SetCore(const JString& fullName) override;
	virtual void		AttachToProcess(const pid_t pid) override;

	virtual void		RunProgram(const JString& args) override;
	virtual void		StopProgram() override;
	virtual void		KillProgram() override;
	virtual bool	ProgramIsRunning() const override;
	virtual bool	ProgramIsStopped() const override;
	virtual bool	OKToDetachOrKill() const override;

	virtual bool	OKToSendCommands(const bool background) const override;
	void				Send(const JUtf8Byte* text);
	void				Send(const JString& text);

	virtual BreakpointManager*	GetBreakpointManager() override;

	virtual void	ShowBreakpointInfo(const JIndex debuggerIndex) override;
	virtual void	SetBreakpoint(const JString& fileName, const JIndex lineIndex,
								  const bool temporary = false) override;
	virtual void	SetBreakpoint(const JString& address,
								  const bool temporary = false) override;
	virtual void	RemoveBreakpoint(const JIndex debuggerIndex) override;
	virtual void	RemoveAllBreakpointsOnLine(const JString& fileName,
										   const JIndex lineIndex) override;
	virtual void	RemoveAllBreakpointsAtAddress(const JString& addr) override;
	virtual void	RemoveAllBreakpoints() override;
	virtual void	SetBreakpointEnabled(const JIndex debuggerIndex, const bool enabled,
									 const bool once = false) override;
	virtual void	SetBreakpointCondition(const JIndex debuggerIndex,
									   const JString& condition) override;
	virtual void	RemoveBreakpointCondition(const JIndex debuggerIndex) override;
	virtual void	SetBreakpointIgnoreCount(const JIndex debuggerIndex, const JSize count) override;

	virtual void	WatchExpression(const JString& expr) override;
	virtual void	WatchLocation(const JString& expr) override;

	virtual void	SwitchToThread(const JUInt64 id) override;
	virtual void	SwitchToFrame(const JUInt64 id) override;
	virtual void	StepOver() override;
	virtual void	StepInto() override;
	virtual void	StepOut() override;
	virtual void	Continue() override;
	virtual void	RunUntil(const JString& fileName, const JIndex lineIndex) override;
	virtual void	SetExecutionPoint(const JString& fileName, const JIndex lineIndex) override;

	virtual void	SetValue(const JString& name, const JString& value) override;

	virtual const JString&	GetPrompt()	const override;
	virtual const JString&	GetScriptPrompt() const override;

	JIndex	GetStackFrameIndex() const;

	const JPtrArray<JString>&	GetSourcePathList() const;

	// Command factory

	virtual Array2DCmd*		CreateArray2DCmd(Array2DDir* dir,
														 JXStringTable* table,
														 JStringTableData* data) override;
	virtual Plot2DCommand*		CreatePlot2DCmd(Plot2DDir* dir,
														JArray<JFloat>* x,
														JArray<JFloat>* y) override;
	virtual DisplaySourceForMainCmd*	CreateDisplaySourceForMainCmd(SourceDirector* sourceDir) override;
	virtual GetCompletionsCmd*		CreateGetCompletionsCmd(CommandInput* input,
														 CommandOutputDisplay* history) override;
	virtual GetFrameCmd*				CreateGetFrameCmd(StackWidget* widget) override;
	virtual GetStack*				CreateGetStackCmd(JTree* tree, StackWidget* widget) override;
	virtual GetThread*			CreateGetThreadCmd(ThreadsWidget* widget) override;
	virtual GetThreads*			CreateGetThreadsCmd(JTree* tree, ThreadsWidget* widget) override;
	virtual GetFullPath*			CreateGetFullPathCmd(const JString& fileName,
													  const JIndex lineIndex = 0) override;
	virtual GetInitArgs*			CreateGetInitArgsCmd(JXInputField* argInput) override;
	virtual GetLocalVars*			CreateGetLocalVarsCmd(VarNode* rootNode) override;
	virtual GetSourceFileList*	CreateGetSourceFileListCmd(FileListDir* fileList) override;
	virtual VarCommand*			CreateVarValueCmd(const JString& expr) override;
	virtual VarCommand*			CreateVarContentCmd(const JString& expr) override;
	virtual VarNode*				CreateVarNode(const bool shouldUpdate = true) override;
	virtual VarNode*				CreateVarNode(JTreeNode* parent, const JString& name,
												  const JString& fullName, const JString& value) override;
	virtual JString					Build1DArrayExpression(const JString& expr,
														   const JInteger index) override;
	virtual JString					Build2DArrayExpression(const JString& expr,
														   const JInteger rowIndex,
														   const JInteger colIndex) override;
	virtual GetMemory*			CreateGetMemoryCmd(MemoryDir* dir) override;
	virtual GetAssemblyCmd*			CreateGetAssemblyCmd(SourceDirector* dir) override;
	virtual GetRegisters*			CreateGetRegistersCmd(RegistersDir* dir) override;

	// called by XD commands

	bool	GetParsedData(xmlNode** root);

	// called by XDSocket

	void	ConnectionEstablished(XDSocket* socket);
	void	ConnectionFinished(XDSocket* socket);

	// called by XDWelcomeTask

	void	BroadcastWelcome(const JString& msg, const bool error);

	// called by XDSetProgramTask

	void	BroadcastProgramSet();

	// only when user types input for program being debugged

	virtual void	SendRaw(const JString& text) override;

protected:

	virtual void	SendMedicCommand(Command* command) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	typedef ACE_Acceptor<XDSocket, ACE_SOCK_ACCEPTOR>	XDAcceptor;

private:

	XDAcceptor*				itsAcceptor;
	XDSocket*				itsLink;		// nullptr if not connected to debugger
	XDBreakpointManager*	itsBPMgr;

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


/******************************************************************************
 BroadcastWelcome

 *****************************************************************************/

inline void
XDLink::BroadcastWelcome
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
XDLink::GetSourcePathList()
	const
{
	return *itsSourcePathList;
}

/******************************************************************************
 GetParsedData

 *****************************************************************************/

inline bool
XDLink::GetParsedData
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
XDLink::GetStackFrameIndex()
	const
{
	return itsStackFrameIndex;
}

/******************************************************************************
 Send

	Sends the given text as command(s) to xdebug.

 *****************************************************************************/

inline void
XDLink::Send
	(
	const JString& text
	)
{
	Send(text.GetBytes());
}

#endif
