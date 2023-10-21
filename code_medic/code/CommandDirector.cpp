/******************************************************************************
 CommandDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997-2004 by John Lindal.

 ******************************************************************************/

#include "CommandDirector.h"
#include "SourceDirector.h"
#include "ThreadsDir.h"
#include "StackDir.h"
#include "BreakpointsDir.h"
#include "VarTreeDir.h"
#include "Array1DDir.h"
#include "Array2DDir.h"
#include "Plot2DDir.h"
#include "MemoryDir.h"
#include "LocalVarsDir.h"
#include "RegistersDir.h"
#include "FileListDir.h"
#include "DebugDir.h"

#include "MDIServer.h"
#include "CommandOutputDisplay.h"
#include "CommandInput.h"
#include "GetCompletionsCmd.h"
#include "EditCommandsDialog.h"
#include "ChooseProcessDialog.h"
#include "GetFullPathCmd.h"
#include "GetInitArgsCmd.h"
#include "RunProgramTask.h"

#include "MemoryDir.h"
#include "BreakpointManager.h"
#include "globals.h"
#include "fileVersions.h"
#include "actionDefs.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXDownRect.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXDockWidget.h>
#include <jx-af/jx/JXStringHistoryMenu.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>

#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kCmdHistoryLength = 100;

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Open source file...   %k Meta-O %i" kOpenSourceFileAction
	"%l| Load configuration... %k Ctrl-O %i" kLoadConfigAction
	"  | Save configuration... %k Ctrl-S %i" kSaveConfigAction
	"%l| Save history                    %i" kSaveHistoryAction
	"  | Save history as...              %i" kSaveHistoryAsAction
	"%l| Page setup...                   %i" kJXPageSetupAction
	"  | Print...              %k Meta-P %i" kJXPrintAction
	"%l| Close                 %k Meta-W %i" kJXCloseWindowAction
	"  | Quit                  %k Meta-Q %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kLoadConfigCmd,
	kSaveConfigCmd,
	kSaveCmd,
	kSaveAsCmd,
	kPageSetupCmd,
	kPrintCmd,
	kCloseCmd,
	kQuitCmd
};

// Debug menu

static const JUtf8Byte* kDebugMenuStr =
	"    Choose program...      %k Meta-Shift-P %i" kChooseBinaryAction
	"  | Reload program                         %i" kReloadBinaryAction
	"  | Choose core...         %k Meta-Shift-C %i" kChooseCoreAction
	"  | Choose process...                      %i" kChooseProcessAction
	"  | Set arguments                          %i" kSetArgumentsAction
	"%l| Restart debugger                       %i" kRestartDebuggerAction
	"%l| Display as variable    %k Meta-D       %i" kDisplayVariableAction
	"  | Display as 1D array    %k Meta-Shift-A %i" kDisplay1DArrayAction
	"  | Plot as 1D array                       %i" kPlot1DArrayAction
	"  | Display as 2D array                    %i" kDisplay2DArrayAction
	"%l| Watch expression                       %i" kWatchVarValueAction
	"  | Watch location                         %i" kWatchVarLocationAction
	"%l| Examine memory                         %i" kExamineMemoryAction
	"  | Disassemble memory                     %i" kDisasmMemoryAction
	"  | Disassemble function                   %i" kDisasmFunctionAction
	"%l| Run program            %k Meta-R       %i" kRunProgramAction
	"  | Pause execution        %k Ctrl-C       %i" kStopProgramAction
	"  | Kill process                           %i" kKillProgramAction
	"%l| Next statement         %k Meta-N       %i" kNextInstrAction
	"  | Step into              %k Meta-J       %i" kStepIntoAction
	"  | Finish subroutine      %k Meta-K       %i" kFinishSubAction
	"  | Continue execution     %k Meta-Shift-N %i" kContinueRunAction
	"%l| Next instruction       %k Meta-U       %i" kNextAsmInstrAction
	"  | Step into (instruction)%k Meta-I       %i" kStepIntoAsmAction
	"%l| Previous statement                     %i" kPrevInstrAction
	"  | Backup into                            %i" kReverseStepIntoAction
	"  | Reverse finish                         %i" kReverseFinishSubAction
	"  | Reverse continue                       %i" kReverseContinueRunAction
	"%l| Remove all breakpoints %k Ctrl-X       %i" kClearAllBreakpointsAction
	"%l";

enum
{
	kSelectBinCmd = 1,
	kReloadBinCmd,
	kSelectCoreCmd,
	kSelectProcessCmd,
	kSetArgsCmd,
	kRestartDebuggerCmd,
	kDisplayVarCmd,
	kDisplay1DArrayCmd,
	kPlot1DArrayCmd,
	kDisplay2DArrayCmd,
	kWatchVarCmd,
	kWatchLocCmd,
	kExamineMemCmd,
	kDisassembleMemCmd,
	kDisassembleFnCmd,
	kRunCmd,
	kStopCmd,
	kKillCmd,
	kNextCmd,
	kStepCmd,
	kFinishCmd,
	kContCmd,
	kNextAsmCmd,
	kStepAsmCmd,
	kPrevCmd,
	kReverseStepCmd,
	kReverseFinishCmd,
	kReverseContCmd,
	kRemoveAllBreakpointsCmd,

	kFirstCustomDebugCmd
};

// Prefs menu

static const JUtf8Byte* kPrefsMenuStr =
	"    gdb %r"
	"  | lldb %r"
	"  | Java %r"
	"  | Xdebug %r"
	"%l| Preferences..."
	"  | Toolbar buttons..."
	"  | Custom commands..."
	"  | Mac/Win/X emulation...";

enum
{
	kUseGDBCmd = 1,
	kUseLLDBCmd,
	kUseJavaCmd,
	kUseXdebugCmd,
	kEditPrefsCmd,
	kEditToolBarCmd,
	kEditCmdsCmd,
	kEditMacWinPrefsCmd
};

