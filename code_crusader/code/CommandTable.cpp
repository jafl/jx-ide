/******************************************************************************
 CommandTable.cpp

	BASE CLASS = JXEditTable

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CommandTable.h"
#include "CommandSelection.h"
#include "CommandPathInput.h"
#include "ProjectDocument.h"
#include "ListCSF.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jMouseUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;

enum
{
	kMenuTextColumn = 1,
	kMenuShortcutColumn,
	kScriptNameColumn,
	kCommandColumn,
	kOptionsColumn,
	kPathColumn
};

const JCoordinate kInitColWidth[] =
{
	100, 80, 80, 200, 50, 80
};

const JSize kColCount = sizeof(kInitColWidth) / sizeof(JCoordinate);

// geometry information

const JFileVersion kCurrentGeometryDataVersion = 0;
const JUtf8Byte kGeometryDataEndDelimiter      = '\1';

// Options menu

static const JUtf8Byte* kOptionsMenuStr =
	"    This task invokes make / ant / etc.       %b %k (M)"
	"  | This task invokes version control         %b %k (V)"
	"%l| Save all files before performing          %b %k (S)"
	"  | Perform on one file at a time             %b %k (O)"
	"%l| Display output in window                  %b %k (W)"
	"  | Raise window before performing            %b %k (R)"
	"  | Beep when task is finished                %b %k (B)"
	"%l| Display separator after this item in menu %b";

enum
{
	kIsMakeCmd = 1,
	kIsCVSCmd,
	kSaveAllCmd,
	kOneAtATimeCmd,
	kUseWindowCmd,
	kRaisedWhenStartCmd,
	kBeepWhenFinishedCmd,
	kShowSeparatorCmd
};

// import/export

static const JUtf8Byte* kCommandFileSignature = "jx_browser_commands";

/******************************************************************************
 Constructor

 ******************************************************************************/

