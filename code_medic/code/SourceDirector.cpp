/******************************************************************************
 SourceDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "SourceDirector.h"
#include "LineNumberTable.h"
#include "LineAddressTable.h"
#include "SourceText.h"
#include "CommandDirector.h"
#include "FileDragSource.h"
#include "TextFileType.h"
#include "FnMenuUpdater.h"
#include "FunctionMenu.h"
#include "EditCommandsDialog.h"
#include "DisplaySourceForMainCmd.h"
#include "ClearSourceDisplayTask.h"
#include "GetAssemblyCmd.h"
#include "StackDir.h"
#include "sharedUtil.h"
#include "globals.h"
#include "actionDefs.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXFileNameDisplay.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jx/JXCloseDirectorTask.h>

#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Open source file...      %k Meta-O       %i" kOpenSourceFileAction
	"  | Open this file in editor %k Meta-Shift-O %i" kEditSourceFileAction
	"%l| Load configuration...    %k Ctrl-O       %i" kLoadConfigAction
	"  | Save configuration...    %k Ctrl-S       %i" kSaveConfigAction
	"%l| Page setup...                            %i" kJXPageSetupAction
	"  | Print...                 %k Meta-P       %i" kJXPrintAction
	"%l| Close                    %k Meta-W       %i" kJXCloseWindowAction
	"  | Quit                     %k Meta-Q       %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kEditCmd,
	kLoadConfigCmd,
	kSaveConfigCmd,
	kPageSetupCmd,
	kPrintCmd,
	kCloseCmd,
	kQuitCmd
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
	"  | Mac/Win/X emulation..."
	"%l| Save window size as default";

enum
{
	kUseGDBCmd = 1,
	kUseLLDBCmd,
	kUseJavaCmd,
	kUseXdebugCmd,
	kEditPrefsCmd,
	kEditToolBarCmd,
	kEditCmdsCmd,
	kEditMacWinPrefsCmd,
	kWindowSizeCmd	// must be last item (see menu creation code)
};

static const JIndex kDebuggerTypeToMenuIndex[] =
{
	kUseGDBCmd,
	kUseXdebugCmd,
	kUseJavaCmd,
	kUseLLDBCmd
};

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About"
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

SourceDirector*
SourceDirector::Create
	(
	CommandDirector*	commandDir,
	const JString&		fileOrFn,
	const Type			type
	)
{
	auto* dir = jnew SourceDirector(commandDir, fileOrFn, type);
	assert( dir != nullptr );
	dir->CreateWindowsMenu();
	return dir;
}

/******************************************************************************
 Constructor

 ******************************************************************************/