static const JIndex kDebuggerTypeToMenuIndex[] =
{
	kUseGDBCmd,
	kUseXdebugCmd,
	kUseJavaCmd,
	kUseLLDBCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

CommandDirector::CommandDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	JXMenuBar* menuBar = BuildWindow();

	itsCurrentSourceDir = jnew SourceDirector(this, SourceDirector::kMainSourceType);
	assert( itsCurrentSourceDir != nullptr );
	itsCurrentSourceDir->Activate();

	itsCurrentAsmDir = jnew SourceDirector(this, SourceDirector::kMainAsmType);
	assert( itsCurrentAsmDir != nullptr );

	itsThreadsDir = jnew ThreadsDir(this);
	assert( itsThreadsDir != nullptr );

	itsStackDir = jnew StackDir(this);
	assert(itsStackDir != nullptr);

	itsBreakpointsDir = jnew BreakpointsDir(this);
	assert(itsBreakpointsDir != nullptr);

	itsVarTreeDir = jnew VarTreeDir(this);
	assert(itsVarTreeDir != nullptr);

	itsLocalVarsDir = jnew LocalVarsDir(this);
	assert(itsLocalVarsDir != nullptr);

	itsRegistersDir = jnew RegistersDir(this);
	assert(itsRegistersDir != nullptr);

	itsFileListDir = jnew FileListDir(this);
	assert(itsFileListDir != nullptr);

	itsDebugDir = jnew DebugDir();
	assert(itsDebugDir!=nullptr);

	JXWDManager* wdMgr = GetDisplay()->GetWDManager();
	wdMgr->PermanentDirectorCreated(this,                JString::empty,                            kShowCommandLineAction);
	wdMgr->PermanentDirectorCreated(itsCurrentSourceDir, JString::empty,                            kShowCurrentSourceAction);
	wdMgr->PermanentDirectorCreated(itsThreadsDir,       JString("Meta-Shift-T", JString::kNoCopy), kShowThreadsAction);
	wdMgr->PermanentDirectorCreated(itsStackDir,         JString("Meta-Shift-S", JString::kNoCopy), kShowStackTraceAction);
	wdMgr->PermanentDirectorCreated(itsBreakpointsDir,   JString("Meta-Shift-B", JString::kNoCopy), kShowBreakpointsAction);
	wdMgr->PermanentDirectorCreated(itsVarTreeDir,       JString("Meta-Shift-V", JString::kNoCopy), kShowVariablesAction);
	wdMgr->PermanentDirectorCreated(itsLocalVarsDir,     JString("Meta-Shift-L", JString::kNoCopy), kShowLocalVariablesAction);
	wdMgr->PermanentDirectorCreated(itsCurrentAsmDir,    JString("Meta-Shift-Y", JString::kNoCopy), kShowCurrentAsmAction);
	wdMgr->PermanentDirectorCreated(itsRegistersDir,     JString("Meta-Shift-R", JString::kNoCopy), kShowRegistersAction);
	wdMgr->PermanentDirectorCreated(itsFileListDir,      JString("Meta-Shift-F", JString::kNoCopy), kShowFileListAction);
	wdMgr->PermanentDirectorCreated(itsDebugDir,         JString::empty,                            kShowDebugInfoAction);

	CreateWindowsMenuAndToolBar(menuBar, itsToolBar, false, false, true,
								itsDebugMenu, itsPrefsMenu, itsHelpMenu);
	itsCurrentSourceDir->CreateWindowsMenu();
	itsCurrentAsmDir->CreateWindowsMenu();

	itsSourceDirs = jnew JPtrArray<SourceDirector>(JPtrArrayT::kForgetAll);
	assert( itsSourceDirs != nullptr );

	itsAsmDirs = jnew JPtrArray<SourceDirector>(JPtrArrayT::kForgetAll);
	assert( itsAsmDirs != nullptr );

	itsArray1DDirs = jnew JPtrArray<Array1DDir>(JPtrArrayT::kForgetAll);
	assert( itsArray1DDirs != nullptr );

	itsArray2DDirs = jnew JPtrArray<Array2DDir>(JPtrArrayT::kForgetAll);
	assert( itsArray2DDirs != nullptr );

	itsPlot2DDirs = jnew JPtrArray<Plot2DDir>(JPtrArrayT::kForgetAll);
	assert( itsPlot2DDirs != nullptr );

	itsMemoryDirs = jnew JPtrArray<MemoryDir>(JPtrArrayT::kForgetAll);
	assert( itsMemoryDirs != nullptr );

	itsHistoryIndex     = 0;
	itsWaitingToRunFlag = false;

	InitializeCommandOutput();

	itsGetArgsCmd = itsLink->CreateGetInitArgsCmd(itsArgInput);

	ListenTo(GetPrefsManager());

	itsFakePrompt->GetText()->SetText(itsLink->GetCommandPrompt());
	itsFakePrompt->SetFontStyle(
		GetPrefsManager()->GetColor(PrefsManager::kRightMarginColorIndex));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CommandDirector::~CommandDirector()
{
	CloseDynamicDirectors();

	if (HasPrefsManager())
	{
		GetPrefsManager()->SaveWindowSize(kCmdWindSizeID, GetWindow());
	}

	jdelete itsSourceDirs;
	jdelete itsAsmDirs;
	jdelete itsArray1DDirs;
	jdelete itsArray2DDirs;
	jdelete itsPlot2DDirs;
	jdelete itsMemoryDirs;
	jdelete itsGetArgsCmd;
}

/******************************************************************************
 Close (virtual)

 ******************************************************************************/

bool
CommandDirector::Close()
{
	CloseDynamicDirectors();

	if (HasPrefsManager())
	{
		GetPrefsManager()->WriteHistoryMenuSetup(itsHistoryMenu);
	}
	return JXWindowDirector::Close();		// deletes us if successful
}

/******************************************************************************
 CloseDynamicDirectors

 ******************************************************************************/
#include <ranges>
void
CommandDirector::CloseDynamicDirectors()
{
	for (auto* dir : std::views::reverse(*itsSourceDirs))
	{
		dir->Close();
	}

	for (auto* dir : std::views::reverse(*itsAsmDirs))
	{
		dir->Close();
	}

	for (auto* dir : std::views::reverse(*itsArray1DDirs))
	{
		dir->Close();
	}

	for (auto* dir : std::views::reverse(*itsArray2DDirs))
	{
		dir->Close();
	}

	for (auto* dir : std::views::reverse(*itsPlot2DDirs))
	{
		dir->Close();
	}

	for (auto* dir : std::views::reverse(*itsMemoryDirs))
	{
		dir->Close();
	}
}

/******************************************************************************
 TransferKeyPressToInput

 ******************************************************************************/

void
CommandDirector::TransferKeyPressToInput
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsCommandInput->Focus())
	{
		itsCommandInput->HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 InitializeCommandOutput

 ******************************************************************************/

void
CommandDirector::InitializeCommandOutput()
{
	itsCommandOutput->GetText()->SetText(JString::empty);

	const JUtf8Byte* map[] =
	{
		"vers", GetVersionNumberStr().GetBytes()
	};
	const JString welcome = JGetString("Welcome::CommandDirector", map, sizeof(map));
	itsCommandOutput->GetText()->SetText(welcome);
	itsCommandOutput->SetCaretLocation(welcome.GetCharacterCount()+1);
}

/******************************************************************************
 PrepareCommand

 ******************************************************************************/

void
CommandDirector::PrepareCommand
	(
	const JString& cmd
	)
{
	itsCommandInput->GetText()->SetText(cmd);
	itsCommandInput->Focus();
	itsCommandInput->GoToEndOfLine();
	Activate();
	GetWindow()->RequestFocus();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_command_window.xpm"

#include <jx-af/image/jx/jx_file_open.xpm>
#include <jx-af/image/jx/jx_file_print.xpm>

JXMenuBar*
CommandDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,550, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kCmdWindowToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,440);
	assert( itsToolBar != nullptr );

	itsProgramButton =
		jnew JXTextButton(JGetString("itsProgramButton::CommandDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,530, 150,20);
	assert( itsProgramButton != nullptr );

	itsDownRect =
		jnew JXDownRect(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 0,470, 500,60);
	assert( itsDownRect != nullptr );

	itsFakePrompt =
		jnew JXStaticText(JGetString("itsFakePrompt::CommandDirector::JXLayout"), itsDownRect,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,0, 38,20);
	assert( itsFakePrompt != nullptr );
	itsFakePrompt->SetToLabel();

	itsCommandInput =
		jnew CommandInput(itsDownRect,
					JXWidget::kHElastic, JXWidget::kFixedTop, 53,0, 445,56);
	assert( itsCommandInput != nullptr );

	itsHistoryMenu =
		jnew JXStringHistoryMenu(kCmdHistoryLength, JString::empty, itsDownRect,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 30,20);
	assert( itsHistoryMenu != nullptr );

	itsArgInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 150,530, 350,20);
	assert( itsArgInput != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitleSuffix::CommandDirector"));
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetCommandWindowClass());
	window->SetMinSize(300, 200);
	GetPrefsManager()->GetWindowSize(kCmdWindSizeID, window);

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_command_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	itsDownRect->SetBackColor(itsCommandInput->GetCurrBackColor());
	itsFakePrompt->SetBackColor(itsCommandInput->GetCurrBackColor());
	TextDisplayBase::AdjustFont(itsFakePrompt);
	ListenTo(itsCommandInput);

	itsHistoryMenu->SetPopupArrowDirection(JXMenu::kArrowPointsUp);
	itsHistoryMenu->SetHistoryDirection(JXStringHistoryMenu::kNewestItemAtBottom);
	GetPrefsManager()->ReadHistoryMenuSetup(itsHistoryMenu);
	ListenTo(itsHistoryMenu);

	itsProgramButton->SetFontName(JFontManager::GetDefaultMonospaceFontName());
	itsProgramButton->SetFontSize(JFontManager::GetDefaultMonospaceFontSize());
	itsProgramButton->SetActive(itsLink->GetFeature(Link::kSetProgram));
	ListenTo(itsProgramButton);

	JPoint p = itsProgramButton->GetPadding();
	p.y      = 0;
	itsProgramButton->SetPaddingBeforeFTC(p);

	TextDisplayBase::AdjustFont(itsArgInput);
	itsArgInput->GetText()->SetCharacterInWordFunction(JXCSFDialogBase::IsCharacterInWord);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "CommandDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&CommandDirector::UpdateFileMenu,
		&CommandDirector::HandleFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd,  jx_file_open);
	itsFileMenu->SetItemImage(kPrintCmd, jx_file_print);

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	// appends Edit & Search menus

	itsCommandOutput =
		jnew CommandOutputDisplay(menuBar,
								  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
								  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( itsCommandOutput != nullptr );
	itsCommandOutput->FitToEnclosure(true, true);

	itsCommandOutput->AppendSearchMenu(menuBar);

	itsDebugMenu = CreateDebugMenu(menuBar);
	ListenTo(itsDebugMenu);

	itsCommandInput->ShareEditMenu(itsCommandOutput);
	itsArgInput->ShareEditMenu(itsCommandOutput);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "CommandDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandlers(this,
		&CommandDirector::UpdatePrefsMenu,
		&CommandDirector::HandlePrefsMenu);

	itsHelpMenu = GetApplication()->CreateHelpMenu(menuBar, "CommandDirector", "CommandDirHelp");

	return menuBar;
}