CommandTable::CommandTable
	(
	const CommandManager::CmdList& cmdList,

	JXTextButton*		addCmdButton,
	JXTextButton*		removeCmdButton,
	JXTextButton*		duplicateCmdButton,
	JXTextButton*		exportButton,
	JXTextButton*		importButton,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1,1, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsFont(JFontManager::GetDefaultFont())
{
	itsTextInput   = nullptr;
	itsDNDRowIndex = 0;

	itsCommandXAtom =
		GetDisplay()->RegisterXAtom(CommandSelection::GetCommandXAtomName());

	// font

	JString fontName;
	JSize fontSize;
	GetPrefsManager()->GetDefaultFont(&fontName, &fontSize);
	itsFont.Set(fontName, fontSize);

	const JSize rowHeight = 2*kVMarginWidth + JMax(
		JFontManager::GetDefaultFont().GetLineHeight(GetFontManager()),
		itsFont.GetLineHeight(GetFontManager()));
	SetDefaultRowHeight(rowHeight);

	// buttons

	itsAddCmdButton       = addCmdButton;
	itsRemoveCmdButton    = removeCmdButton;
	itsDuplicateCmdButton = duplicateCmdButton;
	itsExportButton       = exportButton;
	itsImportButton       = importButton;

	ListenTo(itsAddCmdButton);
	ListenTo(itsRemoveCmdButton);
	ListenTo(itsDuplicateCmdButton);
	ListenTo(itsExportButton);
	ListenTo(itsImportButton);

	itsCSF = jnew ListCSF(JGetString("ReplaceCommandList::CommandTable"),
						   JGetString("AppendToCommandList::CommandTable"));
	assert( itsCSF != nullptr );

	// type menu

	itsOptionsMenu = jnew JXTextMenu(JString::empty, this, kFixedLeft, kFixedTop, 0,0, 10,10);
	assert( itsOptionsMenu != nullptr );
	itsOptionsMenu->SetToHiddenPopupMenu();
	itsOptionsMenu->SetMenuItems(kOptionsMenuStr);
	itsOptionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsOptionsMenu);

	// base path

	ProjectDocument* doc = nullptr;
	if (GetDocumentManager()->GetActiveProjectDocument(&doc))
	{
		itsBasePath = doc->GetFilePath();
	}

	// data

	itsCmdList = jnew CommandManager::CmdList(cmdList);
	assert( itsCmdList != nullptr );
	FinishCmdListCopy(itsCmdList);

	for (JIndex i=1; i<=kColCount; i++)
	{
		AppendCols(1, kInitColWidth[i-1]);
	}

	AppendRows(itsCmdList->GetElementCount());

	UpdateButtons();
	ListenTo(&(GetTableSelection()));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CommandTable::~CommandTable()
{
	itsCmdList->DeleteAll();
	jdelete itsCmdList;

	jdelete itsCSF;
}

/******************************************************************************
 GetCommandList

 ******************************************************************************/

void
CommandTable::GetCommandList
	(
	CommandManager::CmdList* cmdList
	)
	const
{
	assert( !IsEditing() );

	cmdList->DeleteAll();
	*cmdList = *itsCmdList;
	FinishCmdListCopy(cmdList);
}

/******************************************************************************
 FinishCmdListCopy (private)

 ******************************************************************************/

void
CommandTable::FinishCmdListCopy
	(
	CommandManager::CmdList* cmdList
	)
	const
{
	const JSize count = cmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		cmdList->SetElement(i, (cmdList->GetElement(i)).Copy());
	}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
CommandTable::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXEditTable::Draw(p, rect);

	if (itsDNDRowIndex > 0)
	{
		p.ResetClipRect();

		const JSize origLineWidth = p.GetLineWidth();
		p.SetLineWidth(3);

		const JRect b = GetBounds();
		if (RowIndexValid(itsDNDRowIndex))
		{
			const JRect r = GetCellRect(JPoint(1, itsDNDRowIndex));
			p.Line(b.left, r.top, b.right, r.top);
		}
		else
		{
			const JRect r = GetCellRect(JPoint(1, GetRowCount()));
			p.Line(b.left, r.bottom, b.right, r.bottom);
		}

		p.SetLineWidth(origLineWidth);
	}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
CommandTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JPoint editCell;
	if (GetEditedCell(&editCell) && cell == editCell)
	{
		return;
	}

	HilightIfSelected(p, cell, rect);

	const CommandManager::CmdInfo info = itsCmdList->GetElement(cell.y);
	if (info.separator)
	{
		JPoint pt1 = rect.bottomLeft(), pt2 = rect.bottomRight();
		pt1.y--;
		pt2.y--;
		p.Line(pt1, pt2);
	}

	if (cell.x == kOptionsColumn)
	{
		JString s;
		if (info.isMake)
		{
			s += ",M";
		}
		if (info.isVCS)
		{
			s += ",V";
		}
		if (info.saveAll)
		{
			s += ",S";
		}
		if (info.oneAtATime)
		{
			s += ",O";
		}
		if (info.useWindow)
		{
			s += ",W";
		}
		if (info.raiseWindowWhenStart)
		{
			s += ",R";
		}
		if (info.beepWhenFinished)
		{
			s += ",B";
		}

		if (!s.IsEmpty())
		{
			if (s.GetFirstCharacter() == ',')
			{
				JStringIterator iter(&s);
				iter.RemoveNext();
			}
			p.String(rect, s, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	}
	else
	{
		const JString* s = nullptr;
		JFontStyle style;
		if (cell.x == kMenuTextColumn)
		{
			s = info.menuText;
		}
		else if (cell.x == kMenuShortcutColumn)
		{
			s = info.menuShortcut;
		}
		else if (cell.x == kScriptNameColumn)
		{
			s = info.name;
		}
		else if (cell.x == kCommandColumn)
		{
			s = info.cmd;
		}
		else if (cell.x == kPathColumn)
		{
			s = info.path;
			style.color =
				CommandPathInput::GetTextColor(*s, itsBasePath, false);
		}
		assert( s != nullptr );

		JFont font = itsFont;
		font.SetStyle(style);
		p.SetFont(font);

		JRect r = rect;
		r.left += kHMarginWidth;
		p.String(r, *s, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CommandTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	itsStartPt = pt;

	JPoint cell;
	if (ScrollForWheel(button, modifiers) ||
		!GetCell(pt, &cell))
	{
		return;
	}

	if (cell.x == kOptionsColumn)
	{
		SelectSingleCell(cell);
		itsOptionsMenu->PopUp(this, pt, buttonStates, modifiers);
	}
	else if (button == kJXLeftButton)
	{
		SelectSingleCell(cell);
		if (clickCount == 2)
		{
			BeginEditing(cell);
		}
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
CommandTable::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (JMouseMoved(itsStartPt, pt))
	{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetSingleSelectedCell(&cell) && GetCellRect(cell).Contains(itsStartPt))
		{
			auto* data =
				jnew CommandSelection(GetDisplay(), this,
									   itsCmdList->GetElement(cell.y));
			assert( data != nullptr );

			BeginDND(pt, buttonStates, modifiers, data);
		}
	}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
CommandTable::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const bool meta =
		modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex));

	if ((target == this && !meta) ||
		(target != this &&  meta))
	{
		return GetDNDManager()->GetDNDActionMoveXAtom();
	}
	else
	{
		return GetDNDManager()->GetDNDActionCopyXAtom();
	}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept any drops from ourselves and Command from anybody else.

 ******************************************************************************/

bool
CommandTable::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	if (source == this)
	{
		return true;
	}
	else if (source == nullptr)
	{
		return false;
	}

	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
	{
		if (typeList.GetElement(i) == itsCommandXAtom)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
CommandTable::HandleDNDEnter()
{
	itsDNDRowIndex = 0;
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
CommandTable::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	JIndex newRowIndex = itsDNDRowIndex;

	JPoint cell;
	if (GetCell(JPinInRect(pt, GetBounds()), &cell))
	{
		const JRect r = GetCellRect(cell);
		if (pt.y <= r.ycenter())
		{
			newRowIndex = cell.y;
		}
		else
		{
			newRowIndex = cell.y + 1;
		}
	}

	if (newRowIndex != itsDNDRowIndex)
	{
		itsDNDRowIndex = newRowIndex;
		Refresh();
	}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
CommandTable::HandleDNDLeave()
{
	itsDNDRowIndex = 0;
	Refresh();
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept Command, we don't bother to check typeList.

 ******************************************************************************/

void
CommandTable::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();
	JXDNDManager* dndMgr       = GetDNDManager();
	const Atom selName         = dndMgr->GetDNDSelectionName();

	if (source == this && action == dndMgr->GetDNDActionMoveXAtom())
	{
		JPoint cell;
		if ((GetTableSelection()).GetSingleSelectedCell(&cell) &&
			itsDNDRowIndex != JIndex(cell.y) && itsDNDRowIndex != JIndex(cell.y)+1)
		{
			JIndex newIndex = itsDNDRowIndex;
			if (newIndex > JIndex(cell.y))
			{
				newIndex--;
			}
			newIndex = JMin(newIndex, GetRowCount());

			itsCmdList->MoveElementToIndex(cell.y, newIndex);
			MoveRow(cell.y, newIndex);
			SelectSingleCell(JPoint(1, newIndex));
		}
	}
	else if (source == this)
	{
		JPoint cell;
		if ((GetTableSelection()).GetSingleSelectedCell(&cell))
		{
			itsCmdList->InsertElementAtIndex(
				itsDNDRowIndex, (itsCmdList->GetElement(cell.y)).Copy());
			InsertRows(itsDNDRowIndex, 1);
			SelectSingleCell(JPoint(1, itsDNDRowIndex));
		}
	}
	else
	{
		Atom returnType;
		unsigned char* data;
		JSize dataLength;
		JXSelectionManager::DeleteMethod delMethod;
		if (selMgr->GetData(selName, time, itsCommandXAtom,
							&returnType, &data, &dataLength, &delMethod))
		{
			if (returnType == itsCommandXAtom)
			{
				const std::string s((char*) data, dataLength);
				std::istringstream input(s);

				CommandManager::CmdInfo cmdInfo =
					CommandManager::ReadCmdInfo(input, CommandManager::GetCurrentCmdInfoFileVersion());
				if (!input.fail())
				{
					const JIndex newIndex = JMax(JIndex(1), itsDNDRowIndex);
					itsCmdList->InsertElementAtIndex(newIndex, cmdInfo);
					InsertRows(newIndex, 1);
					SelectSingleCell(JPoint(1, newIndex));

					if (action == dndMgr->GetDNDActionMoveXAtom())
					{
						selMgr->SendDeleteRequest(selName, time);
					}
				}
			}

			selMgr->DeleteData(&data, delMethod);
		}
	}

	HandleDNDLeave();
}

/******************************************************************************
 HandleKeyPress (virtual)

	Backspace/Delete:  clear menu text, menu shortcut, name

 ******************************************************************************/

void
CommandTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	bool cleared = false;
	if (c == kJDeleteKey || c == kJForwardDeleteKey)
	{
		JTableSelection& s = GetTableSelection();
		JPoint cell;
		if (s.GetSingleSelectedCell(&cell))
		{
			CommandManager::CmdInfo info = itsCmdList->GetElement(cell.y);
			if (cell.x == kMenuTextColumn)
			{
				info.menuText->Clear();
				cleared = true;
			}
			else if (cell.x == kMenuShortcutColumn)
			{
				info.menuShortcut->Clear();
				cleared = true;
			}
			else if (cell.x == kScriptNameColumn)
			{
				info.name->Clear();
				cleared = true;
			}
		}
	}

	if (cleared)
	{
		Refresh();
	}
	else if (IsEditing())
	{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
	}
	else
	{
		HandleSelectionKeyPress(c, modifiers);
	}
}

/******************************************************************************
 IsEditable (virtual)

 ******************************************************************************/

bool
CommandTable::IsEditable
	(
	const JPoint& cell
	)
	const
{
	return cell.x != kOptionsColumn;
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
CommandTable::CreateXInputField
	(
	const JPoint&		cell,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	assert( itsTextInput == nullptr );

	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	if (cell.x == kPathColumn)
	{
		auto* pathInput =
			jnew CommandPathInput(this, kFixedLeft, kFixedTop, x,y, w,h);
		pathInput->SetBasePath(itsBasePath);
		pathInput->ShouldAllowInvalidPath();
		itsTextInput = pathInput;
	}
	else
	{
		itsTextInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x,y, w,h);
	}
	assert( itsTextInput != nullptr );

	const CommandManager::CmdInfo info = itsCmdList->GetElement(cell.y);
	const JString* text = nullptr;
	if (cell.x == kMenuTextColumn)
	{
		text = info.menuText;
	}
	else if (cell.x == kMenuShortcutColumn)
	{
		text = info.menuShortcut;
	}
	else if (cell.x == kScriptNameColumn)
	{
		text = info.name;
	}
	else if (cell.x == kCommandColumn)
	{
		text = info.cmd;
	}
	else if (cell.x == kPathColumn)
	{
		text = info.path;
	}
	assert( text != nullptr );

	itsTextInput->GetText()->SetText(*text);
	itsTextInput->SetFont(itsFont);
	return itsTextInput;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

static const JRegex illegalNamePattern = "[[:space:]]+";

bool
CommandTable::ExtractInputData
	(
	const JPoint& cell
	)
{
	assert( itsTextInput != nullptr && cell.x != kOptionsColumn );

	CommandManager::CmdInfo info = itsCmdList->GetElement(cell.y);

	const JString& text = itsTextInput->GetText()->GetText();

	JString* s = nullptr;
	if (cell.x == kMenuTextColumn)
	{
		s = info.menuText;
	}
	else if (cell.x == kMenuShortcutColumn)
	{
		s = info.menuShortcut;
	}
	else if (cell.x == kScriptNameColumn)
	{
		if (illegalNamePattern.Match(text))
		{
			JGetUserNotification()->ReportError(JGetString("NoSpacesInCmdName::CommandTable"));
			return false;
		}
		s = info.name;
	}
	else if (cell.x == kCommandColumn)
	{
		s = info.cmd;
	}
	else if (cell.x == kPathColumn)
	{
		s = info.path;
	}
	assert( s != nullptr );

	if (itsTextInput->InputValid())
	{
		*s = text;
		s->TrimWhitespace();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
CommandTable::PrepareDeleteXInputField()
{
	itsTextInput = nullptr;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CommandTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddCmdButton && message.Is(JXButton::kPushed))
	{
		AddCommand();
	}
	else if (sender == itsRemoveCmdButton && message.Is(JXButton::kPushed))
	{
		RemoveCommand();
	}
	else if (sender == itsDuplicateCmdButton && message.Is(JXButton::kPushed))
	{
		DuplicateCommand();
	}

	else if (sender == itsExportButton && message.Is(JXButton::kPushed))
	{
		ExportAllCommands();
	}
	else if (sender == itsImportButton && message.Is(JXButton::kPushed))
	{
		ImportCommands();
	}


	else if (sender == itsOptionsMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateOptionsMenu();
	}
	else if (sender == itsOptionsMenu && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleOptionsMenu(selection->GetIndex());
	}

	else
	{
		if (sender == &(GetTableSelection()))
		{
			UpdateButtons();
		}

		JXEditTable::Receive(sender, message);
	}
}

/******************************************************************************
 AddCommand (private)

 ******************************************************************************/

void
CommandTable::AddCommand()
{
	if (EndEditing())
	{
		CommandManager::CmdInfo info(jnew JString("./"), jnew JString, jnew JString,
									   jnew JString, jnew JString, jnew JString);
		assert( info.path != nullptr && info.cmd != nullptr && info.name != nullptr &&
				info.menuText != nullptr && info.menuShortcut != nullptr );
		itsCmdList->AppendElement(info);
		AppendRows(1);
		BeginEditing(JPoint(kCommandColumn, itsCmdList->GetElementCount()));
	}
}

/******************************************************************************
 RemoveCommand

 ******************************************************************************/

void
CommandTable::RemoveCommand()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
	{
		CancelEditing();

		CommandManager::CmdInfo info = itsCmdList->GetElement(cell.y);
		info.Free();

		itsCmdList->RemoveElement(cell.y);
		RemoveRow(cell.y);
	}
}

/******************************************************************************
 DuplicateCommand (private)

 ******************************************************************************/

void
CommandTable::DuplicateCommand()
{
	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell) && EndEditing())
	{
		itsCmdList->AppendElement((itsCmdList->GetElement(cell.y)).Copy());
		AppendRows(1);
		BeginEditing(JPoint(kCommandColumn, itsCmdList->GetElementCount()));
	}
}

/******************************************************************************
 ExportAllCommands (private)

 ******************************************************************************/

void
CommandTable::ExportAllCommands()
{
	JString origName, newName;
	if (!EndEditing() ||
		!itsCSF->SaveFile(JGetString("ExportPrompt::CommandTable"), JString::empty,
						  JGetString("ExportFileName::CommandTable"), &newName))
	{
		return;
	}

	std::ofstream output(newName.GetBytes());
	output << kCommandFileSignature << '\n';
	output << CommandManager::GetCurrentCmdInfoFileVersion() << '\n';

	const JSize count = itsCmdList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		output << JBoolToString(true);
		CommandManager::WriteCmdInfo(output, itsCmdList->GetElement(i));
	}

	output << JBoolToString(false) << '\n';
}

/******************************************************************************
 ImportCommands (private)

 ******************************************************************************/

void
CommandTable::ImportCommands()
{
	JString fileName;
	if (!EndEditing() ||
		!itsCSF->ChooseFile(JString::empty, JString::empty,
							JGetString("ImportFilter::CommandTable"),
							JString::empty, &fileName))
	{
		return;
	}

	// read file

	std::ifstream input(fileName.GetBytes());

	CommandManager::CmdList cmdList;
	if (ProjectDocument::ReadTasksFromProjectFile(input, &cmdList))
	{
		if (itsCSF->ReplaceExisting())
		{
			itsCmdList->DeleteAll();
		}

		const JSize count = cmdList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			itsCmdList->AppendElement(cmdList.GetElement(i));
		}
	}
	else
	{
		const JString signature = JRead(input, strlen(kCommandFileSignature));
		if (input.fail() || signature != kCommandFileSignature)
		{
			JGetUserNotification()->ReportError(JGetString("ImportNotTaskFile::CommandTable"));
			return;
		}

		JFileVersion vers;
		input >> vers;
		if (input.fail() || vers > CommandManager::GetCurrentCmdInfoFileVersion())
		{
			JGetUserNotification()->ReportError(JGetString("ImportNewerVersion::CommandTable"));
			return;
		}

		if (itsCSF->ReplaceExisting())
		{
			itsCmdList->DeleteAll();
		}

		while (true)
		{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
			if (input.fail() || !keepGoing)
			{
				break;
			}

			CommandManager::CmdInfo info = CommandManager::ReadCmdInfo(input, vers);
			itsCmdList->AppendElement(info);
		}
	}

	// adjust table

	const JSize count = itsCmdList->GetElementCount();
	if (GetRowCount() < count)
	{
		AppendRows(count - GetRowCount());
	}
	else if (count < GetRowCount())
	{
		RemoveNextRows(count+1, GetRowCount() - count);
	}

	Refresh();
}

/******************************************************************************
 UpdateOptionsMenu (private)

 ******************************************************************************/

void
CommandTable::UpdateOptionsMenu()
{
	JPoint cell;
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	bool changed = false;

	CommandManager::CmdInfo info = itsCmdList->GetElement(cell.y);
	if (info.isMake)
	{
		itsOptionsMenu->CheckItem(kIsMakeCmd);
		info.saveAll   = true;
		info.useWindow = true;
		changed        = true;
	}

	if (info.isVCS)
	{
		itsOptionsMenu->CheckItem(kIsCVSCmd);
		info.saveAll = true;
		changed      = true;
	}

	itsOptionsMenu->SetItemEnable(kSaveAllCmd, !info.isMake && !info.isVCS);
	if (info.saveAll)
	{
		itsOptionsMenu->CheckItem(kSaveAllCmd);
	}

	if (info.oneAtATime)
	{
		itsOptionsMenu->CheckItem(kOneAtATimeCmd);
	}

	itsOptionsMenu->SetItemEnable(kUseWindowCmd, !info.isMake);
	if (info.useWindow)
	{
		itsOptionsMenu->CheckItem(kUseWindowCmd);
	}
	else
	{
		info.raiseWindowWhenStart = false;
		changed                   = true;
	}

	itsOptionsMenu->SetItemEnable(kRaisedWhenStartCmd, info.useWindow);
	if (info.raiseWindowWhenStart)
	{
		itsOptionsMenu->CheckItem(kRaisedWhenStartCmd);
	}

	if (info.beepWhenFinished)
	{
		itsOptionsMenu->CheckItem(kBeepWhenFinishedCmd);
	}

	if (info.separator)
	{
		itsOptionsMenu->CheckItem(kShowSeparatorCmd);
	}

	if (changed)
	{
		itsCmdList->SetElement(cell.y, info);
		TableRefreshCell(cell);
	}
}

/******************************************************************************
 HandleOptionsMenu (private)

 ******************************************************************************/

void
CommandTable::HandleOptionsMenu
	(
	const JIndex index
	)
{
	JPoint cell;
	const bool ok = (GetTableSelection()).GetFirstSelectedCell(&cell);
	assert( ok );

	CommandManager::CmdInfo info = itsCmdList->GetElement(cell.y);
	if (index == kIsMakeCmd)
	{
		info.isMake = !info.isMake;
		if (info.isMake)
		{
			info.saveAll   = true;
			info.useWindow = true;
		}
	}
	else if (index == kIsCVSCmd)
	{
		info.isVCS = !info.isVCS;
		if (info.isVCS)
		{
			info.saveAll = true;
		}
	}
	else if (index == kSaveAllCmd)
	{
		info.saveAll = !info.saveAll;
	}
	else if (index == kOneAtATimeCmd)
	{
		info.oneAtATime = !info.oneAtATime;
	}
	else if (index == kUseWindowCmd)
	{
		info.useWindow = !info.useWindow;
		if (!info.useWindow)
		{
			info.raiseWindowWhenStart = false;
		}
	}
	else if (index == kRaisedWhenStartCmd)
	{
		info.raiseWindowWhenStart = !info.raiseWindowWhenStart;
	}
	else if (index == kBeepWhenFinishedCmd)
	{
		info.beepWhenFinished = !info.beepWhenFinished;
	}
	else if (index == kShowSeparatorCmd)
	{
		info.separator = !info.separator;
	}

	TableRefreshRow(cell.y);
	itsCmdList->SetElement(cell.y, info);
}

/******************************************************************************
 UpdateButtons (private)

 ******************************************************************************/

void
CommandTable::UpdateButtons()
{
	if ((GetTableSelection()).HasSelection())
	{
		itsRemoveCmdButton->Activate();
		itsDuplicateCmdButton->Activate();
	}
	else
	{
		itsRemoveCmdButton->Deactivate();
		itsDuplicateCmdButton->Deactivate();
	}
}

/******************************************************************************
 ReadGeometry

 ******************************************************************************/

void
CommandTable::ReadGeometry
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentGeometryDataVersion)
	{
		JCoordinate w;
		input >> w;
		SetColWidth(kMenuTextColumn, w);
		input >> w;
		SetColWidth(kMenuShortcutColumn, w);
		input >> w;
		SetColWidth(kScriptNameColumn, w);
		input >> w;
		SetColWidth(kCommandColumn, w);
		input >> w;
		SetColWidth(kOptionsColumn, w);
		input >> w;
		SetColWidth(kPathColumn, w);
	}

	JIgnoreUntil(input, kGeometryDataEndDelimiter);
}

/******************************************************************************
 WriteGeometry

 ******************************************************************************/

void
CommandTable::WriteGeometry
	(
	std::ostream& output
	)
	const
{
	output << kCurrentGeometryDataVersion;
	output << ' ' << GetColWidth(kMenuTextColumn);
	output << ' ' << GetColWidth(kMenuShortcutColumn);
	output << ' ' << GetColWidth(kScriptNameColumn);
	output << ' ' << GetColWidth(kCommandColumn);
	output << ' ' << GetColWidth(kOptionsColumn);
	output << ' ' << GetColWidth(kPathColumn);
	output << kGeometryDataEndDelimiter;
}

/******************************************************************************
 SetColTitles

 ******************************************************************************/

void
CommandTable::SetColTitles
	(
	JXColHeaderWidget* widget
	)
	const
{
	widget->SetColumnTitles("CommandTable", kColCount);
}
