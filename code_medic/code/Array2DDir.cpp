/******************************************************************************
 Array2DDir.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "Array2DDir.h"
#include "Array2DTable.h"
#include "Array2DCmd.h"
#include "ArrayExprInput.h"
#include "ArrayIndexInput.h"
#include "CommandDirector.h"
#include "VarNode.h"
#include "globals.h"

#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXRowHeaderWidget.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXWDManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXCloseDirectorTask.h>
#include <jx-af/jx/JXPSPrinter.h>

#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kDefaultColWidth = 50;
const JCoordinate kMinColWidth     = 10;

/******************************************************************************
 Constructor

 *****************************************************************************/

Array2DDir::Array2DDir
	(
	CommandDirector*	supervisor,
	const JString&		expr
	)
	:
	JXWindowDirector(JXGetApplication())
{
	// format variable for input field

	itsExpr = expr;
	VarNode::TrimExpression(&itsExpr);

	itsRowRequestRange.Set(0, 10);
	itsColRequestRange.Set(0, 10);

	Array2DDirX(supervisor);
}

Array2DDir::Array2DDir
	(
	std::istream&			input,
	const JFileVersion	vers,
	CommandDirector*	supervisor
	)
	:
	JXWindowDirector(JXGetApplication())
{
	input >> itsExpr >> itsRowRequestRange >> itsColRequestRange;
	Array2DDirX(supervisor);
	GetWindow()->ReadGeometry(input);
}

// private