/******************************************************************************
 DockAll

 ******************************************************************************/

void
CommandDirector::DockAll
	(
	JXDockWidget* srcDock,
	JXDockWidget* infoDock,
	JXDockWidget* dataDock
	)
{
	srcDock->Dock(itsCurrentSourceDir);
	srcDock->Dock(itsCurrentAsmDir);

	JSize count = itsSourceDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		srcDock->Dock(itsSourceDirs->GetElement(i));
	}

	count = itsAsmDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		srcDock->Dock(itsAsmDirs->GetElement(i));
	}

	infoDock->Dock(itsThreadsDir);
	infoDock->Dock(itsStackDir);
	infoDock->Dock(itsBreakpointsDir);
	infoDock->Dock(itsFileListDir);

	dataDock->Dock(itsVarTreeDir);
	dataDock->Dock(itsLocalVarsDir);
	dataDock->Dock(itsRegistersDir);

	count = itsArray1DDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		dataDock->Dock(itsArray1DDirs->GetElement(i));
	}

	count = itsArray2DDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		dataDock->Dock(itsArray2DDirs->GetElement(i));
	}

	count = itsPlot2DDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		dataDock->Dock(itsPlot2DDirs->GetElement(i));
	}

	count = itsMemoryDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		dataDock->Dock(itsMemoryDirs->GetElement(i));
	}
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
CommandDirector::UpdateWindowTitle
	(
	const JString& programName
	)
{
	JString title = programName;
	title += JGetString("WindowTitleSuffix::CommandDirector");
	GetWindow()->SetTitle(title);

	if (!programName.IsEmpty())
	{
		itsProgramButton->SetLabel(programName);
	}
	else
	{
		itsProgramButton->SetLabel(JGetString("NoBinaryButtonLabel::CommandDirector"));
	}
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
CommandDirector::GetName()
	const
{
	return JGetString("WindowsMenuText::CommandDirector");
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
CommandDirector::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetCommandLineIcon();
	return true;
}

/******************************************************************************
 CreateDebugMenu

 ******************************************************************************/

#include <jx-af/image/jx/jx_executable_small.xpm>
#include "medic_choose_core.xpm"
#include "medic_attach_process.xpm"
#include "medic_show_1d_array.xpm"
#include "medic_show_2d_plot.xpm"
#include "medic_show_2d_array.xpm"
#include "medic_show_memory.xpm"
#include "medic_run_program.xpm"
#include "medic_stop_program.xpm"
#include "medic_kill_program.xpm"
#include "medic_next.xpm"
#include "medic_step.xpm"
#include "medic_finish.xpm"
#include "medic_continue.xpm"
#include "medic_nexti.xpm"
#include "medic_stepi.xpm"
#include "medic_reverse_next.xpm"
#include "medic_reverse_step.xpm"
#include "medic_reverse_finish.xpm"
#include "medic_reverse_continue.xpm"

JXTextMenu*
CommandDirector::CreateDebugMenu
	(
	JXMenuBar* menuBar
	)
{
	JXTextMenu* menu = menuBar->AppendTextMenu(JGetString("DebugMenuTitle::CommandDirector"));
	menu->SetMenuItems(kDebugMenuStr, "CommandDirector");

	menu->SetItemImage(kSelectBinCmd,      jx_executable_small);
	menu->SetItemImage(kSelectCoreCmd,     medic_choose_core);
	menu->SetItemImage(kSelectProcessCmd,  medic_attach_process);
	menu->SetItemImage(kDisplay1DArrayCmd, medic_show_1d_array);
	menu->SetItemImage(kPlot1DArrayCmd,    medic_show_2d_plot);
	menu->SetItemImage(kDisplay2DArrayCmd, medic_show_2d_array);
	menu->SetItemImage(kExamineMemCmd,     medic_show_memory);
	menu->SetItemImage(kRunCmd,            medic_run_program);
	menu->SetItemImage(kStopCmd,           medic_stop_program);
	menu->SetItemImage(kKillCmd,           medic_kill_program);
	menu->SetItemImage(kNextCmd,           medic_next);
	menu->SetItemImage(kStepCmd,           medic_step);
	menu->SetItemImage(kFinishCmd,         medic_finish);
	menu->SetItemImage(kContCmd,           medic_continue);
	menu->SetItemImage(kNextAsmCmd,        medic_nexti);
	menu->SetItemImage(kStepAsmCmd,        medic_stepi);
	menu->SetItemImage(kPrevCmd,           medic_reverse_next);
	menu->SetItemImage(kReverseStepCmd,    medic_reverse_step);
	menu->SetItemImage(kReverseFinishCmd,  medic_reverse_finish);
	menu->SetItemImage(kReverseContCmd,    medic_reverse_continue);

	// Meta-. should also be "stop"

	JXKeyModifiers m(menu->GetDisplay());
	m.SetState(kJXMetaKeyIndex, true);
	menu->GetWindow()->InstallMenuShortcut(menu, kStopCmd, '.', m);

	AdjustDebugMenu(menu);
	return menu;
}

/******************************************************************************
 AddDebugMenuItemsToToolBar (static)

 ******************************************************************************/

void
CommandDirector::AddDebugMenuItemsToToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu*	debugMenu,
	const bool	includeStepAsm
	)
{
	toolBar->AppendButton(debugMenu, kRunCmd);
	toolBar->AppendButton(debugMenu, kStopCmd);
	toolBar->AppendButton(debugMenu, kKillCmd);
	toolBar->NewGroup();
	toolBar->AppendButton(debugMenu, kNextCmd);
	toolBar->AppendButton(debugMenu, kStepCmd);
	toolBar->AppendButton(debugMenu, kFinishCmd);
	toolBar->AppendButton(debugMenu, kContCmd);

	if (includeStepAsm)
	{
		toolBar->NewGroup();
		toolBar->AppendButton(debugMenu, kNextAsmCmd);
		toolBar->AppendButton(debugMenu, kStepAsmCmd);
	}
}

