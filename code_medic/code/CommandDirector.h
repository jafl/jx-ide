/******************************************************************************
 CommandDirector.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CommandDirector
#define _H_CommandDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPtrArray-JString.h>

class JXMenuBar;
class JXTextMenu;
class JXTextButton;
class JXTEBase;
class JXInputField;
class JXStaticText;
class JXDownRect;
class JXToolBar;
class JXDockWidget;
class JXStringHistoryMenu;
class JXKeyModifiers;
class Link;
class CommandOutputDisplay;
class CommandInput;
class SourceDirector;
class ThreadsDir;
class StackDir;
class BreakpointsDir;
class VarTreeDir;
class LocalVarsDir;
class Array1DDir;
class Array2DDir;
class Plot2DDir;
class MemoryDir;
class RegistersDir;
class FileListDir;
class DebugDir;
class GetInitArgsCmd;

class CommandDirector : public JXWindowDirector
{
public:

	CommandDirector(JXDirector* supervisor);

	~CommandDirector() override;

	bool			Close() override;
	const JString&	GetName() const override;
	bool			GetMenuIcon(const JXImage** icon) const override;

	void	InitializeCommandOutput();
	void	PrepareCommand(const JString& cmd);

	void	RunProgram();

	void	OpenSourceFiles();
	void	OpenSourceFile(const JString& file, const JSize lineIndex = 0,
						   const bool askDebuggerWhenRelPath = true);
	void	ReportUnreadableSourceFile(const JString& fileName) const;

	void	DisassembleFunction(const JString& fn, const JString& addr = JString::empty);

	void	LoadConfig();
	void	SaveConfig();

	void	DisplayExpression(const JString& expr);
	void	Display1DArray(const JString& expr);
	void	Display2DArray(const JString& expr);
	void	Plot1DArray(const JString& expr);

	void	CloseDynamicDirectors();

	JXTextMenu*	CreateDebugMenu(JXMenuBar* menuBar);
	void		AdjustDebugMenu(JXTextMenu* menu);
	void		UpdateDebugMenu(JXTextMenu* menu, JXTEBase* te1, JXTEBase* te2);
	void		HandleDebugMenu(JXTextMenu* menu, const JIndex index, JXTEBase* te1, JXTEBase* te2);
	static bool	UpgradeWindowsAndDebugMenusToolBarID(JString* s);

	static void	CreateWindowsMenuAndToolBar(JXMenuBar* menuBar, JXToolBar* toolBar,
											const bool includeStepAsm,
											const bool includeCmdLine,
											const bool includeCurrSrc,
											JXTextMenu* debugMenu,
											JXTextMenu* prefsMenu,
											JXTextMenu* helpMenu,
											std::function<void(JString*)>* upgradeToolBarID);

	SourceDirector*	GetCurrentSourceDir();
	ThreadsDir*		GetThreadsDir();
	StackDir*		GetStackDir();
	BreakpointsDir*	GetBreakpointsDir();
	VarTreeDir*		GetVarTreeDir();
	LocalVarsDir*	GetLocalVarsDir();
	RegistersDir*	GetRegistersDir();
	FileListDir*	GetFileListDir();
	DebugDir*		GetDebugDir();

	// called by SourceText

	void	TransferKeyPressToInput(const JUtf8Character& c,
									const int keySym, const JXKeyModifiers& modifiers);

	// called by dynamically created directors

	void	SourceWindowClosed(SourceDirector* dir);

	void	DirectorCreated(Array1DDir* dir);
	void	DirectorDeleted(Array1DDir* dir);

	void	DirectorCreated(Array2DDir* dir);
	void	DirectorDeleted(Array2DDir* dir);

	void	DirectorCreated(Plot2DDir* dir);
	void	DirectorDeleted(Plot2DDir* dir);

	void	DirectorCreated(MemoryDir* dir);
	void	DirectorDeleted(MemoryDir* dir);

	// called by DockManager

	void	DockAll(JXDockWidget* srcDock, JXDockWidget* infoDock,
					JXDockWidget* dataDock);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	Link*			itsLink;
	SourceDirector*	itsCurrentSourceDir;
	SourceDirector*	itsCurrentAsmDir;
	ThreadsDir*		itsThreadsDir;
	StackDir*		itsStackDir;
	BreakpointsDir*	itsBreakpointsDir;
	VarTreeDir*		itsVarTreeDir;
	LocalVarsDir*	itsLocalVarsDir;
	RegistersDir*	itsRegistersDir;
	FileListDir*	itsFileListDir;
	DebugDir*		itsDebugDir;

	JPtrArray<SourceDirector>*	itsSourceDirs;
	JPtrArray<SourceDirector>*	itsAsmDirs;
	JPtrArray<Array1DDir>*		itsArray1DDirs;
	JPtrArray<Array2DDir>*		itsArray2DDirs;
	JPtrArray<Plot2DDir>*		itsPlot2DDirs;
	JPtrArray<MemoryDir>*		itsMemoryDirs;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsDebugMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	JIndex	itsHistoryIndex;
	JString	itsCurrentHistoryFile;
	JString	itsCurrentCommand;
	bool	itsWaitingToRunFlag;

	GetInitArgsCmd*	itsGetArgsCmd;		// nullptr except at startup; deleted by Link

// begin JXLayout

	JXToolBar*            itsToolBar;
	CommandOutputDisplay* itsCommandOutput;
	JXDownRect*           itsDownRect;
	JXStaticText*         itsFakePrompt;
	CommandInput*         itsCommandInput;
	JXStringHistoryMenu*  itsHistoryMenu;
	JXTextButton*         itsProgramButton;
	JXInputField*         itsArgInput;

// end JXLayout

private:

	JXMenuBar*	BuildWindow();
	void		UpdateWindowTitle(const JString& programName);

	void	HandleUserInput();
	void	HandleCompletionRequest();
	void	ShowHistoryCommand(const JInteger delta);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	SaveInCurrentFile();
	void	SaveInNewFile();

	static void	AddWindowsMenuItemsToToolBar(JXToolBar* toolBar,
											 JXTextMenu* windowsMenu,
											 const bool includeCmdLine,
											 const bool includeCurrSrc);
	static void	AddDebugMenuItemsToToolBar(JXToolBar* toolBar, JXTextMenu* debugMenu,
										   const bool includeStepAsm);

	// Debug menu

	void	ChangeProgram();
	void	ReloadProgram();
	void	ChooseCoreFile();
	void	ChooseProcess();

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);
};


/******************************************************************************
 Directors

 *****************************************************************************/

