/******************************************************************************
 Plot2DDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "Plot2DDir.h"
#include "Plot2DExprTable.h"
#include "Plot2DCmd.h"
#include "CommandDirector.h"
#include "VarNode.h"
#include "globals.h"
#include "actionDefs.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/j2dplot/JX2DPlotWidget.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXWDMenu.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXCloseDirectorTask.h>
#include <jx-af/jx/JXPSPrinter.h>

#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Open source file...      %k Meta-O %i" kOpenSourceFileAction
	"%l| PostScript page setup...           %i" kJXPageSetupAction
	"  | Print PostScript...      %k Meta-P %i" kJXPrintAction
	"  | Print EPS...             %k Meta-P %i" kJXPrintAction
	"%l| Close                    %k Meta-W %i" kJXCloseWindowAction
	"  | Quit                     %k Meta-Q %i" kJXQuitAction;

enum
{
	kOpenCmd = 1,
	kPSPageSetupCmd,
	kPrintPSCmd,
	kPrintEPSCmd,
	kCloseWindowCmd,
	kQuitCmd
};

// Actions menu

static const JUtf8Byte* kActionMenuStr =
	"Save window size as default";

enum
{
	kSavePrefsCmd = 1
};

// Help menu

static const JUtf8Byte* kHelpMenuStr =
	"    About"
	"%l| Table of Contents"
	"  | Overview"
	"  | This window %k F1"
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
 Constructor

 *****************************************************************************/

Plot2DDir::Plot2DDir
	(
	CommandDirector*	supervisor,
	const JString&		origExpr
	)
	:
	JXWindowDirector(JXGetApplication())
{
	Plot2DDirX1(supervisor);

	// format variable

	JString expr = origExpr;
	VarNode::TrimExpression(&expr);

	itsExprData->AppendRows(1);
	itsExprData->SetString(1, Plot2DExprTable::kXExprColIndex, JString("$i", JString::kNoCopy));
	itsExprData->SetString(1, Plot2DExprTable::kYExprColIndex, expr);
	itsExprData->SetString(1, Plot2DExprTable::kRangeMinColIndex, JString("0", JString::kNoCopy));
	itsExprData->SetString(1, Plot2DExprTable::kRangeMaxColIndex, JString("10", JString::kNoCopy));

	Plot2DDirX2();
}

Plot2DDir::Plot2DDir
	(
	std::istream&		input,
	const JFileVersion	vers,
	CommandDirector*	supervisor
	)
	:
	JXWindowDirector(JXGetApplication())
{
	Plot2DDirX1(supervisor);

	JSize count;
	input >> count;
	itsExprData->AppendRows(count);

	JString s;
	for (JIndex i=1; i<=count; i++)
	{
		for (JIndex j=1; j<=Plot2DExprTable::kColCount; j++)
		{
			input >> s;
			itsExprData->SetString(i, j, s);
		}
	}

	Plot2DDirX2();

	for (JIndex i=1; i<=Plot2DExprTable::kColCount; i++)
	{
		JCoordinate w;
		input >> w;
		itsColHeader->SetColWidth(i, w);
	}

	itsPlotWidget->PWXReadSetup(input);
	GetWindow()->ReadGeometry(input);
}

// private

void
Plot2DDir::Plot2DDirX1
	(
	CommandDirector* supervisor
	)
{
	itsLink                 = GetLink();
	itsCommandDir           = supervisor;
	itsShouldUpdateFlag     = false;		// window is always initially hidden
	itsWaitingForReloadFlag = false;

	itsExprData = jnew JStringTableData;
	assert( itsExprData != nullptr );

	itsExprData->AppendCols(Plot2DExprTable::kColCount);

	itsUpdateCmdList = jnew JPtrArray<Plot2DCmd>(JPtrArrayT::kDeleteAll);
	assert( itsUpdateCmdList != nullptr );

	itsXData = jnew JPtrArray<JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert( itsXData != nullptr );

	itsYData = jnew JPtrArray<JArray<JFloat> >(JPtrArrayT::kDeleteAll);
	assert( itsYData != nullptr );
}

