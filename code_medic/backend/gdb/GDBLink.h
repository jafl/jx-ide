/******************************************************************************
 GDBLink.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBLink
#define _H_GDBLink

#include "Link.h"

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include "Pipe.h"
#include <jx-af/jcore/JStringPtrMap.h>

class JProcess;

namespace gdb {

class BreakpointManager;
class GetStopLocationForLinkCmd;
class GetStopLocationForAsmCmd;
class PingTask;

namespace Output { class Scanner; }

class Link : public ::Link
{
public:

	Link();

	virtual	~Link();

	virtual bool	DebuggerHasStarted() const override;
	virtual bool	HasLoadedSymbols() const override;
	virtual bool	IsDebugging() const override;
	virtual bool	IsDefiningScript() const override;

	virtual bool	ChangeDebugger() override;
	virtual bool	RestartDebugger() override;

	virtual JString	GetChooseProgramInstructions() const override;
	virtual bool	HasProgram() const override;
	virtual bool	GetProgram(JString* fullName) const override;
	virtual void	SetProgram(const JString& fullName) override;
	virtual void	ReloadProgram() override;
	virtual bool	HasCore() const override;
	virtual bool	GetCore(JString* fullName) const override;
	virtual void	SetCore(const JString& fullName) override;
	virtual void	AttachToProcess(const pid_t pid) override;

	virtual void	RunProgram(const JString& args) override;
	virtual void	StopProgram() override;
	virtual void	KillProgram() override;
	virtual bool	ProgramIsRunning() const override;
	virtual bool	ProgramIsStopped() const override;
	virtual bool	OKToDetachOrKill() const override;

	virtual bool	OKToSendMultipleCommands() const override;
	virtual bool	OKToSendCommands(const bool background) const override;
	void			Send(const JString& text);
	void			SendWhenStopped(const JString& text);

	virtual ::BreakpointManager*	GetBreakpointManager() override;

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

	virtual ::Array2DCmd*				CreateArray2DCmd(Array2DDir* dir,
														 JXStringTable* table,
														 JStringTableData* data) override;
	virtual ::Plot2DCmd*				CreatePlot2DCmd(Plot2DDir* dir,
														JArray<JFloat>* x,
														JArray<JFloat>* y) override;
	virtual ::DisplaySourceForMainCmd*	CreateDisplaySourceForMainCmd(SourceDirector* sourceDir) override;
	virtual ::GetCompletionsCmd*		CreateGetCompletionsCmd(CommandInput* input,
																CommandOutputDisplay* history) override;
	virtual ::GetFrameCmd*				CreateGetFrameCmd(StackWidget* widget) override;
	virtual ::GetStackCmd*				CreateGetStackCmd(JTree* tree, StackWidget* widget) override;
	virtual ::GetThreadCmd*				CreateGetThreadCmd(ThreadsWidget* widget) override;
	virtual ::GetThreadsCmd*			CreateGetThreadsCmd(JTree* tree, ThreadsWidget* widget) override;
	virtual ::GetFullPathCmd*			CreateGetFullPathCmd(const JString& fileName,
															 const JIndex lineIndex = 0) override;
	virtual ::GetInitArgsCmd*			CreateGetInitArgsCmd(JXInputField* argInput) override;
	virtual ::GetLocalVarsCmd*			CreateGetLocalVarsCmd(::VarNode* rootNode) override;
	virtual ::GetSourceFileListCmd*		CreateGetSourceFileListCmd(FileListDir* fileList) override;
	virtual ::VarCmd*					CreateVarValueCmd(const JString& expr) override;
	virtual ::VarCmd*					CreateVarContentCmd(const JString& expr) override;
	virtual ::VarNode*					CreateVarNode(const bool shouldUpdate = true) override;
	virtual ::VarNode*					CreateVarNode(JTreeNode* parent, const JString& name,
													  const JString& fullName, const JString& value) override;
	virtual JString						Build1DArrayExpression(const JString& expr,
															   const JInteger index) override;
	virtual JString						Build2DArrayExpression(const JString& expr,
															   const JInteger rowIndex,
															   const JInteger colIndex) override;
	virtual ::GetMemoryCmd*				CreateGetMemoryCmd(MemoryDir* dir) override;
	virtual ::GetAssemblyCmd*			CreateGetAssemblyCmd(SourceDirector* dir) override;
	virtual ::GetRegistersCmd*			CreateGetRegistersCmd(RegistersDir* dir) override;

	// called by GDB commands

	void	SaveProgramName(const JString& fileName);
	void	SaveCoreName(const JString& fileName);

	static bool	ParseList(std::istringstream& stream, JPtrArray<JString>* list, const JUtf8Byte terminator = '}');
	static bool	ParseMap(std::istringstream& stream, JStringPtrMap<JString>* map);
	static bool	ParseMapArray(std::istringstream& stream, JPtrArray< JStringPtrMap<JString> >* list);

	// called by ChooseProcessDialog

	void	ProgramStarted(const pid_t pid);

	// called by GDBGetStopLocationCmd*

	void	SendProgramStopped(const Location& location);
	void	SendProgramStopped2(const Location& location);

	// called by GDBDisplaySourceForMainCmd

	void	FirstBreakImpossible();

	// called by GDBPingTask

	void	SendPing();

	// only when user types input for program being debugged

	virtual void	SendRaw(const JString& text) override;

protected:

	virtual void	SendMedicCommand(Command* command) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	typedef Pipe<ACE_LSOCK_STREAM>	ProcessLink;

private:

	JString		itsDebuggerCmd;
	JProcess*	itsDebuggerProcess;		// can be nullptr
	JProcess*	itsChildProcess;		// nullptr until aquired pid

	ProcessLink*	itsOutputLink;		// nullptr if debugger not started
	ProcessLink*	itsInputLink;		// nullptr if debugger not started

	JString	itsProgramName;				// from "info file"
	JString	itsCoreName;				// from "info file"
	bool	itsHasStartedFlag;			// debugger is running
	bool	itsInitFinishedFlag;		// debugger has been fully initialized
	bool	itsSymbolsLoadedFlag;		// debugger has loaded symbols
	bool	itsDebuggerBusyFlag;		// debugger is busy
	bool	itsIsDebuggingFlag;			// a process is being debugged
	bool	itsIsAttachedFlag;			// debugging pre-existing process
	bool	itsProgramIsStoppedFlag;	// the process is stopped
	bool	itsFirstBreakFlag;			// next program stop is to get pid
	bool	itsPrintingOutputFlag;		// output is being printed to GUI
	bool	itsDefiningScriptFlag;		// debugger is printing prompt ">"
	bool	itsWaitingToQuitFlag;		// user stopped the debugger
	JSize	itsContinueCount;			// # of prompts with empty fg Q before "continue"
	JIndex	itsPingID;

	gdb::Output::Scanner*			itsScanner;
	gdb::BreakpointManager*			itsBPMgr;
	gdb::GetStopLocationForLinkCmd*	itsGetStopLocation;
	gdb::GetStopLocationForAsmCmd*	itsGetStopLocation2;
	gdb::PingTask*					itsPingTask;

private:

	bool	StartDebugger();
	void	InitDebugger();
	void	InitFlags();
	void	StopDebugger();
	void	ReadFromDebugger();

	void	DetachOrKill();

	void	ProgramFinished1();

	void	PrivateSendProgramStopped(const Location& location);
};

};

#endif