/******************************************************************************
 AdjustDebugMenu

 ******************************************************************************/

void
CommandDirector::AdjustDebugMenu
	(
	JXTextMenu* menu
	)
{
	const JSize itemCount = menu->GetItemCount();
	for (JIndex i=itemCount; i>=kFirstCustomDebugCmd; i--)
	{
		menu->RemoveItem(i);
	}

	JPtrArray<JString> cmds(JPtrArrayT::kDeleteAll);
	GetPrefsManager()->GetCmdList(&cmds);

	for (const auto* cmd : cmds)
	{
		menu->AppendItem(*cmd, JXMenu::kPlainType, JString::empty, JString::empty, *cmd);
	}
}

/******************************************************************************
 CreateWindowsMenuAndToolBar (static)

 ******************************************************************************/

void
CommandDirector::CreateWindowsMenuAndToolBar
	(
	JXMenuBar*		menuBar,
	JXToolBar*		toolBar,
	const bool		includeStepAsm,
	const bool		includeCmdLine,
	const bool		includeCurrSrc,
	JXTextMenu*		debugMenu,
	JXTextMenu*		prefsMenu,
	JXTextMenu*		helpMenu
	)
{
	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->InsertMenuBefore(prefsMenu, wdMenu);

	toolBar->LoadPrefs();
	if (toolBar->IsEmpty())
	{
		AddDebugMenuItemsToToolBar(toolBar, debugMenu, includeStepAsm);
		toolBar->NewGroup();
		AddWindowsMenuItemsToToolBar(toolBar, wdMenu, includeCmdLine, includeCurrSrc);
		GetApplication()->AppendHelpMenuToToolBar(toolBar, helpMenu);
	}
}

/******************************************************************************
 AddWindowsMenuItemsToToolBar (static)

 ******************************************************************************/