SourceDirector::SourceDirector
	(
	CommandDirector*	commandDir,
	const Type			type
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsType(type),
	itsSrcMainCmd(nullptr),
	itsGetAssemblyCmd(nullptr)
{
	assert( IsMainSourceWindow() );
	SourceViewDirectorX(commandDir);

	if (itsType == kMainSourceType)
	{
		itsSrcMainCmd = itsLink->CreateDisplaySourceForMainCmd(this);
	}
	else if (itsType == kMainAsmType)
	{
		UpdateFileType();	// only need to do this once
	}
}

// protected

SourceDirector::SourceDirector
	(
	CommandDirector*	commandDir,
	const JString&		fileOrFn,
	const Type			type
	)
	:
	JXWindowDirector(JXGetApplication()),
	itsType(type),
	itsSrcMainCmd(nullptr),
	itsGetAssemblyCmd(nullptr)
{
	SourceViewDirectorX(commandDir);

	if (itsType == kAsmType && !fileOrFn.IsEmpty())
	{
		Location loc;
		loc.SetFunctionName(fileOrFn);
		loc.SetMemoryAddress(JString("0x0", JString::kNoCopy));	// not allowed to be null
		DisplayDisassembly(loc);
	}
	else if (itsType == kSourceType && JFileReadable(fileOrFn))
	{
		DisplayFile(fileOrFn);
	}
}

void
SourceDirector::SourceViewDirectorX
	(
	CommandDirector* commandDir
	)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	itsCommandDir = commandDir;
	ListenTo(itsCommandDir);
	BuildWindow();

	ListenTo(GetPrefsManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SourceDirector::~SourceDirector()
{
	if (itsType == kMainSourceType)
	{
		GetPrefsManager()->SaveWindowSize(kMainCodeWindSizeID, GetWindow());
	}
	else if (itsType == kMainAsmType)
	{
		GetPrefsManager()->SaveWindowSize(kMainAsmWindSizeID, GetWindow());
	}
	else
	{
		itsCommandDir->SourceWindowClosed(this);
	}

	jdelete itsSrcMainCmd;
	jdelete itsGetAssemblyCmd;
}

/******************************************************************************
 GetCurrentExecLine

 ******************************************************************************/

JIndex
SourceDirector::GetCurrentExecLine()
	const
{
	return itsTable->GetCurrentLine();
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

#include "medic_current_source_window.xpm"
#include "medic_current_asm_window.xpm"

#include <jx-af/image/jx/jx_file_open.xpm>
#include <jx-af/image/jx/jx_file_print.xpm>
#include <jx-af/image/jx/jx_help_toc.xpm>
#include <jx-af/image/jx/jx_help_specific.xpm>

void
SourceDirector::BuildWindow()
{
	const JIndex prefID = itsType == kMainSourceType ? kMainSrcToolBarID  :
						  itsType == kMainAsmType    ? kMainAsmToolBarID  :
						  itsType == kAsmType        ? kOtherAsmToolBarID :
						  kOtherSrcToolBarID;

// begin JXLayout

	auto* window = jnew JXWindow(this, 600,550, JString::empty);
	assert( window != nullptr );

	itsMenuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
	assert( itsMenuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), prefID, itsMenuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,500);
	assert( itsToolBar != nullptr );

	itsFileDisplay =
		jnew JXFileNameDisplay(JString::empty, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,530, 580,20);
	assert( itsFileDisplay != nullptr );

	auto* dragSource =
		jnew FileDragSource(this, window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,530, 20,20);
	assert( dragSource != nullptr );

// end JXLayout

	window->SetTitle(JGetString(
		itsType == kMainAsmType ? "InitAsmWindowTitle::SourceDirector" :
								  "InitSourceWindowTitle::SourceDirector"));

	window->SetMinSize(300, 200);

	if (itsType == kMainSourceType)
	{
		window->SetCloseAction(JXWindow::kDeactivateDirector);

		window->SetWMClass(GetWMClassInstance(), GetMainSourceWindowClass());
		GetPrefsManager()->GetWindowSize(kMainCodeWindSizeID, window);

		JXDisplay* display = GetDisplay();
		auto* icon      = jnew JXImage(display, medic_current_source_window);
		assert( icon != nullptr );
		window->SetIcon(icon);
	}
	else if (itsType == kMainAsmType)
	{
		window->SetCloseAction(JXWindow::kDeactivateDirector);

		window->SetWMClass(GetWMClassInstance(), GetMainAsmWindowClass());
		GetPrefsManager()->GetWindowSize(kMainAsmWindSizeID, window);

		JXDisplay* display = GetDisplay();
		auto* icon      = jnew JXImage(display, medic_current_asm_window);
		assert( icon != nullptr );
		window->SetIcon(icon);
	}
	else if (itsType == kAsmType)
	{
		window->SetWMClass(GetWMClassInstance(), GetAsmViewWindowClass());
		GetPrefsManager()->GetWindowSize(kAsmWindSizeID, window, true);
	}
	else
	{
		window->SetWMClass(GetWMClassInstance(), GetSourceViewWindowClass());
		GetPrefsManager()->GetWindowSize(kCodeWindSizeID, window, true);
	}

	itsFileMenu = itsMenuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "SourceDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd,  jx_file_open);
	itsFileMenu->SetItemImage(kPrintCmd, jx_file_print);

	// appends Edit & Search menus

	const JCoordinate kInitTableWidth = 50;

	auto* scrollbarSet =
		jnew JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
						   JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();

	itsText =
		jnew SourceText(this, itsCommandDir, itsMenuBar, scrollbarSet, encl,
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 kInitTableWidth, 0,
						 encl->GetApertureWidth()-kInitTableWidth, 100);
	assert( itsText != nullptr );
	itsText->FitToEnclosure(false, true);
	ListenTo(itsText);

	if (itsType == kMainAsmType || itsType == kAsmType)
	{
		itsTable =
			jnew LineAddressTable(this, itsText, scrollbarSet, encl,
								   JXWidget::kFixedLeft, JXWidget::kVElastic,
								   0, 0, kInitTableWidth, 100);
	}
	else
	{
		itsTable =
			jnew LineNumberTable(this, itsText, scrollbarSet, encl,
								  JXWidget::kFixedLeft, JXWidget::kVElastic,
								  0, 0, kInitTableWidth, 100);
	}
	assert( itsTable != nullptr );
	itsTable->FitToEnclosure(false, true);

	// requires itsText

	itsDebugMenu = itsCommandDir->CreateDebugMenu(itsMenuBar);
	ListenTo(itsDebugMenu);

	itsFnMenu =
		jnew FunctionMenu(nullptr, kUnknownFT, itsText, itsMenuBar,
						   JXWidget::kFixedLeft, JXWidget::kFixedTop,
						   0,0, 10,10);
	assert( itsFnMenu != nullptr );
	itsFnMenu->Hide();

	itsPrefsMenu = itsMenuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "SourceDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	if (IsMainSourceWindow())
	{
		itsPrefsMenu->RemoveItem(kWindowSizeCmd);
	}
	ListenTo(itsPrefsMenu);

	itsHelpMenu = itsMenuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "SourceDirector");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	if (!IsMainSourceWindow())
	{
		(GetDisplay()->GetWDManager())->DirectorCreated(this);
	}
}

/******************************************************************************
 CreateWindowsMenu

	Call this after all directors have been constructed.

 ******************************************************************************/

void
SourceDirector::CreateWindowsMenu()
{
	itsCommandDir->CreateWindowsMenuAndToolBar(
			itsMenuBar, itsToolBar,
			itsType == kMainAsmType || itsType == kAsmType,
			true, itsType != kMainSourceType,
			itsDebugMenu, itsPrefsMenu, itsHelpMenu, kTOCCmd, kThisWindowCmd);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
SourceDirector::GetName()
	const
{
	if (itsType == kMainSourceType)
	{
		return JGetString("MainSourceName::SourceDirector");
	}
	else if (itsType == kMainAsmType)
	{
		return JGetString("MainAsmName::SourceDirector");
	}
	else
	{
		return JXWindowDirector::GetName();
	}
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
SourceDirector::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	if (itsType == kMainSourceType)
	{
		*icon = GetCurrentSourceIcon();
	}
	else if (itsType == kMainAsmType)
	{
		*icon = GetCurrentAsmIcon();
	}
	else if (itsType == kAsmType)
	{
		*icon = GetAsmSourceIcon();
	}
	else
	{
		*icon = GetSourceFileIcon();
	}
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SourceDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (IsMainSourceWindow() &&
		sender == itsLink && message.Is(Link::kDebuggerStarted))
	{
		ClearDisplay();
	}
	else if (sender == itsLink && message.Is(Link::kPrepareToLoadSymbols))
	{
		if (!itsCurrentFile.IsEmpty())		// reload file, in case it changed
		{
			JCharacterRange r;
			const JString fileName = itsCurrentFile;
			const JIndex lineIndex = itsText->GetSelection(&r) ?
									 itsText->GetLineForChar(r.first) :
									 itsTable->GetCurrentLine();
			itsCurrentFile.Clear();			// force reload
			DisplayFile(fileName, lineIndex);
		}
	}
	else if (IsMainSourceWindow() &&
			 sender == itsLink && message.Is(Link::kSymbolsLoaded))
	{
		const auto* info =
			dynamic_cast<const Link::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		UpdateWindowTitle(info->GetProgramName());
	}
	else if (IsMainSourceWindow() &&
			 sender == itsLink && message.Is(Link::kProgramStopped))
	{
		const auto* info =
			dynamic_cast<const Link::ProgramStopped*>(&message);
		assert( info != nullptr);
		const Location* loc;
		const bool hasFile = info->GetLocation(&loc);
		if (itsType == kMainSourceType && hasFile)
		{
			DisplayFile(loc->GetFileName(), loc->GetLineNumber());
		}
		else if (itsType == kMainAsmType &&
				 !loc->GetFunctionName().IsEmpty() &&
				 !loc->GetMemoryAddress().IsEmpty())
		{
			DisplayDisassembly(*loc);
		}
		else if (itsType == kMainAsmType)
		{
			// wait for kProgramStopped2
		}
		else
		{
			ClearDisplay();
			(GetCommandDirector())->GetStackDir()->Activate();
		}
	}
	else if (itsType == kMainAsmType && sender == itsLink &&
			 message.Is(Link::kProgramStopped2))
	{
		const auto* info =
			dynamic_cast<const Link::ProgramStopped2*>(&message);
		assert( info != nullptr);
		const Location* loc;
		info->GetLocation(&loc);
		if (!loc->GetFunctionName().IsEmpty() &&
			!loc->GetMemoryAddress().IsEmpty())
		{
			DisplayDisassembly(*loc);
		}
		else
		{
			ClearDisplay();
		}
	}

	else if (sender == itsLink &&
			 (message.Is(Link::kProgramFinished) ||
			  message.Is(Link::kCoreCleared)     ||
			  message.Is(Link::kDetachedFromProcess)))
	{
		if (itsSrcMainCmd != nullptr)
		{
			itsSrcMainCmd->Command::Send();
		}
		else if (itsType == kMainAsmType)
		{
			ClearDisplay();
		}
	}

	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateFileMenu();
	}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
	}

	else if (sender == itsDebugMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		itsCommandDir->UpdateDebugMenu(itsDebugMenu, itsText, nullptr);
	}
	else if (sender == itsDebugMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsCommandDir->HandleDebugMenu(itsDebugMenu, selection->GetIndex(), itsText, nullptr);
	}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdatePrefsMenu();
	}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandlePrefsMenu(selection->GetIndex());
	}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
	}

	else if (sender == GetPrefsManager() && message.Is(PrefsManager::kFileTypesChanged))
	{
		UpdateFileType();
	}
	else if (sender == GetPrefsManager() && message.Is(PrefsManager::kCustomCommandsChanged))
	{
		itsCommandDir->AdjustDebugMenu(itsDebugMenu);
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
SourceDirector::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown() && IsMainSourceWindow())
	{
		itsLink = GetLink();
		ListenTo(itsLink);

		if (itsType == kMainSourceType)
		{
			jdelete itsSrcMainCmd;
			itsSrcMainCmd = itsLink->CreateDisplaySourceForMainCmd(this);
		}
		else if (itsType == kMainAsmType)
		{
			jdelete itsGetAssemblyCmd;
			itsGetAssemblyCmd = nullptr;
		}

		auto* task = jnew ClearSourceDisplayTask(this);
		assert( task != nullptr );
		task->Go();
	}
	else if (sender == itsLink && !IsShuttingDown())
	{
		JXCloseDirectorTask::Close(this);
	}
	else
	{
		JXWindowDirector::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 DisplayFile

 ******************************************************************************/

void
SourceDirector::DisplayFile
	(
	const JString&	fileName,
	const JIndex	lineNumber,
	const bool	markLine
	)
{
	assert( !fileName.IsEmpty() );

	if (!itsCurrentFile.IsEmpty() && JSameDirEntry(fileName, itsCurrentFile))
	{
		DisplayLine(lineNumber, markLine);
		UpdateWindowTitle(JString::empty);
	}
	else if (JFileReadable(fileName))
	{
		itsCurrentFile = fileName;

		JStyledText::PlainTextFormat format;
		itsText->GetText()->ReadPlainText(itsCurrentFile, &format);

		// Assembly must not end with newline, so we have to enforce this for
		// source code, too.

		const JString& text = itsText->GetText()->GetText();
		JStringIterator iter(text, kJIteratorStartAtEnd);
		JUtf8Character c;
		while (iter.Prev(&c, kJIteratorStay) && c == '\n')
		{
			iter.SkipPrev();
		}
		if (!iter.AtEnd())
		{
			itsText->SetSelection(JCharacterRange(
				iter.GetNextCharacterIndex(), text.GetCharacterCount()));
			iter.Invalidate();
			itsText->DeleteSelection();
		}
		iter.Invalidate();

		itsText->SetCaretLocation(1);

		itsFileDisplay->GetText()->SetText(itsCurrentFile);

		UpdateFileType();
		UpdateWindowTitle(JString::empty);

		DisplayLine(lineNumber, markLine);
	}
}

/******************************************************************************
 DisplayDisassembly

 ******************************************************************************/

void
SourceDirector::DisplayDisassembly
	(
	const Location& loc
	)
{
	const JString& fnName = loc.GetFunctionName();
	const JString& addr   = loc.GetMemoryAddress();

	assert( !fnName.IsEmpty() );
	assert( !addr.IsEmpty() );

	if (fnName == itsCurrentFn)
	{
		JIndex i;
		if (dynamic_cast<LineAddressTable*>(itsTable)->FindAddressLineNumber(addr, &i))
		{
			DisplayLine(i);
		}
		else
		{
			itsTable->SetCurrentLine(0);
		}

		UpdateWindowTitle(JString::empty);
	}
	else
	{
		itsCurrentFn = fnName;

		if (itsGetAssemblyCmd == nullptr)
		{
			itsGetAssemblyCmd = GetLink()->CreateGetAssemblyCmd(this);
		}

		if (itsGetAssemblyCmd != nullptr)
		{
			itsAsmLocation = loc;
			itsGetAssemblyCmd->Send();
		}

		UpdateWindowTitle(JString::empty);
	}
}

/******************************************************************************
 DisplayDisassembly

 ******************************************************************************/

void
SourceDirector::DisplayDisassembly
	(
	JPtrArray<JString>*	addrList,
	const JString&		instText
	)
{
	auto* table = dynamic_cast<LineAddressTable*>(itsTable);
	table->SetLineNumbers(addrList);
	itsText->GetText()->SetText(instText);

	JIndex i;
	if (!addrList->IsEmpty() &&
		table->FindAddressLineNumber(itsAsmLocation.GetMemoryAddress(), &i))
	{
		DisplayLine(i);
	}
	else
	{
		table->SetCurrentLine(0);
	}
}

/******************************************************************************
 DisplayLine

 ******************************************************************************/

void
SourceDirector::DisplayLine
	(
	const JSize		lineNumber,
	const bool	markLine
	)
{
	if (lineNumber > 0)
	{
		const JIndex i = itsText->CRLineIndexToVisualLineIndex(lineNumber);

		itsText->GoToLine(i);
		if (IsMainSourceWindow() && markLine)
		{
			itsTable->SetCurrentLine(i);
		}
		else
		{
			itsText->SelectLine(i);
		}
	}
}

/******************************************************************************
 ClearDisplay

 ******************************************************************************/

void
SourceDirector::ClearDisplay()
{
	auto* table = dynamic_cast<LineAddressTable*>(itsTable);
	if (table != nullptr)
	{
		table->ClearLineNumbers();
	}
	itsText->GetText()->SetText(JString::empty);
	itsFileDisplay->GetText()->SetText(JString::empty);
	itsCurrentFile.Clear();
	itsCurrentFn.Clear();
	UpdateWindowTitle(JString::empty);

	UpdateFileType();
}

/******************************************************************************
 UpdateFileType

 ******************************************************************************/

void
SourceDirector::UpdateFileType()
{
	const TextFileType fileType =
		itsType == kMainAsmType || itsType == kAsmType ? kAssemblyFT :
		itsCurrentFile.IsEmpty() ? kUnknownFT :
			GetPrefsManager()->GetFileType(itsCurrentFile);

	itsText->SetFileType(fileType);

	if (!itsCurrentFile.IsEmpty() && (GetFnMenuUpdater())->CanCreateMenu(fileType))
	{
		itsMenuBar->InsertMenuAfter(itsDebugMenu, itsFnMenu);
		itsFnMenu->TextChanged(fileType, itsCurrentFile);
	}
	else
	{
		itsMenuBar->RemoveMenu(itsFnMenu);
	}

	if (!itsCurrentFile.IsEmpty())
	{
		bool setTabWidth, setTabMode, tabInsertsSpaces, setAutoIndent, autoIndent;
		JSize tabWidth;
		ParseEditorOptions(itsCurrentFile, itsText->GetText()->GetText(), &setTabWidth, &tabWidth,
						   &setTabMode, &tabInsertsSpaces, &setAutoIndent, &autoIndent);
		if (setTabWidth)
		{
			itsText->GetText()->SetCRMTabCharCount(tabWidth);
			itsText->SetDefaultTabWidth(
				tabWidth * itsText->GetText()->GetDefaultFont().GetCharWidth(itsText->GetFontManager(), JUtf8Character(' ')));
		}
	}
}

/******************************************************************************
 UpdateWindowTitle (private)

	binaryName can be nullptr

 ******************************************************************************/

void
SourceDirector::UpdateWindowTitle
	(
	const JString& binaryName
	)
{
	JString path, title, prefix;
	if (itsType == kMainAsmType || itsType == kAsmType)
	{
		prefix = JGetString("WindowTitlePrefixMainAsm::SourceDirector");
		title  = itsCurrentFn.IsEmpty() ? JGetString("WindowTitleNoAsm::SourceDirector") : itsCurrentFn;
	}
	else
	{
		prefix = JGetString("WindowTitlePrefixMainSrc::SourceDirector");

		if (itsCurrentFile.IsEmpty())
		{
			title = JGetString("WindowTitleNoSrc::SourceDirector");
		}
		else
		{
			JSplitPathAndName(itsCurrentFile, &path, &title);
		}
	}

	if (IsMainSourceWindow())
	{
		// Save prefix with binary name since we will be called when file
		// name changes, too.

		if (!binaryName.IsEmpty())
		{
			itsWindowTitlePrefix  = binaryName;
			itsWindowTitlePrefix += prefix;
		}
		else if (itsWindowTitlePrefix.IsEmpty())
		{
			itsWindowTitlePrefix = prefix;
		}

		title.Prepend(itsWindowTitlePrefix);
	}

	GetWindow()->SetTitle(title);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
SourceDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kEditCmd, !itsCurrentFile.IsEmpty());
	itsFileMenu->SetItemEnable(kLoadConfigCmd, itsLink->HasProgram());
	itsFileMenu->SetItemEnable(kCloseCmd, !GetWindow()->IsDocked());
}