void
Array2DDir::Array2DDirX
	(
	CommandDirector* supervisor
	)
{
	itsLink = GetLink();
	ListenTo(itsLink);

	itsCommandDir           = supervisor;
	itsShouldUpdateFlag     = false;		// window is always initially hidden
	itsWaitingForReloadFlag = false;

	itsData = jnew JStringTableData;

	itsRowUpdateList = jnew JArray<JIntRange>;
	itsColUpdateList = jnew JArray<JIntRange>;

	itsRowDisplayRange.SetToEmptyAt(0);
	itsColDisplayRange.SetToEmptyAt(0);

	BuildWindow();

	// after constructing itsTable; before updating
	itsUpdateCmd = itsLink->CreateArray2DCmd(this, itsTable, itsData);

	FitToRanges();

	ListenTo(GetWindow());

	itsCommandDir->DirectorCreated(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

Array2DDir::~Array2DDir()
{
	itsCommandDir->DirectorDeleted(this);

	jdelete itsData;
	jdelete itsRowUpdateList;
	jdelete itsColUpdateList;
	jdelete itsUpdateCmd;
}

/******************************************************************************
 StreamOut

 *****************************************************************************/

void
Array2DDir::StreamOut
	(
	std::ostream& output
	)
{
	output << ' ' << itsExpr;
	output << ' ' << itsRowRequestRange;
	output << ' ' << itsColRequestRange;
	GetWindow()->WriteGeometry(output);	// must be last
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
Array2DDir::Activate()
{
	JXWindowDirector::Activate();
	ShouldUpdate(true);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "Array2DDir-File.h"
#include "Array2DDir-Actions.h"
#include "medic_2d_array_window.xpm"

void
Array2DDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,500, JString::empty);
	window->SetMinSize(300, 250);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Medic_Variables_Array_2D");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 370,30);
	assert( menuBar != nullptr );

	auto* exprLabel =
		jnew JXStaticText(JGetString("exprLabel::Array2DDir::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,40, 70,20);
	exprLabel->SetToLabel(false);

	itsExprInput =
		jnew ArrayExprInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 90,40, 260,20);

	auto* rowFTCContainer =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,60, 120,90);
	assert( rowFTCContainer != nullptr );

	auto* colFTCContainer =
		jnew JXWidgetSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 150,60, 200,90);
	assert( colFTCContainer != nullptr );

	auto* rowIndexLabel =
		jnew JXStaticText(JGetString("rowIndexLabel::Array2DDir::JXLayout"), rowFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,10, 110,20);
	rowIndexLabel->SetToLabel(false);

	auto* colIndexLabel =
		jnew JXStaticText(JGetString("colIndexLabel::Array2DDir::JXLayout"), colFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	colIndexLabel->SetToLabel(false);

	auto* rowStartLabel =
		jnew JXStaticText(JGetString("rowStartLabel::Array2DDir::JXLayout"), rowFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,35, 40,20);
	rowStartLabel->SetToLabel(false);

	itsRowStartIndex =
		jnew ArrayIndexInput(rowFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,35, 60,20);

	auto* colStartLabel =
		jnew JXStaticText(JGetString("colStartLabel::Array2DDir::JXLayout"), colFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,35, 40,20);
	colStartLabel->SetToLabel(false);

	itsColStartIndex =
		jnew ArrayIndexInput(colFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,35, 60,20);

	auto* rowEndLabel =
		jnew JXStaticText(JGetString("rowEndLabel::Array2DDir::JXLayout"), rowFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,65, 40,20);
	rowEndLabel->SetToLabel(false);

	itsRowEndIndex =
		jnew ArrayIndexInput(rowFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,65, 60,20);

	auto* colEndLabel =
		jnew JXStaticText(JGetString("colEndLabel::Array2DDir::JXLayout"), colFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,65, 40,20);
	colEndLabel->SetToLabel(false);

	itsColEndIndex =
		jnew ArrayIndexInput(colFTCContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 50,65, 60,20);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,170, 370,330);
	assert( scrollbarSet != nullptr );

	itsColHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kFixedTop, 30,0, 340,20);

	itsRowHeader =
		jnew JXRowHeaderWidget(itsTable, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kFixedLeft, JXWidget::kVElastic, 0,20, 30,310);

	itsTable =
		jnew Array2DTable(itsCommandDir, this, menuBar, itsData, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 30,20, 340,310);

// end JXLayout

	window->ShouldFocusWhenShow(true);

	UpdateWindowTitle();

	JXDisplay* display = GetDisplay();
	auto* icon      = jnew JXImage(display, medic_2d_array_window);
	window->SetIcon(icon);

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	bool wrapped;
	const JStringMatch m1 = rowIndexLabel->GetText()->SearchForward(
			JStyledText::TextIndex(1,1), JRegex("\\$i"), false, false, &wrapped);
	if (!m1.IsEmpty())
	{
		rowIndexLabel->GetText()->SetFont(JStyledText::TextRange(m1), font, true);
	}

	const JStringMatch m2 = colIndexLabel->GetText()->SearchForward(
			JStyledText::TextIndex(1,1), JRegex("\\$j"), false, false, &wrapped);
	if (!m2.IsEmpty())
	{
		colIndexLabel->GetText()->SetFont(JStyledText::TextRange(m2), font, true);
	}

	itsTable->SetDefaultColWidth(kDefaultColWidth);
	itsColHeader->TurnOnColResizing(kMinColWidth);

	GetPrefsManager()->GetWindowSize(kArray2DWindSizeID, window, true);	// after all widgets

	itsExprInput->GetText()->SetText(itsExpr);
	itsExprInput->SetIsRequired();
	ListenTo(itsExprInput);

	itsRowStartIndex->SetValue(itsRowRequestRange.first);
	ListenTo(itsRowStartIndex);

	itsRowEndIndex->SetValue(itsRowRequestRange.last);
	ListenTo(itsRowEndIndex);

	itsColStartIndex->SetValue(itsColRequestRange.first);
	ListenTo(itsColStartIndex);

	itsColEndIndex->SetValue(itsColRequestRange.last);
	ListenTo(itsColEndIndex);

	ListenTo(itsRowHeader);

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::Array2DDir_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&Array2DDir::UpdateFileMenu,
		&Array2DDir::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	JXTEBase* te = itsTable->GetEditMenuHandler();
	itsExprInput->ShareEditMenu(te);
	itsRowStartIndex->ShareEditMenu(te);
	itsRowEndIndex->ShareEditMenu(te);
	itsColStartIndex->ShareEditMenu(te);
	itsColEndIndex->ShareEditMenu(te);

	itsActionMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::Array2DDir_Actions"));
	itsActionMenu->SetMenuItems(kActionsMenuStr);
	itsActionMenu->AttachHandlers(this,
		&Array2DDir::UpdateActionMenu,
		&Array2DDir::HandleActionMenu);
	ConfigureActionsMenu(itsActionMenu);

	GetApplication()->CreateWindowsMenu(menuBar);
	GetApplication()->CreateHelpMenu(menuBar, "VarTreeHelp-Array2D");

	GetDisplay()->GetWDManager()->DirectorCreated(this);
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
Array2DDir::UpdateWindowTitle()
{
	JString title = itsExpr;
	title += JGetString("WindowTitleSuffix::Array2DDir");
	GetWindow()->SetTitle(title);
}

/******************************************************************************
 GetMenuIcon (virtual)

 ******************************************************************************/

bool
Array2DDir::GetMenuIcon
	(
	const JXImage** icon
	)
	const
{
	*icon = GetArray2DIcon();
	return true;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
Array2DDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsExprInput &&
		(message.Is(JXWidget::kLostFocus) ||
		 message.Is(ArrayExprInput::kReturnKeyPressed)))
	{
		if (itsExprInput->GetText()->GetText() != itsExpr)
		{
			itsExpr = itsExprInput->GetText()->GetText();
			UpdateWindowTitle();
			UpdateAll();
		}
	}

	else if (sender == itsRowStartIndex &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(ArrayIndexInput::kReturnKeyPressed)))
	{
		JInteger value;
		if (itsRowStartIndex->GetValue(&value) && value != itsRowRequestRange.first)
		{
			itsRowRequestRange.first = value;
			FitToRanges();
		}
	}
	else if (sender == itsRowEndIndex &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(ArrayIndexInput::kReturnKeyPressed)))
	{
		JInteger value;
		if (itsRowEndIndex->GetValue(&value) && value != itsRowRequestRange.last)
		{
			itsRowRequestRange.last = value;
			FitToRanges();
		}
	}
	else if (sender == itsColStartIndex &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(ArrayIndexInput::kReturnKeyPressed)))
	{
		JInteger value;
		if (itsColStartIndex->GetValue(&value) && value != itsColRequestRange.first)
		{
			itsColRequestRange.first = value;
			FitToRanges();
		}
	}
	else if (sender == itsColEndIndex &&
			 (message.Is(JXWidget::kLostFocus) ||
			  message.Is(ArrayIndexInput::kReturnKeyPressed)))
	{
		JInteger value;
		if (itsColEndIndex->GetValue(&value) && value != itsColRequestRange.last)
		{
			itsColRequestRange.last = value;
			FitToRanges();
		}
	}

	else if (sender == itsLink && VarNode::ShouldUpdate(message))
	{
		UpdateAll();
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

	else if (sender == GetWindow() && message.Is(JXWindow::kIconified))
	{
		ShouldUpdate(false);
	}
	else if (sender == GetWindow() && message.Is(JXWindow::kDeiconified))
	{
		ShouldUpdate(true);
	}

	else if (sender == itsRowHeader && message.Is(JXRowHeaderWidget::kNeedsToBeWidened))
	{
		auto& info = dynamic_cast<const JXRowHeaderWidget::NeedsToBeWidened&>(message);

		const JCoordinate dw = info.GetDeltaWidth();
		itsRowHeader->AdjustSize(dw,0);
		itsColHeader->Move(dw,0);
		itsColHeader->AdjustSize(-dw,0);
		itsTable->Move(dw,0);
		itsTable->AdjustSize(-dw,0);
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
Array2DDir::ReceiveGoingAway
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
 FitToRanges (private)

 ******************************************************************************/

void
Array2DDir::FitToRanges()
{
	JXGetApplication()->DisplayBusyCursor();

	JIntRange overlap;
	if (JIntersection(itsRowDisplayRange, itsRowRequestRange, &overlap))
	{
		if (itsRowDisplayRange.first < itsRowRequestRange.first)
		{
			itsData->RemoveNextRows(1, itsRowRequestRange.first - itsRowDisplayRange.first);
			itsRowDisplayRange.first = itsRowRequestRange.first;
		}
		else if (itsRowRequestRange.first < itsRowDisplayRange.first)
		{
			const JIntRange newRange(itsRowRequestRange.first, itsRowDisplayRange.first-1);

			itsData->PrependRows(itsRowDisplayRange.first - itsRowRequestRange.first);
			itsRowDisplayRange.first = itsRowRequestRange.first;

			SetRowTitles(newRange);
			BeginRowUpdate(newRange);
		}

		if (itsRowRequestRange.last < itsRowDisplayRange.last)
		{
			itsData->RemoveNextRows(itsRowRequestRange.last+1 + 1-itsRowDisplayRange.first,
									itsRowDisplayRange.last - itsRowRequestRange.last);
			itsRowDisplayRange.last = itsRowRequestRange.last;
		}
		else if (itsRowDisplayRange.last < itsRowRequestRange.last)
		{
			const JIntRange newRange(itsRowDisplayRange.last+1, itsRowRequestRange.last);

			itsData->AppendRows(itsRowRequestRange.last - itsRowDisplayRange.last);
			itsRowDisplayRange.last = itsRowRequestRange.last;

			SetRowTitles(newRange);
			BeginRowUpdate(newRange);
		}
	}
	else
	{
		itsData->RemoveAllRows();
		itsData->AppendRows(itsRowRequestRange.GetCount());
		itsRowDisplayRange = itsRowRequestRange;
		SetRowTitles(itsRowRequestRange);
		UpdateAll();
	}

	if (JIntersection(itsColDisplayRange, itsColRequestRange, &overlap))
	{
		if (itsColDisplayRange.first < itsColRequestRange.first)
		{
			itsData->RemoveNextCols(1, itsColRequestRange.first - itsColDisplayRange.first);
			itsColDisplayRange.first = itsColRequestRange.first;
		}
		else if (itsColRequestRange.first < itsColDisplayRange.first)
		{
			const JIntRange newRange(itsColRequestRange.first, itsColDisplayRange.first-1);

			itsData->PrependCols(itsColDisplayRange.first - itsColRequestRange.first);
			itsColDisplayRange.first = itsColRequestRange.first;

			SetColTitles(newRange);
			BeginColUpdate(newRange);
		}

		if (itsColRequestRange.last < itsColDisplayRange.last)
		{
			itsData->RemoveNextCols(itsColRequestRange.last+1 + 1-itsColDisplayRange.first,
									itsColDisplayRange.last - itsColRequestRange.last);
			itsColDisplayRange.last = itsColRequestRange.last;
		}
		else if (itsColDisplayRange.last < itsColRequestRange.last)
		{
			const JIntRange newRange(itsColDisplayRange.last+1, itsColRequestRange.last);

			itsData->AppendCols(itsColRequestRange.last - itsColDisplayRange.last);
			itsColDisplayRange.last = itsColRequestRange.last;

			SetColTitles(newRange);
			BeginColUpdate(newRange);
		}
	}
	else
	{
		itsData->RemoveAllCols();
		itsData->AppendCols(itsColRequestRange.GetCount());
		itsColDisplayRange = itsColRequestRange;
		SetColTitles(itsColRequestRange);
		UpdateAll();
	}
}

/******************************************************************************
 SetRowTitles (private)

 ******************************************************************************/

void
Array2DDir::SetRowTitles
	(
	const JIntRange& r
	)
{
	for (JInteger i=r.first; i<=r.last; i++)
	{
		const JString s(i, 0);
		itsRowHeader->SetRowTitle(i - itsRowDisplayRange.first + 1, s);
	}
}

/******************************************************************************
 SetColTitles (private)

 ******************************************************************************/

void
Array2DDir::SetColTitles
	(
	const JIntRange& r
	)
{
	for (JInteger i=r.first; i<=r.last; i++)
	{
		const JString s(i, 0);
		itsColHeader->SetColTitle(i - itsColDisplayRange.first + 1, s);
	}
}

/******************************************************************************
 ShouldUpdate (private)

 ******************************************************************************/

void
Array2DDir::ShouldUpdate
	(
	const bool update
	)
{
	itsShouldUpdateFlag = update;
	if (itsShouldUpdateFlag)
	{
		UpdateNext();
	}
}

/******************************************************************************
 UpdateAll (private)

 ******************************************************************************/

void
Array2DDir::UpdateAll()
{
	if (itsData->GetColCount() <= itsData->GetRowCount())
	{
		BeginRowUpdate(itsRowDisplayRange);
	}
	else
	{
		BeginColUpdate(itsColDisplayRange);
	}

	itsTable->SetAllCellStyles(
		VarNode::GetFontStyle(false, false));
}

/******************************************************************************
 BeginUpdate (private)

 ******************************************************************************/

void
Array2DDir::BeginUpdate
	(
	JArray<JIntRange>*	list,
	const JIntRange&	range
	)
{
	const JSize count = list->GetItemCount();
	JIntRange overlap;
	for (JIndex i=count; i>=1; i--)
	{
		JIntRange r = list->GetItem(i);
		if (r.Contains(range))
		{
			return;
		}
		else if (range.Contains(r))
		{
			list->RemoveItem(i);
		}
		else if (JIntersection(range, r, &overlap))
		{
			r = JCovering(range, r);
			list->SetItem(i, r);
			return;
		}
	}

	list->AppendItem(range);
	UpdateNext();
}

/******************************************************************************
 UpdateNext

 ******************************************************************************/

void
Array2DDir::UpdateNext()
{
	if (itsTable->GetRowCount() == 0 ||
		itsTable->GetColCount() == 0)
	{
		itsRowUpdateList->RemoveAll();
		itsColUpdateList->RemoveAll();
		return;
	}

	if (!itsShouldUpdateFlag ||
		itsUpdateCmd->GetState() != Command::kUnassigned)
	{
		return;
	}

	Array2DCmd::UpdateType type;
	JIndex index;
	JInteger arrayIndex;
	if (!itsRowUpdateList->IsEmpty())
	{
		JIntRange r = itsRowUpdateList->GetFirstItem();
		if (!JIntersection(itsRowDisplayRange, r, &r))
		{
			itsRowUpdateList->RemoveItem(1);
			UpdateNext();
			return;
		}

		type       = Array2DCmd::kRow;
		index      = r.first - itsRowDisplayRange.first + 1;
		arrayIndex = r.first;

		r.first++;
		if (r.IsEmpty())
		{
			itsRowUpdateList->RemoveItem(1);
		}
		else
		{
			itsRowUpdateList->SetItem(1, r);
		}
	}
	else if (!itsColUpdateList->IsEmpty())
	{
		JIntRange r = itsColUpdateList->GetFirstItem();
		if (!JIntersection(itsColDisplayRange, r, &r))
		{
			itsColUpdateList->RemoveItem(1);
			UpdateNext();
			return;
		}

		type       = Array2DCmd::kCol;
		index      = r.first - itsColDisplayRange.first + 1;
		arrayIndex = r.first;

		r.first++;
		if (r.IsEmpty())
		{
			itsColUpdateList->RemoveItem(1);
		}
		else
		{
			itsColUpdateList->SetItem(1, r);
		}
	}
	else
	{
		return;
	}

	itsTable->CancelEditing();
	itsUpdateCmd->PrepareToSend(type, index, arrayIndex);
	itsUpdateCmd->Send();
}

/******************************************************************************
 GetExpression

 ******************************************************************************/

JString
Array2DDir::GetExpression
	(
	const JPoint& cell
	)
	const
{
	return itsLink->Build2DArrayExpression(
		itsExpr,
		itsRowDisplayRange.first + cell.y - 1,
		itsColDisplayRange.first + cell.x - 1);
}

/******************************************************************************
 UpdateFileMenu

 ******************************************************************************/

void
Array2DDir::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kPrintCmd, !itsData->IsEmpty());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
Array2DDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenCmd)
	{
		itsCommandDir->OpenSourceFiles();
	}

	else if (index == kPageSetupCmd)
	{
		GetPSPrinter()->EditUserPageSetup();
	}
	else if (index == kPrintCmd && itsTable->EndEditing())
	{
		JXPSPrinter* p = GetPSPrinter();
		if (p->ConfirmUserPrintSetup())
		{
			itsTable->Print(*p);
		}
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
Array2DDir::UpdateActionMenu()
{
	if (itsTable->GetTableSelection().HasSelection())
	{
		itsActionMenu->EnableItem(kDisplay1DArrayCmd);
		itsActionMenu->EnableItem(kPlot1DArrayCmd);
		itsActionMenu->EnableItem(kDisplay2DArrayCmd);
		itsActionMenu->EnableItem(kWatchVarCmd);
		itsActionMenu->EnableItem(kWatchLocCmd);
	}

	if (itsLink->GetFeature(Link::kExamineMemory))
	{
		itsActionMenu->EnableItem(kExamineMemCmd);
	}
	if (itsLink->GetFeature(Link::kDisassembleMemory))
	{
		itsActionMenu->EnableItem(kDisassembleMemCmd);
	}

	itsActionMenu->EnableItem(kSavePrefsCmd);
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
Array2DDir::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kDisplay1DArrayCmd)
	{
		Display1DArray();
	}
	else if (index == kPlot1DArrayCmd)
	{
		Plot1DArray();
	}
	else if (index == kDisplay2DArrayCmd)
	{
		Display2DArray();
	}

	else if (index == kWatchVarCmd)
	{
		WatchExpression();
	}
	else if (index == kWatchLocCmd)
	{
		WatchLocation();
	}

	else if (index == kExamineMemCmd)
	{
		ExamineMemory(MemoryDir::kHexByte);
	}
	else if (index == kDisassembleMemCmd)
	{
		ExamineMemory(MemoryDir::kAsm);
	}

	else if (index == kSavePrefsCmd)
	{
		GetPrefsManager()->SaveWindowSize(kArray2DWindSizeID, GetWindow());
	}
}