void
CommandDirector::AddWindowsMenuItemsToToolBar
	(
	JXToolBar*	toolBar,
	JXTextMenu*	windowsMenu,
	const bool	includeCmdLine,
	const bool	includeCurrSrc
	)
{
	if (includeCmdLine)
	{
		toolBar->AppendButton(windowsMenu, JString(kShowCommandLineAction, JString::kNoCopy));
	}
	if (includeCurrSrc)
	{
		toolBar->AppendButton(windowsMenu, JString(kShowCurrentSourceAction, JString::kNoCopy));
	}
	toolBar->AppendButton(windowsMenu, JString(kShowStackTraceAction, JString::kNoCopy));
	toolBar->AppendButton(windowsMenu, JString(kShowBreakpointsAction, JString::kNoCopy));
	toolBar->AppendButton(windowsMenu, JString(kShowVariablesAction, JString::kNoCopy));
	toolBar->AppendButton(windowsMenu, JString(kShowLocalVariablesAction, JString::kNoCopy));
	toolBar->AppendButton(windowsMenu, JString(kShowFileListAction, JString::kNoCopy));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CommandDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// Link

	if (sender == itsLink && message.Is(Link::kUserOutput))
	{
		const auto* output =
			dynamic_cast<const Link::UserOutput*>(&message);
		assert(output != nullptr);
		itsCommandOutput->SetCaretLocation(
			itsCommandOutput->GetText()->GetText().GetCharacterCount()+1);

		JFont font = itsCommandOutput->GetText()->GetDefaultFont();
		font.SetBold(output->IsFromTarget());
		font.SetColor(output->IsError() ? JColorManager::GetDarkRedColor() : JColorManager::GetBlackColor());
		itsCommandOutput->SetCurrentFont(font);
		itsCommandOutput->Paste(output->GetText());
		if (output->GetText().GetLastCharacter() != '\n')
		{
			itsCommandOutput->Paste(JString::newline);
		}
		itsCommandOutput->GetText()->ClearUndo();
	}

	else if (sender == itsLink && message.Is(Link::kDebuggerBusy))
	{
		itsFakePrompt->SetFontStyle(
			GetPrefsManager()->GetColor(PrefsManager::kRightMarginColorIndex));
	}
	else if (sender == itsLink && message.Is(Link::kDebuggerReadyForInput))
	{
		itsFakePrompt->GetText()->SetText(itsLink->GetCommandPrompt());
		itsFakePrompt->SetFontStyle(
			GetPrefsManager()->GetColor(PrefsManager::kTextColorIndex));
	}
	else if (sender == itsLink && message.Is(Link::kDebuggerDefiningScript))
	{
		itsFakePrompt->GetText()->SetText(itsLink->GetScriptPrompt());
		itsFakePrompt->SetFontStyle(
			GetPrefsManager()->GetColor(PrefsManager::kTextColorIndex));
		// can't do it here because, when core file loaded, hook-run is
		// defined after stack window is opened
//		Activate();
//		GetWindow()->RequestFocus();
	}

	else if (sender == itsLink && message.Is(Link::kProgramFinished))
	{
		GetDisplay()->Beep();
	}

	else if (sender == itsLink && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());

		if (itsWaitingToRunFlag)
		{
			auto* task = jnew RunProgramTask();
			assert( task != nullptr );
			task->Start();
			itsWaitingToRunFlag = false;
		}
	}

	else if (sender == itsLink && message.Is(Link::kDebuggerStarted))
	{
		if (itsGetArgsCmd != nullptr)
		{
			itsGetArgsCmd->Send();
			itsGetArgsCmd = nullptr;	// one shot
		}
	}

	else if (sender == itsLink && message.Is(Link::kProgramStopped))
	{
		itsCommandInput->Focus();

		bool active = false;
		if (itsCurrentSourceDir->IsActive())
		{
			(itsCurrentSourceDir->GetWindow())->Raise(false);
			active = true;
		}
		if (itsCurrentAsmDir->IsActive() &&
			(!itsCurrentAsmDir->GetWindow()->IsDocked() ||
			 !itsCurrentSourceDir->IsActive()))
		{
			(itsCurrentAsmDir->GetWindow())->Raise(false);
			active = true;
		}
		if (!active)
		{
			itsCurrentSourceDir->Activate();
		}
	}

	// CommandInput

	else if (sender == itsCommandInput && message.Is(CommandInput::kReturnKeyPressed))
	{
		HandleUserInput();
	}
	else if (sender == itsCommandInput && message.Is(CommandInput::kTabKeyPressed))
	{
		HandleCompletionRequest();
	}
	else if (sender == itsCommandInput &&
			 (message.Is(JXWidget::kGotFocus) ||
			  message.Is(JXWidget::kLostFocus)))
	{
		itsDownRect->SetBackColor(itsCommandInput->GetCurrBackColor());
		itsFakePrompt->SetBackColor(itsCommandInput->GetCurrBackColor());
	}
	else if (sender == itsCommandInput && message.Is(CommandInput::kUpArrowKeyPressed))
	{
		ShowHistoryCommand(+1);
	}
	else if (sender == itsCommandInput && message.Is(CommandInput::kDownArrowKeyPressed))
	{
		ShowHistoryCommand(-1);
	}

	// menus

	else if (sender == itsDebugMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateDebugMenu(itsDebugMenu, itsCommandOutput, itsCommandInput);
	}
	else if (sender == itsDebugMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleDebugMenu(itsDebugMenu, selection->GetIndex(), itsCommandOutput, itsCommandInput);
	}

	else if (sender == GetPrefsManager() && message.Is(PrefsManager::kCustomCommandsChanged))
	{
		AdjustDebugMenu(itsDebugMenu);
	}

	else if (sender == itsHistoryMenu && message.Is(JXMenu::kItemSelected))
	{
		const JString& str = itsHistoryMenu->GetItemText(message);
		itsCommandInput->GetText()->SetText(str);
		itsCommandInput->GoToEndOfLine();
	}

	// misc

	else if (sender == itsProgramButton && message.Is(JXButton::kPushed))
	{
		ChangeProgram();
	}

	else if (message.Is(GetFullPathCmd::kFileFound))
	{
		const auto* info =
			dynamic_cast<const GetFullPathCmd::FileFound*>(&message);
		assert( info != nullptr );
		OpenSourceFile(info->GetFullName(), info->GetLineIndex());
	}
	else if (message.Is(GetFullPathCmd::kFileNotFound))
	{
		const auto* info =
			dynamic_cast<const GetFullPathCmd::FileNotFound*>(&message);
		assert( info != nullptr );
		ReportUnreadableSourceFile(info->GetFileName());
	}
	else if (message.Is(GetFullPathCmd::kNewCommand))
	{
		const auto* info =
			dynamic_cast<const GetFullPathCmd::NewCommand*>(&message);
		assert( info != nullptr );
		ListenTo(info->GetNewCommand());
	}

	else
	{
		JXWindowDirector::Receive(sender,message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CommandDirector::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		itsLink = GetLink();
		ListenTo(itsLink);

		itsWaitingToRunFlag = false;

		UpdateWindowTitle(JString::empty);
		itsFakePrompt->GetText()->SetText(itsLink->GetCommandPrompt());
		itsFakePrompt->SetFontStyle(
			GetPrefsManager()->GetColor(PrefsManager::kRightMarginColorIndex));

		itsProgramButton->SetActive(itsLink->GetFeature(Link::kSetProgram));
		itsArgInput->GetText()->SetText(JString::empty);

		jdelete itsGetArgsCmd;
		itsGetArgsCmd = itsLink->CreateGetInitArgsCmd(itsArgInput);
	}
	else
	{
		JXWindowDirector::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 HandleUserInput (private)

 ******************************************************************************/

void
CommandDirector::HandleUserInput()
{
	itsHistoryIndex = 0;
	JString input   = itsCommandInput->GetText()->GetText();

	if ((!itsLink->IsDebugging() || itsLink->ProgramIsStopped()) &&
		!itsLink->IsDefiningScript())
	{
		itsCommandOutput->PlaceCursorAtEnd();
		itsCommandOutput->SetCurrentFontStyle(itsCommandOutput->GetText()->GetDefaultFont().GetStyle());
		itsCommandOutput->Paste("\n" + itsLink->GetCommandPrompt() + " ");
	}
	else
	{
		itsCommandOutput->GoToEndOfLine();
	}
	itsCommandOutput->SetCurrentFontStyle(itsCommandOutput->GetText()->GetDefaultFont().GetStyle());
	itsCommandOutput->Paste(input);

	itsCommandOutput->Paste(JString::newline);
	input.TrimWhitespace();
	if (!input.IsEmpty())
	{
		itsHistoryMenu->AddString(input);
	}
	itsLink->SendRaw(input);

	itsCommandInput->GetText()->SetText(JString::empty);
}

/******************************************************************************
 HandleCompletionRequest (private)

 ******************************************************************************/

void
CommandDirector::HandleCompletionRequest()
{
	const JString& input = itsCommandInput->GetText()->GetText();
	if (!input.EndsWith(" ") &&
		!input.EndsWith("\t") &&
		!input.EndsWith(":"))
	{
		GetCompletionsCmd* cmd =
			itsLink->CreateGetCompletionsCmd(itsCommandInput, itsCommandOutput);
		cmd->Send();
	}
	else
	{
		GetDisplay()->Beep();
	}
}

/******************************************************************************
 ShowHistoryCommand (private)

 ******************************************************************************/

void
CommandDirector::ShowHistoryCommand
	(
	const JInteger delta
	)
{
	if (itsHistoryIndex == 0)
	{
		itsCurrentCommand = itsCommandInput->GetText()->GetText();
	}

	const JIndex menuIndex = itsHistoryMenu->GetItemCount() - itsHistoryIndex + 1;

	const JInteger i = JInteger(menuIndex) - delta;
	if (0 < i && i <= (JInteger) itsHistoryMenu->GetItemCount())
	{
		itsHistoryIndex     += delta;
		const JString& input = itsHistoryMenu->GetItemText(i);
		itsCommandInput->GetText()->SetText(input);
	}
	else if (i > (JInteger) itsHistoryMenu->GetItemCount())
	{
		itsHistoryIndex = 0;
		itsCommandInput->GetText()->SetText(itsCurrentCommand);
	}

	itsCommandInput->GoToEndOfLine();
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
CommandDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kCloseCmd, !GetWindow()->IsDocked());
	itsFileMenu->SetItemEnabled(kLoadConfigCmd, itsLink->HasProgram());
}

/******************************************************************************
 HandleFileMenu

 ******************************************************************************/

void
CommandDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
	{
		OpenSourceFiles();
	}

	else if (index == kLoadConfigCmd)
	{
		LoadConfig();
	}
	else if (index == kSaveConfigCmd)
	{
		SaveConfig();
	}

	else if (index == kSaveCmd)
	{
		SaveInCurrentFile();
	}
	else if (index == kSaveAsCmd)
	{
		SaveInNewFile();
	}

	else if (index == kPageSetupCmd)
	{
		itsCommandOutput->HandlePTPageSetup();
	}
	else if (index == kPrintCmd)
	{
		itsCommandOutput->PrintPT();
	}

	else if (index == kCloseCmd)
	{
		Deactivate();
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 OpenSourceFiles

 ******************************************************************************/

void
CommandDirector::OpenSourceFiles()
{
	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectMultipleFiles);
	if (dlog->DoDialog())
	{
		JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
		dlog->GetFullNames(&list);

		for (auto* s : list)
		{
			OpenSourceFile(*s);
		}
	}
}

/******************************************************************************
 OpenSourceFile

 ******************************************************************************/

void
CommandDirector::OpenSourceFile
	(
	const JString&	fileName,
	const JSize		lineIndex,
	const bool		askDebuggerWhenRelPath
	)
{
	JString fullName;
	if (JIsRelativePath(fileName) && askDebuggerWhenRelPath)
	{
		bool exists;
		JString fullName;
		const bool known = itsLink->FindFile(fileName, &exists, &fullName);
		if (known && exists)
		{
			OpenSourceFile(fullName, lineIndex);
		}
		else if (known)
		{
			ReportUnreadableSourceFile(fileName);
		}
		else
		{
			GetFullPathCmd* cmd = itsLink->CreateGetFullPathCmd(fileName, lineIndex);
			ListenTo(cmd);
		}
	}
	else if (JConvertToAbsolutePath(fileName, JString::empty, &fullName))
	{
		const JSize count = itsSourceDirs->GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			SourceDirector* dir = itsSourceDirs->GetElement(i);
			const JString* f;
			if (dir->GetFileName(&f) && JSameDirEntry(fullName, *f))
			{
				dir->Activate();
				if (lineIndex > 0)
				{
					dir->DisplayLine(lineIndex);
				}
				return;
			}
		}

		SourceDirector* dir = SourceDirector::Create(this, fullName, SourceDirector::kSourceType);
		itsSourceDirs->Append(dir);
		dir->Activate();
		if (lineIndex > 0)
		{
			dir->DisplayLine(lineIndex);
		}
	}
	else
	{
		ReportUnreadableSourceFile(fileName);
	}
}

