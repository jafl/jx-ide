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

	virtual bool	OKToSendMultipleCommands() const override;
	virtual bool	OKToSendCommands(const bool background) const override;

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
	virtual void	StepOverAssembly() override;
	virtual void	StepIntoAssembly() override;
	virtual void	RunUntil(const JString& addr) override;
	virtual void	SetExecutionPoint(const JString& addr) override;
	virtual void	BackupOver() override;
	virtual void	BackupInto() override;
	virtual void	BackupOut() override;
	virtual void	BackupContinue() override;

	virtual void	SetValue(const JString& name, const JString& value) override;

	virtual const JString&	GetPrompt()	const override;
	virtual const JString&	GetScriptPrompt() const override;

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

	virtual void	SendRaw(const JString& text) override;

protected:

	virtual void	SendMedicCommand(Command* command) override;

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