/******************************************************************************
 Display1DArray

 ******************************************************************************/

void
Array2DDir::Display1DArray()
{
	JTableSelectionIterator iter(&(itsTable->GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		expr = GetExpression(cell);
		itsCommandDir->Display1DArray(expr);
	}
}

/******************************************************************************
 Plot1DArray

 ******************************************************************************/

void
Array2DDir::Plot1DArray()
{
	JTableSelectionIterator iter(&(itsTable->GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		expr = GetExpression(cell);
		itsCommandDir->Plot1DArray(expr);
	}
}

/******************************************************************************
 Display2DArray

 ******************************************************************************/

void
Array2DDir::Display2DArray()
{
	JTableSelectionIterator iter(&(itsTable->GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		expr = GetExpression(cell);
		itsCommandDir->Display2DArray(expr);
	}
}

/******************************************************************************
 WatchExpression

 ******************************************************************************/

void
Array2DDir::WatchExpression()
{
	JTableSelectionIterator iter(&(itsTable->GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		expr = GetExpression(cell);
		GetLink()->WatchExpression(expr);
	}
}

/******************************************************************************
 WatchLocation

 ******************************************************************************/

void
Array2DDir::WatchLocation()
{
	JTableSelectionIterator iter(&(itsTable->GetTableSelection()));
	JPoint cell;
	JString expr;
	while (iter.Next(&cell))
	{
		expr = GetExpression(cell);
		GetLink()->WatchLocation(expr);
	}
}

/******************************************************************************
 ExamineMemory

 ******************************************************************************/

void
Array2DDir::ExamineMemory
	(
	const MemoryDir::DisplayType type
	)
{
	JTableSelectionIterator iter(&(itsTable->GetTableSelection()));
	JPoint cell;
	JString expr;
	MemoryDir* dir = nullptr;
	while (iter.Next(&cell))
	{
		expr = GetExpression(cell);

		dir = jnew MemoryDir(itsCommandDir, expr);
		dir->SetDisplayType(type);
		dir->Activate();
	}

	if (dir == nullptr)
	{
		dir = jnew MemoryDir(itsCommandDir, JString::empty);
		dir->SetDisplayType(type);
		dir->Activate();
	}
}