/******************************************************************************
 DisassembleFunction

 ******************************************************************************/

void
CommandDirector::DisassembleFunction
	(
	const JString& origFn,
	const JString& addr
	)
{
	SourceDirector* dir = nullptr;

	JString fn = origFn;
	JStringIterator iter(&fn);
	if (iter.Next("("))
	{
		iter.SkipPrev();
		iter.RemoveAllNext();
	}
	iter.Invalidate();

	const JSize count = itsAsmDirs->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		dir = itsAsmDirs->GetElement(i);
		const JString* f;
		if (dir->GetFunctionName(&f) && fn == *f)
		{
			dir->Activate();
			break;
		}
		dir = nullptr;		// trigger Create()
	}

	if (dir == nullptr)
	{
		dir = SourceDirector::Create(this, addr.IsEmpty() ? fn : JString::empty, SourceDirector::kAsmType);
		itsAsmDirs->Append(dir);
		dir->Activate();
	}

	if (!addr.IsEmpty())
	{
		Location loc;
		loc.SetFunctionName(fn);
		loc.SetMemoryAddress(addr);
		dir->DisplayDisassembly(loc);
	}
}

/******************************************************************************
 ReportUnreadableSourceFile

 ******************************************************************************/

void
CommandDirector::ReportUnreadableSourceFile
	(
	const JString& fileName
	)
	const
{
	const JUtf8Byte* map[] =
	{
		"name", fileName.GetBytes()
	};
	const JString err = JGetString("CannotOpenFile::CommandDirector", map, sizeof(map));
	JGetUserNotification()->ReportError(err);
}

/******************************************************************************
 LoadConfig

 ******************************************************************************/

void
CommandDirector::LoadConfig()
{
	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile);
	if (dlog->DoDialog())
	{
		const JString fullName = dlog->GetFullName();

		std::ifstream input(fullName.GetBytes());

		JFileVersion vers;
		input >> vers;
		if (vers <= kCurrentConfigVersion)
		{
			(itsLink->GetBreakpointManager())->ReadSetup(input, vers);
			itsVarTreeDir->ReadSetup(input, vers);

			JSize count;
			input >> count;

			for (JIndex i=1; i<=count; i++)
			{
				auto* dir = jnew Array1DDir(input, vers, this);
				assert( dir != nullptr );
				// adds itself to list automatically
				dir->Activate();
			}

			input >> count;

			for (JIndex i=1; i<=count; i++)
			{
				auto* dir = jnew Array2DDir(input, vers, this);
				assert( dir != nullptr );
				// adds itself to list automatically
				dir->Activate();
			}

			input >> count;

			for (JIndex i=1; i<=count; i++)
			{
				auto* dir = jnew Plot2DDir(input, vers, this);
				assert( dir != nullptr );
				// adds itself to list automatically
				dir->Activate();
			}

			if (vers >= 3)
			{
				input >> count;

				for (JIndex i=1; i<=count; i++)
				{
					auto* dir = jnew MemoryDir(input, vers, this);
					assert( dir != nullptr );
					// adds itself to list automatically
					dir->Activate();
				}
			}
		}
		else
		{
			JGetUserNotification()->DisplayMessage(JGetString("ConfigTooNew::CommandDirector"));
		}
	}
}

/******************************************************************************
 SaveConfig

 ******************************************************************************/

void
CommandDirector::SaveConfig()
{
	JString fullName, path, origName;
	if (itsLink->GetProgram(&fullName))
	{
		JSplitPathAndName(fullName, &path, &origName);
		origName += "_debug_config";
	}
	else
	{
		origName = "debug_config";
	}

	auto* dlog = JXSaveFileDialog::Create(JGetString("SaveFilePrompt::CommandDirector"));
	if (dlog->DoDialog())
	{
		fullName = dlog->GetFullName();
		std::ofstream output(fullName.GetBytes());

		output << kCurrentConfigVersion;
		itsLink->GetBreakpointManager()->WriteSetup(output);
		itsVarTreeDir->WriteSetup(output);

		JSize count = itsArray1DDirs->GetElementCount();
		output << ' ' << count;

		for (auto* d: *itsArray1DDirs)
		{
			d->StreamOut(output);
		}

		count = itsArray2DDirs->GetElementCount();
		output << ' ' << count;

		for (auto* d: *itsArray2DDirs)
		{
			d->StreamOut(output);
		}

		count = itsPlot2DDirs->GetElementCount();
		output << ' ' << count;

		for (auto* d: *itsPlot2DDirs)
		{
			d->StreamOut(output);
		}

		count = itsMemoryDirs->GetElementCount();
		output << ' ' << count;

		for (auto* d: *itsMemoryDirs)
		{
			d->StreamOut(output);
		}
	}
}