void
Plot2DDir::Plot2DDirX2()
{
	BuildWindow();

	itsCommandDir->DirectorCreated(this);

	ListenTo(itsLink);
	ListenTo(itsExprData);
	ListenTo(&(itsExprTable->GetTableSelection()));
	ListenTo(itsPlotWidget);
	ListenTo(GetWindow());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Plot2DDir::~Plot2DDir()
{
	itsCommandDir->DirectorDeleted(this);

	jdelete itsExprData;
	jdelete itsXData;
	jdelete itsYData;
	jdelete itsUpdateCmdList;
}

/******************************************************************************
 StreamOut

 *****************************************************************************/

void
Plot2DDir::StreamOut
	(
	std::ostream& output
	)
{
	// after Plot2DDirX1()

	const JSize count = itsExprData->GetRowCount();
	output << ' ' << count;

	for (JIndex i=1; i<=count; i++)
	{
		for (JIndex j=1; j<=Plot2DExprTable::kColCount; j++)
		{
			output << ' ' << itsExprData->GetString(i, j);
		}
	}

	// after Plot2DDirX2()

	for (JIndex i=1; i<=Plot2DExprTable::kColCount; i++)
	{
		output << ' ' << itsColHeader->GetColWidth(i);
	}

	itsPlotWidget->PWXWriteSetup(output);
	GetWindow()->WriteGeometry(output);	// must be last
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
Plot2DDir::Activate()
{
	JXWindowDirector::Activate();
	ShouldUpdate(true);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "medic_2d_plot_window.xpm"

#include <jx-af/image/jx/jx_file_open.xpm>
#include <jx-af/image/jx/jx_file_print.xpm>
#include <jx-af/image/jx/jx_help_toc.xpm>
#include <jx-af/image/jx/jx_help_specific.xpm>

void
Plot2DDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 570,500, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 570,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,30, 480,100);
	assert( scrollbarSet != nullptr );

	itsPlotWidget =
		jnew JX2DPlotWidget(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,130, 570,370);
	assert( itsPlotWidget != nullptr );

	itsAddPlotButton =
		jnew JXTextButton(JGetString("itsAddPlotButton::Plot2DDir::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 490,40, 70,20);
	assert( itsAddPlotButton != nullptr );

	itsDuplicatePlotButton =
		jnew JXTextButton(JGetString("itsDuplicatePlotButton::Plot2DDir::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 490,70, 70,20);
	assert( itsDuplicatePlotButton != nullptr );

	itsRemovePlotButton =
		jnew JXTextButton(JGetString("itsRemovePlotButton::Plot2DDir::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 490,100, 70,20);
	assert( itsRemovePlotButton != nullptr );

// end JXLayout

	window->SetMinSize(300, 300);
	window->ShouldFocusWhenShow(true);
	window->SetWMClass(GetWMClassInstance(), GetPlot2DWindowClass());

	UpdateWindowTitle();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_2d_plot_window);
	assert( icon != nullptr );
	window->SetIcon(icon);

	JXContainer* encl = scrollbarSet->GetScrollEnclosure();
	const JCoordinate kColHeaderHeight = 20;
	const JCoordinate w = encl->GetApertureWidth();

	itsExprTable =
		jnew Plot2DExprTable(menuBar, itsExprData, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kVElastic,
					0, kColHeaderHeight,
					w, encl->GetApertureHeight() - kColHeaderHeight);
	assert( itsExprTable != nullptr );

	itsColHeader =
		jnew JXColHeaderWidget(itsExprTable, scrollbarSet, encl,
					JXWidget::kHElastic, JXWidget::kFixedTop,
					0,0, w, kColHeaderHeight);
	assert( itsColHeader != nullptr );

	itsExprTable->ConfigureColHeader(itsColHeader);

	itsPlotWidget->SetPSPrinter(GetPSPrinter());
	itsPlotWidget->SetEPSPrinter(GetPlotEPSPrinter());

	GetPrefsManager()->GetWindowSize(kPlot2DWindSizeID, window, true);	// after all widgets

	UpdateButtons();
	ListenTo(itsAddPlotButton);
	ListenTo(itsDuplicatePlotButton);
	ListenTo(itsRemovePlotButton);

	// menus

	itsActionMenu = menuBar->PrependTextMenu(JGetString("ActionsMenuTitle::global"));
	itsActionMenu->SetMenuItems(kActionMenuStr, "Plot2DDir");
	itsActionMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionMenu);

	JXTextMenu* editMenu;
	const bool hasEditMenu = (itsExprTable->GetEditMenuHandler())->GetEditMenu(&editMenu);
	assert( hasEditMenu );
	menuBar->PrependMenu(editMenu);

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "ThreadsDir");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kOpenCmd,    jx_file_open);
	itsFileMenu->SetItemImage(kPrintPSCmd, jx_file_print);

	auto* wdMenu =
		jnew JXWDMenu(JGetString("WindowsMenuTitle::JXGlobal"), menuBar,
					 JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( wdMenu != nullptr );
	menuBar->AppendMenu(wdMenu);

	itsHelpMenu = menuBar->AppendTextMenu(JGetString("HelpMenuTitle::JXGlobal"));
	itsHelpMenu->SetMenuItems(kHelpMenuStr, "Plot2DDir");
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);

	itsHelpMenu->SetItemImage(kTOCCmd,        jx_help_toc);
	itsHelpMenu->SetItemImage(kThisWindowCmd, jx_help_specific);

	(GetDisplay()->GetWDManager())->DirectorCreated(this);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
Plot2DDir::UpdateWindowTitle()
{
	JString title = itsPlotWidget->GetTitle();
	title += JGetString("WindowTitleSuffix::Plot2DDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
Plot2DDir::UpdateButtons()
{
	const bool hasSelection = (itsExprTable->GetTableSelection()).HasSelection();
	itsDuplicatePlotButton->SetActive(hasSelection);
	itsRemovePlotButton->SetActive(hasSelection);
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
Plot2DDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetPlot2DIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Plot2DDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && VarNode::ShouldUpdate(message))
	{
		UpdateAll();
	}

	else if (sender == itsExprData && message.Is(JTableData::kRowsInserted))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowsInserted*>(&message);
		assert( info != nullptr );
		Update(info->GetFirstIndex(), info->GetLastIndex());	// only append
	}
	else if (sender == itsExprData && message.Is(JTableData::kRowDuplicated))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowDuplicated*>(&message);
		assert( info != nullptr );
		Update(info->GetNewIndex(), info->GetNewIndex());
	}
	else if (sender == itsExprData && message.Is(JTableData::kRowsRemoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowsRemoved*>(&message);
		assert( info != nullptr );

		StopListening(itsPlotWidget);
		for (JIndex i=info->GetLastIndex(); i>=info->GetFirstIndex(); i--)
		{
			itsUpdateCmdList->DeleteElement(i);
			itsPlotWidget->RemoveCurve(i);
		}
		ListenTo(itsPlotWidget);
	}
	else if (sender == itsExprData && message.Is(JTableData::kRectChanged))
	{
		const auto* info =
			dynamic_cast<const JTableData::RectChanged*>(&message);
		assert( info != nullptr );
		const JRect& r = info->GetRect();
		Update(r.top, r.bottom-1);
	}

	else if (sender == &(itsExprTable->GetTableSelection()))
	{
		UpdateButtons();
	}

	else if (sender == itsPlotWidget && message.Is(J2DPlotWidget::kCurveRemoved))
	{
		const auto* info =
			dynamic_cast<const J2DPlotWidget::CurveRemoved*>(&message);
		assert( info != nullptr );
		const JIndex index = info->GetIndex();

		StopListening(itsExprData);
		itsUpdateCmdList->DeleteElement(index);
		itsExprData->RemoveRow(index);
		ListenTo(itsExprData);
	}
	else if (sender == itsPlotWidget && message.Is(J2DPlotWidget::kTitleChanged))
	{
		UpdateWindowTitle();
	}

	else if (sender == itsAddPlotButton && message.Is(JXButton::kPushed))
	{
		if (itsExprTable->EndEditing())
		{
			JPtrArray<JString> data(JPtrArrayT::kDeleteAll);
			data.Append(JString("$i", JString::kNoCopy));
			data.Append(JString("$i", JString::kNoCopy));
			data.Append(JString("0", JString::kNoCopy));
			data.Append(JString("10", JString::kNoCopy));
			itsExprData->AppendRows(1, &data);
		}
	}
	else if (sender == itsDuplicatePlotButton && message.Is(JXButton::kPushed))
	{
		DuplicateSelectedPlots();
	}
	else if (sender == itsRemovePlotButton && message.Is(JXButton::kPushed))
	{
		RemoveSelectedPlots();
	}

	else if (sender == itsLink && message.Is(Link::kDebuggerRestarted))
	{
		itsWaitingForReloadFlag = true;
	}
	else if (sender == itsLink && message.Is(Link::kDebuggerStarted))
	{
		if (!itsWaitingForReloadFlag)
		{
			JXCloseDirectorTask::Close(this);	// close after bcast is finished
		}
		itsWaitingForReloadFlag = false;
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

	else if (sender == itsActionMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateActionMenu();
	}
	else if (sender == itsActionMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleActionMenu(selection->GetIndex());
	}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleHelpMenu(selection->GetIndex());
	}

	else if (sender == GetWindow() && message.Is(JXWindow::kIconified))
	{
		ShouldUpdate(false);
	}
	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
	{
		ShouldUpdate(true);
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
Plot2DDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		JXCloseDirectorTask::Close(this);
	}
	else
	{
		JXWindowDirector::ReceiveGoingAway(sender);
	}
}

/******************************************************************************
 ShouldUpdate (private)

 ******************************************************************************/

void
Plot2DDir::ShouldUpdate
	(
	const bool update
	)
{
	itsShouldUpdateFlag = update;
	if (itsShouldUpdateFlag)
	{
		UpdateAll();
	}
}

/******************************************************************************
 UpdateAll (private)

 ******************************************************************************/

void
Plot2DDir::UpdateAll()
{
	Update(1, itsExprData->GetRowCount());
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
Plot2DDir::Update
	(
	const JIndex first,
	const JIndex last
	)
{
	itsExprTable->CancelEditing();

	const JSize count = itsExprData->GetRowCount();
	while (itsUpdateCmdList->GetElementCount() > count)
	{
		itsUpdateCmdList->DeleteElement(itsUpdateCmdList->GetElementCount());
	}

	JString curveName;
	for (JIndex i=first; i<=last; i++)
	{
		Plot2DCmd* cmd;
		if (itsUpdateCmdList->IndexValid(i))
		{
			cmd = itsUpdateCmdList->GetElement(i);
		}
		else
		{
			auto* x = jnew JArray<JFloat>(100);
			assert( x != nullptr );
			itsXData->Append(x);

			auto* y = jnew JArray<JFloat>(100);
			assert( y != nullptr );
			itsYData->Append(y);

			JIndex j;
			const bool ok = itsPlotWidget->AddCurve(*x, *y, true, JString::empty, &j);
			assert( ok && j == i );

			cmd = itsLink->CreatePlot2DCmd(this, x, y);
			itsUpdateCmdList->Append(cmd);
		}

		curveName  = itsExprData->GetString(i, Plot2DExprTable::kXExprColIndex);
		curveName += " ; ";
		curveName += itsExprData->GetString(i, Plot2DExprTable::kYExprColIndex);
		itsPlotWidget->SetCurveName(i, curveName);

		JInteger min, max;
		bool ok = (itsExprData->GetString(i, Plot2DExprTable::kRangeMinColIndex)).ConvertToInteger(&min);
		assert( ok );
		ok = (itsExprData->GetString(i, Plot2DExprTable::kRangeMaxColIndex)).ConvertToInteger(&max);
		assert( ok );

		cmd->UpdateRange(i, min, max);
		cmd->Send();
	}
}

/******************************************************************************
 GetXExpression

 ******************************************************************************/

JString
Plot2DDir::GetXExpression
	(
	const JIndex	curveIndex,
	const JInteger	i
	)
	const
{
	const JString& expr = itsExprData->GetString(curveIndex, Plot2DExprTable::kXExprColIndex);
	return itsLink->Build1DArrayExpression(expr, i);
}

/******************************************************************************
 GetYExpression

 ******************************************************************************/

JString
Plot2DDir::GetYExpression
	(
	const JIndex	curveIndex,
	const JInteger	i
	)
	const
{
	const JString& expr = itsExprData->GetString(curveIndex, Plot2DExprTable::kYExprColIndex);
	return itsLink->Build1DArrayExpression(expr, i);
}

/******************************************************************************
 DuplicateSelectedPlots (private)

 ******************************************************************************/

void
Plot2DDir::DuplicateSelectedPlots()
{
	if (!itsExprTable->EndEditing())
	{
		return;
	}

	JPtrArray<JString> rowData(JPtrArrayT::kDeleteAll);

	JTableSelectionIterator iter(&(itsExprTable->GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		itsExprData->GetRow(cell.y, &rowData);
		itsExprData->AppendRows(1, &rowData);
	}
}

/******************************************************************************
 RemoveSelectedPlots (private)

 ******************************************************************************/

void
Plot2DDir::RemoveSelectedPlots()
{
	itsExprTable->CancelEditing();

	JTableSelectionIterator iter(&(itsExprTable->GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		itsExprData->RemoveRow(cell.y);
	}
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
Plot2DDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kPrintPSCmd, !itsExprData->IsEmpty());
	itsFileMenu->SetItemEnable(kPrintEPSCmd, !itsExprData->IsEmpty());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
Plot2DDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
	{
		itsCommandDir->OpenSourceFiles();
	}

	else if (index == kPSPageSetupCmd)
	{
		(GetPSPrinter())->BeginUserPageSetup();
	}
	else if (index == kPrintPSCmd && itsExprTable->EndEditing())
	{
		itsPlotWidget->PrintPS();
	}
	else if (index == kPrintEPSCmd && itsExprTable->EndEditing())
	{
		itsPlotWidget->PrintPlotEPS();
	}

	else if (index == kCloseWindowCmd)
	{
		Close();
	}
	else if (index == kQuitCmd)
	{
		JXGetApplication()->Quit();
	}
}

/******************************************************************************
 UpdateActionMenu

 ******************************************************************************/

void
Plot2DDir::UpdateActionMenu()
{
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
Plot2DDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kSavePrefsCmd)
	{
		GetPrefsManager()->SaveWindowSize(kPlot2DWindSizeID, GetWindow());
	}
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
Plot2DDir::HandleHelpMenu
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
		JXGetHelpManager()->ShowSection("2DPlotHelp");
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