inline SourceDirector*
CommandDirector::GetCurrentSourceDir()
{
	return itsCurrentSourceDir;
}

inline ThreadsDir*
CommandDirector::GetThreadsDir()
{
	return itsThreadsDir;
}

inline StackDir*
CommandDirector::GetStackDir()
{
	return itsStackDir;
}

inline BreakpointsDir*
CommandDirector::GetBreakpointsDir()
{
	return itsBreakpointsDir;
}

inline VarTreeDir*
CommandDirector::GetVarTreeDir()
{
	return itsVarTreeDir;
}

inline LocalVarsDir*
CommandDirector::GetLocalVarsDir()
{
	return itsLocalVarsDir;
}

inline RegistersDir*
CommandDirector::GetRegistersDir()
{
	return itsRegistersDir;
}

inline FileListDir*
CommandDirector::GetFileListDir()
{
	return itsFileListDir;
}

inline DebugDir*
CommandDirector::GetDebugDir()
{
	return itsDebugDir;
}

/******************************************************************************
 Track dynamically created directors

 ******************************************************************************/

inline void
CommandDirector::SourceWindowClosed
	(
	SourceDirector* dir
	)
{
	itsSourceDirs->Remove(dir);
	itsAsmDirs->Remove(dir);
}

inline void
CommandDirector::DirectorCreated
	(
	Array1DDir* dir
	)
{
	itsArray1DDirs->Append(dir);
}

inline void
CommandDirector::DirectorDeleted
	(
	Array1DDir* dir
	)
{
	itsArray1DDirs->Remove(dir);
}

inline void
CommandDirector::DirectorCreated
	(
	Array2DDir* dir
	)
{
	itsArray2DDirs->Append(dir);
}

inline void
CommandDirector::DirectorDeleted
	(
	Array2DDir* dir
	)
{
	itsArray2DDirs->Remove(dir);
}

inline void
CommandDirector::DirectorCreated
	(
	Plot2DDir* dir
	)
{
	itsPlot2DDirs->Append(dir);
}

inline void
CommandDirector::DirectorDeleted
	(
	Plot2DDir* dir
	)
{
	itsPlot2DDirs->Remove(dir);
}

inline void
CommandDirector::DirectorCreated
	(
	MemoryDir* dir
	)
{
	itsMemoryDirs->Append(dir);
}

inline void
CommandDirector::DirectorDeleted
	(
	MemoryDir* dir
	)
{
	itsMemoryDirs->Remove(dir);
}

#endif