/******************************************************************************
 SaveInCurrentFile

 ******************************************************************************/

void
CommandDirector::SaveInCurrentFile()
{
	if (itsCurrentHistoryFile.IsEmpty())
	{
		SaveInNewFile();
	}
	else
	{
		itsCommandOutput->GetText()->WritePlainText(itsCurrentHistoryFile, JStyledText::kUNIXText);
	}
}

/******************************************************************************
 SaveInNewFile

 ******************************************************************************/

void
CommandDirector::SaveInNewFile()
{
	auto* dlog = JXSaveFileDialog::Create(JGetString("SaveFilePrompt::CommandDirector"));
	if (dlog->DoDialog())
	{
		itsCurrentHistoryFile = dlog->GetFullName();
		SaveInCurrentFile();
	}
}

/******************************************************************************
 UpdateDebugMenu

	te1 and te2 can both be nullptr.  If not, they are used to look for
	selected text to send to the Variable Tree window.

	Shared with SourceDirector

 ******************************************************************************/

void
CommandDirector::UpdateDebugMenu
	(
	JXTextMenu*	menu,
	JXTEBase*	te1,
	JXTEBase*	te2
	)
{
	if (itsLink->GetFeature(Link::kSetArgs))
	{
		menu->EnableItem(kSetArgsCmd);
	}

	if (itsLink->DebuggerHasStarted())
	{
		const bool canSetProgram = itsLink->GetFeature(Link::kSetProgram);
		if (canSetProgram)
		{
			menu->EnableItem(kSelectBinCmd);
		}

		if (itsLink->GetFeature(Link::kSetCore))
		{
			menu->EnableItem(kSelectCoreCmd);
		}
		if (itsLink->GetFeature(Link::kSetProcess))
		{
			menu->EnableItem(kSelectProcessCmd);
		}

		menu->EnableItem(kRestartDebuggerCmd);
		if (itsLink->GetFeature(Link::kRunProgram))
		{
			menu->EnableItem(kRunCmd);
		}

		const JSize count = menu->GetItemCount();
		for (JIndex i=kFirstCustomDebugCmd; i<=count; i++)
		{
			menu->EnableItem(i);
		}

		menu->EnableItem(kRemoveAllBreakpointsCmd);
		if (itsLink->HasProgram())
		{
			if (canSetProgram)
			{
				menu->EnableItem(kReloadBinCmd);
			}
			if (itsLink->GetFeature(Link::kExamineMemory))
			{
				menu->EnableItem(kExamineMemCmd);
			}
			if (itsLink->GetFeature(Link::kDisassembleMemory))
			{
				menu->EnableItem(kDisassembleMemCmd);
			}

			if (itsLink->HasLoadedSymbols())
			{
				JString text;
				if (((te1 != nullptr && te1->GetSelection(&text)) ||
					 (te2 != nullptr && te2->GetSelection(&text))) &&
					 !text.Contains("\n"))
				{
					menu->EnableItem(kDisplayVarCmd);
					menu->EnableItem(kDisplay1DArrayCmd);
					menu->EnableItem(kPlot1DArrayCmd);
					menu->EnableItem(kDisplay2DArrayCmd);
					menu->EnableItem(kWatchVarCmd);
					menu->EnableItem(kWatchLocCmd);

					if (itsLink->GetFeature(Link::kDisassembleMemory))
					{
						menu->EnableItem(kDisassembleFnCmd);
					}
				}

				if (itsLink->IsDebugging())
				{
					if (itsLink->GetFeature(Link::kStopProgram))
					{
						menu->EnableItem(kKillCmd);
					}

					if (itsLink->ProgramIsStopped())
					{
						menu->EnableItem(kNextCmd);
						menu->EnableItem(kStepCmd);
						menu->EnableItem(kFinishCmd);
						menu->EnableItem(kContCmd);

						if (itsLink->GetFeature(Link::kDisassembleMemory))
						{
							menu->EnableItem(kNextAsmCmd);
							menu->EnableItem(kStepAsmCmd);
						}

						const bool bkwd = itsLink->GetFeature(Link::kExecuteBackwards);
						menu->SetItemEnabled(kPrevCmd, bkwd);
						menu->SetItemEnabled(kReverseStepCmd, bkwd);
						menu->SetItemEnabled(kReverseFinishCmd, bkwd);
						menu->SetItemEnabled(kReverseContCmd, bkwd);
					}
					else if (itsLink->GetFeature(Link::kStopProgram))
					{
						menu->EnableItem(kStopCmd);
					}
				}
			}
		}
	}
}

/******************************************************************************
 HandleDebugMenu

	te1 and te2 can both be nullptr.  If not, they are used to look for
	selected text to send to the Variable Tree window.

	Shared with SourceDirector

 ******************************************************************************/