/******************************************************************************
 HandleFileMenu

 ******************************************************************************/

void
SourceDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
	{
		itsCommandDir->OpenSourceFiles();
	}
	else if (index == kEditCmd)
	{
		JCharacterRange r;
		const JIndex visualIndex =
			itsText->GetSelection(&r) ?
				itsText->GetLineForChar(r.first) :
				itsTable->GetCurrentLine();

		(GetApplication())->
			EditFile(itsCurrentFile,
				visualIndex > 0 ?
					itsText->VisualLineIndexToCRLineIndex(visualIndex) :
					visualIndex);
	}

	else if (index == kLoadConfigCmd)
	{
		itsCommandDir->LoadConfig();
	}
	else if (index == kSaveConfigCmd)
	{
		itsCommandDir->SaveConfig();
	}

	else if (index == kPageSetupCmd)
	{
		itsText->HandlePTPageSetup();
	}
	else if (index == kPrintCmd)
	{
		itsText->PrintPT();
	}

	else if (index == kCloseCmd)
	{
		if (IsMainSourceWindow())
		{
			Deactivate();
		}
		else
		{
			Close();
		}
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 UpdatePrefsMenu

 ******************************************************************************/

void
SourceDirector::UpdatePrefsMenu()
{
	PrefsManager::DebuggerType type = GetPrefsManager()->GetDebuggerType();
	itsPrefsMenu->CheckItem(kDebuggerTypeToMenuIndex[ type ]);

	itsPrefsMenu->DisableItem(kUseJavaCmd);
}

/******************************************************************************
 HandlePrefsMenu

 ******************************************************************************/

void
SourceDirector::HandlePrefsMenu
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
		assert(dlog != nullptr);
		dlog->BeginDialog();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}

	else if (index == kWindowSizeCmd)
	{
		if (itsType == kSourceType)
		{
			GetPrefsManager()->SaveWindowSize(kCodeWindSizeID, GetWindow());
		}
		else if (itsType == kAsmType)
		{
			GetPrefsManager()->SaveWindowSize(kAsmWindSizeID, GetWindow());
		}
	}
}

/******************************************************************************
 HandleHelpMenu

 ******************************************************************************/

void
SourceDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kAboutCmd)
	{
		(GetApplication())->DisplayAbout();
	}
	else if (index == kTOCCmd)
	{
		JXGetHelpManager()->ShowTOC();
	}
	else if (index == kOverviewCmd)
	{
		JXGetHelpManager()->ShowSection("OverviewHelp");
	}
	else if (index == kThisWindowCmd)
	{
		JXGetHelpManager()->ShowSection("SourceWindowHelp");
	}
	else if (index == kChangesCmd)
	{
		JXGetHelpManager()->ShowChangeLog();
	}
	else if (index == kCreditsCmd)
	{
		JXGetHelpManager()->ShowCredits();
	}
}
