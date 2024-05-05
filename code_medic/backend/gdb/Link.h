/******************************************************************************
 Link.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBLink
#define _H_GDBLink

#include <Link.h>

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

#define GDB_COMMAND_PREFIX "\x1a\x1a"

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
	void	Send(const JString& text);
	void	SendWhenStopped(const JString& text);

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
	void	StepOverAssembly() override;
	void	StepIntoAssembly() override;
	void	RunUntil(const JString& addr) override;
	void	SetExecutionPoint(const JString& addr) override;
	void	BackupOver() override;
	void	BackupInto() override;
	void	BackupOut() override;
	void	BackupContinue() override;

	void	SetValue(const JString& name, const JString& value) override;

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
	bool						CreateVarTypeCmd(const JString& expr, ::VarTypeCmd** cmd) override;
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

	void	SendRaw(const JString& text) override;

protected:

	void	SendMedicCommand(Command* command) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	using ProcessLink = Pipe<ACE_LSOCK_STREAM>;

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

	void	CleanUpAfterProgramFinished();

	void	PrivateSendProgramStopped(const Location& location);
};

};

#endif