void
CommandDirector::HandleDebugMenu
	(
	JXTextMenu*		menu,
	const JIndex	index,
	JXTEBase*		te1,
	JXTEBase*		te2
	)
{
	if (index == kSelectBinCmd)
	{
		ChangeProgram();
	}
	else if (index == kReloadBinCmd)
	{
		ReloadProgram();
	}
	else if (index == kSelectCoreCmd)
	{
		ChooseCoreFile();
	}
	else if (index == kSelectProcessCmd)
	{
		ChooseProcess();
	}
	else if (index == kSetArgsCmd)
	{
		Activate();
		GetWindow()->RequestFocus();
		if (itsArgInput->Focus())
		{
			itsArgInput->SelectAll();
		}
	}

	else if (index == kRestartDebuggerCmd)
	{
		itsLink->RestartDebugger();
	}

	else if (index == kDisplayVarCmd)
	{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
		{
			DisplayExpression(text);
		}
	}
	else if (index == kDisplay1DArrayCmd)
	{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
		{
			Display1DArray(text);
		}
	}
	else if (index == kPlot1DArrayCmd)
	{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
		{
			Plot1DArray(text);
		}
	}
	else if (index == kDisplay2DArrayCmd)
	{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
		{
			Display2DArray(text);
		}
	}

	else if (index == kWatchVarCmd)
	{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
		{
			GetLink()->WatchExpression(text);
		}
	}
	else if (index == kWatchLocCmd)
	{
		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
		{
			GetLink()->WatchLocation(text);
		}
	}

	else if (index == kExamineMemCmd)
	{
		JString s1, s2;
		if ((te1 != nullptr && te1->GetSelection(&s1) && !s1.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&s1) && !s1.Contains("\n")))
		{
			s2 = s1;
		}

		auto* dir = jnew MemoryDir(this, s2);
		assert(dir != nullptr);
		dir->Activate();
	}
	else if (index == kDisassembleMemCmd)
	{
		JString s1, s2;
		if ((te1 != nullptr && te1->GetSelection(&s1) && !s1.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&s1) && !s1.Contains("\n")))
		{
			s2 = s1;
		}

		auto* dir = jnew MemoryDir(this, s2);
		assert(dir != nullptr);
		dir->SetDisplayType(MemoryDir::kAsm);
		dir->Activate();
	}
	else if (index == kDisassembleFnCmd)
	{
		JString fn;
		if ((te1 != nullptr && te1->GetSelection(&fn) && !fn.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&fn) && !fn.Contains("\n")))
		{
			DisassembleFunction(fn);
		}
	}

	else if (index == kRunCmd)
	{
		RunProgram();
	}
	else if (index == kStopCmd)
	{
		itsLink->StopProgram();
	}
	else if (index == kKillCmd)
	{
		itsLink->KillProgram();
	}

	else if (index == kNextCmd)
	{
		itsLink->StepOver();
	}
	else if (index == kStepCmd)
	{
		itsLink->StepInto();
	}
	else if (index == kFinishCmd)
	{
		itsLink->StepOut();
	}
	else if (index == kContCmd)
	{
		itsLink->Continue();
	}

	else if (index == kNextAsmCmd)
	{
		itsLink->StepOverAssembly();
	}
	else if (index == kStepAsmCmd)
	{
		itsLink->StepIntoAssembly();
	}

	else if (index == kPrevCmd)
	{
		itsLink->BackupOver();
	}
	else if (index == kReverseStepCmd)
	{
		itsLink->BackupInto();
	}
	else if (index == kReverseFinishCmd)
	{
		itsLink->BackupOut();
	}
	else if (index == kReverseContCmd)
	{
		itsLink->BackupContinue();
	}

	else if (index == kRemoveAllBreakpointsCmd)
	{
		itsLink->RemoveAllBreakpoints();
	}

	else if (index >= kFirstCustomDebugCmd)
	{
		JString s = menu->GetItemText(index);

		JString text;
		if ((te1 != nullptr && te1->GetSelection(&text) && !text.Contains("\n")) ||
			(te2 != nullptr && te2->GetSelection(&text) && !text.Contains("\n")))
		{
			JStringIterator iter(&s);
			while (iter.Next("##"))
			{
				iter.ReplaceLastMatch(text);
			}
		}
		else if (s.Contains("##"))
		{
			JGetUserNotification()->ReportError(JGetString("NoSelection::CommandDirector"));
			return;
		}

		itsLink->SendRaw(s);
	}
}

/******************************************************************************
 RunProgram

 ******************************************************************************/

void
CommandDirector::RunProgram()
{
	if (!itsLink->HasProgram())
	{
		ChangeProgram();
		if (itsLink->HasProgram())
		{
			itsWaitingToRunFlag = true;
		}
	}
	else if (GetLink()->HasPendingCommands())
	{
		auto* task = jnew RunProgramTask();
		assert( task != nullptr );
		task->Start();
	}
	else if (itsLink->OKToDetachOrKill())
	{
		itsLink->RunProgram(itsArgInput->GetText()->GetText());
	}
}

/******************************************************************************
 ChangeProgram

 ******************************************************************************/

void
CommandDirector::ChangeProgram()
{
	if (itsLink->OKToDetachOrKill())
	{
		auto* dlog = JXChooseFileDialog::Create(
			JXChooseFileDialog::kSelectSingleFile, JString::empty, JString::empty,
			itsLink->GetChooseProgramInstructions());
		if (dlog->DoDialog())
		{
			const JString fullName = dlog->GetFullName();
			MDIServer::UpdateDebuggerType(fullName);
			itsLink->SetProgram(fullName);
		}
	}
}

/******************************************************************************
 ReloadProgram

 ******************************************************************************/

void
CommandDirector::ReloadProgram()
{
	if (itsLink->OKToDetachOrKill())
	{
		itsLink->ReloadProgram();
	}
}

/******************************************************************************
 ChooseCoreFile

 ******************************************************************************/

void
CommandDirector::ChooseCoreFile()
{
	JString origPath;

#ifdef _J_MACOS

	origPath = "/cores/*";

#endif

	if (itsLink->OKToDetachOrKill())
	{
		auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectSingleFile, origPath);
		if (dlog->DoDialog())
		{
			itsLink->SetCore(dlog->GetFullName());
		}
	}
}

/******************************************************************************
 ChooseProcess

 ******************************************************************************/

void
CommandDirector::ChooseProcess()
{
	if (itsLink->OKToDetachOrKill())
	{
		auto* dlog = jnew ChooseProcessDialog();
		assert( dlog != nullptr );
		if (dlog->DoDialog())
		{
			JInteger pid;
			const bool ok = dlog->GetProcessID(&pid);
			assert( ok );

			GetLink()->AttachToProcess(pid);
		}
	}
}

/******************************************************************************
 DisplayExpression

 ******************************************************************************/

void
CommandDirector::DisplayExpression
	(
	const JString& expr
	)
{
	itsVarTreeDir->Activate();
	itsVarTreeDir->DisplayExpression(expr);
}

/******************************************************************************
 Display1DArray

 ******************************************************************************/

void
CommandDirector::Display1DArray
	(
	const JString& expr
	)
{
	auto* dir = jnew Array1DDir(this, expr);
	assert( dir != nullptr );
	dir->Activate();
}

/******************************************************************************
 Display2DArray

 ******************************************************************************/

void
CommandDirector::Display2DArray
	(
	const JString& expr
	)
{
	auto* dir = jnew Array2DDir(this, expr);
	assert( dir != nullptr );
	dir->Activate();
}

/******************************************************************************
 Plot1DArray

 ******************************************************************************/

void
CommandDirector::Plot1DArray
	(
	const JString& expr
	)
{
	auto* dir = jnew Plot2DDir(this, expr);
	assert( dir != nullptr );
	dir->Activate();
}

/******************************************************************************
 UpdatePrefsMenu

 ******************************************************************************/

void
CommandDirector::UpdatePrefsMenu()
{
	PrefsManager::DebuggerType type = GetPrefsManager()->GetDebuggerType();
	itsPrefsMenu->CheckItem(kDebuggerTypeToMenuIndex[ type ]);

//	itsPrefsMenu->DisableItem(kUseJavaCmd);
}

/******************************************************************************
 HandlePrefsMenu

 ******************************************************************************/

void
CommandDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kUseGDBCmd)
	{
		GetPrefsManager()->SetDebuggerType(PrefsManager::kGDBType);
	}
	else if (index == kUseLLDBCmd)
	{
		GetPrefsManager()->SetDebuggerType(PrefsManager::kLLDBType);
	}
	else if (index == kUseJavaCmd)
	{
		GetPrefsManager()->SetDebuggerType(PrefsManager::kJavaType);
	}
	else if (index == kUseXdebugCmd)
	{
		GetPrefsManager()->SetDebuggerType(PrefsManager::kXdebugType);
	}

	else if (index == kEditPrefsCmd)
	{
		GetPrefsManager()->EditPrefs();
	}
	else if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kEditCmdsCmd)
	{
		auto* dlog = jnew EditCommandsDialog;
		assert( dlog != nullptr );
		dlog->DoDialog();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}
}
