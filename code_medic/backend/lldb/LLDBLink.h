/******************************************************************************
 LLDBLink.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBLink
#define _H_LLDBLink

// conflict between X11 and LLDB
#undef Status

#include "Link.h"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBListener.h"
#include <stdio.h>

class LLDBBreakpointManager;
class LLDBGetStopLocationForLink;
class LLDBGetStopLocationForAsm;

#ifdef _J_OSX
typedef int j_lldb_cookie_fn_return;
typedef int j_lldb_cookie_size;
#else
typedef long j_lldb_cookie_fn_return;
typedef unsigned long j_lldb_cookie_size;
#endif

class LLDBLink : public Link, public lldb::SBListener
{
public:

	LLDBLink();

	virtual	~LLDBLink();

	bool	DebuggerHasStarted() const override;
	bool	HasLoadedSymbols() const override;
	bool	IsDebugging() const override;
	bool	IsDefiningScript() const override;

	bool	ChangeDebugger() override;
	bool	RestartDebugger() override;

	JString	GetChooseProgramInstructions() const override;
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

	bool	OKToSendMultipleCommands() const override;
	bool	OKToSendCommands(const bool background) const override;

	BreakpointManager*	GetBreakpointManager() override;

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

	void	SwitchToThread(const JUInt64 id) override;
	void	SwitchToFrame(const JUInt64 id) override;
	void	StepOver() override;
	void	StepInto() override;
	void	StepOut() override;
	void	Continue() override;
	void	RunUntil(const JString& fileName, const JIndex lineIndex) override;
	void	SetExecutionPoint(const JString& fileName, const JIndex lineIndex) override;
	void	StepOverAssembly() override;
	void	StepIntoAssembly() override;
	void	RunUntil(const JString& addr) override;
	void	SetExecutionPoint(const JString& addr) override;
	void	BackupOver() override;
	void	BackupInto() override;
	void	BackupOut() override;
	void	BackupContinue() override;

	void	SetValue(const JString& name, const JString& value) override;

	const JString&	GetPrompt()	const override;
	const JString&	GetScriptPrompt() const override;

	// Command factory

	Array2DCmd*					CreateArray2DCmd(Array2DDir* dir,
														 JXStringTable* table,
														 JStringTableData* data) override;
	Plot2DCmd*					CreatePlot2DCmd(Plot2DDir* dir,
														JArray<JFloat>* x,
														JArray<JFloat>* y) override;
	DisplaySourceForMainCmd*	CreateDisplaySourceForMainCmd(SourceDirector* sourceDir) override;
	GetCompletionsCmd*			CreateGetCompletionsCmd(CommandInput* input,
																CommandOutputDisplay* history) override;
	GetFrameCmd*				CreateGetFrameCmd(StackWidget* widget) override;
	GetStackCmd*				CreateGetStackCmd(JTree* tree, StackWidget* widget) override;
	GetThreadCmd*				CreateGetThreadCmd(ThreadsWidget* widget) override;
	GetThreadsCmd*				CreateGetThreadsCmd(JTree* tree, ThreadsWidget* widget) override;
	GetFullPathCmd*				CreateGetFullPathCmd(const JString& fileName,
															 const JIndex lineIndex = 0) override;
	GetInitArgsCmd*				CreateGetInitArgsCmd(JXInputField* argInput) override;
	GetLocalVarsCmd*			CreateGetLocalVarsCmd(VarNode* rootNode) override;
	GetSourceFileListCmd*		CreateGetSourceFileListCmd(FileListDir* fileList) override;
	VarCmd*						CreateVarValueCmd(const JString& expr) override;
	VarCmd*						CreateVarContentCmd(const JString& expr) override;
	VarNode*					CreateVarNode(const bool shouldUpdate = true) override;
	VarNode*					CreateVarNode(JTreeNode* parent, const JString& name,
													  const JString& fullName, const JString& value) override;
	JString						Build1DArrayExpression(const JString& expr,
															   const JInteger index) override;
	JString						Build2DArrayExpression(const JString& expr,
															   const JInteger rowIndex,
															   const JInteger colIndex) override;
	GetMemoryCmd*				CreateGetMemoryCmd(MemoryDir* dir) override;
	GetAssemblyCmd*				CreateGetAssemblyCmd(SourceDirector* dir) override;
	GetRegistersCmd*			CreateGetRegistersCmd(RegistersDir* dir) override;

	// called by commands

	lldb::SBDebugger*	GetDebugger();

	// called by LLDBWelcomeTask

	void	BroadcastWelcome(const JString& msg, const bool restart);

	// called by LLDBRunBackgroundCommandTask

	void	SendMedicCommandSync(Command* command);

	// called by LLDBSymbolsLoadedTask

	void	SymbolsLoaded(const JString& fullName);

	// called by App

	void	HandleLLDBEvent();

	// mostly when user types input for program being debugged

	void	SendRaw(const JString& text) override;

protected:

	void	SendMedicCommand(Command* command) override;

private:

	lldb::SBDebugger*	itsDebugger;
	FILE*				itsStdoutStream;
	FILE*				itsStderrStream;
	JString				itsLastProgramInput;	// to avoid printing echo

	LLDBBreakpointManager*	itsBPMgr;

	JString	itsPrompt;			// to allow GetPrompt() to return JString&
	JString	itsCoreName;
	bool	itsIsAttachedFlag;	// debugging pre-existing process

private:

	bool	StartDebugger(const bool restart);
	void	InitFlags();
	void	StopDebugger();

	void	ProgramStarted(const pid_t pid);
	bool	ProgramStopped(JString* msg = nullptr);
	void	ProgramFinished1();

	void	DetachOrKill(const bool destroyTarget);

	void	Watch(const JString& expr, const bool resolveAddress);

	void	HandleLLDBEvent(const lldb::SBEvent& e);

	static j_lldb_cookie_fn_return	ReceiveLLDBMessageLine(void* baton, const char* line, j_lldb_cookie_size count);
	static j_lldb_cookie_fn_return	ReceiveLLDBErrorLine(void* baton, const char* line, j_lldb_cookie_size count);
	static void						LogLLDBMessage(const JUtf8Byte* msg, void* baton);
};


/******************************************************************************
 GetDebugger

 *****************************************************************************/

inline lldb::SBDebugger*
LLDBLink::GetDebugger()
{
	return itsDebugger;
}

/******************************************************************************
 BroadcastWelcome

 *****************************************************************************/

inline void
LLDBLink::BroadcastWelcome
	(
	const JString&	msg,
	const bool	restart
	)
{
	if (restart)
		{
		Broadcast(DebuggerRestarted());
		}
	Broadcast(DebuggerStarted());
	Broadcast(DebuggerReadyForInput());

	Broadcast(UserOutput(msg, false, false));
}

#endif
