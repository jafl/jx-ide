/******************************************************************************
 CRMRuleListTable.cpp

	BASE CLASS = JXStringTable

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CRMRuleListTable.h"
#include "CRMRuleTable.h"
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/JStringTableData.h>
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CRMRuleListTable::CRMRuleListTable
	(
	JArray<PrefsManager::CRMRuleListInfo>*	crmList,

	const JIndex		initialSelection,
	const JIndex		firstUnusedID,
	CRMRuleTable*		ruleTable,
	JXTextButton*		addRowButton,
	JXTextButton*		removeRowButton,
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
	JXStringTable(jnew JStringTableData,
				  scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsCRMList(crmList),
	itsOwnsCRMListFlag(true),
	itsFirstNewID(firstUnusedID),
	itsLastNewID(itsFirstNewID - 1),
	itsCRMIndex(0),
	itsRuleTable(ruleTable)
{
	itsCRMList->SetCompareFunction(CompareNames);
	itsCRMList->Sort();

	itsAddRowButton = addRowButton;
	ListenTo(itsAddRowButton);

	itsRemoveRowButton = removeRowButton;
	itsRemoveRowButton->Deactivate();
	ListenTo(itsRemoveRowButton);

	auto* data = GetStringData();
	data->AppendCols(1);

	const JSize count = itsCRMList->GetElementCount();
	data->AppendRows(count);

	JIndex i = 0;
	for (const auto& info : *itsCRMList)
	{
		i++;
		data->SetString(i,1, *info.name);
	}

	JTableSelection& s = GetTableSelection();
	ListenTo(&s);

	if (data->RowIndexValid(initialSelection))
	{
		s.SelectCell(initialSelection,1);
	}
	else if (data->GetRowCount() > 0)
	{
		s.SelectCell(1,1);
	}
}

std::weak_ordering
CRMRuleListTable::CompareNames
	(
	const PrefsManager::CRMRuleListInfo& i1,
	const PrefsManager::CRMRuleListInfo& i2
	)
{
	return JCompareStringsCaseInsensitive(i1.name, i2.name);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CRMRuleListTable::~CRMRuleListTable()
{
	if (itsOwnsCRMListFlag)
	{
		const JSize count = itsCRMList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
		{
			PrefsManager::CRMRuleListInfo info = itsCRMList->GetElement(i);
			jdelete info.name;
			(info.list)->DeleteAll();
			jdelete info.list;
		}
		jdelete itsCRMList;
	}

	jdelete GetStringData();
}

/******************************************************************************
 GetCurrentCRMRuleSetName

 ******************************************************************************/

bool
CRMRuleListTable::GetCurrentCRMRuleSetName
	(
	JString* name
	)
	const
{
	if (itsCRMIndex > 0 &&
		const_cast<CRMRuleListTable*>(this)->EndEditing())
	{
		*name = GetStringData()->GetString(itsCRMIndex, 1);
		return true;
	}
	else
	{
		name->Clear();
		return false;
	}
}

/******************************************************************************
 GetCRMRuleLists

	The client receives ownership of itsCRMList.  If no rule sets were
	created, *lastNewID < *firstNewID.  Otherwise, all id's between
	*firstNewID and *lastNewID inclusive have been allocated.

 ******************************************************************************/

JArray<PrefsManager::CRMRuleListInfo>*
CRMRuleListTable::GetCRMRuleLists
	(
	JIndex* firstNewID,
	JIndex* lastNewID
	)
	const
{
	assert( !IsEditing() &&
			!itsRuleTable->IsEditing() );

	JStringTableData* data = GetStringData();
	const JSize count      = data->GetRowCount();
	for (JIndex i=1; i<=count; i++)
	{
		PrefsManager::CRMRuleListInfo info = itsCRMList->GetElement(i);
		*(info.name) = data->GetString(i,1);
	}

	if (itsCRMIndex > 0)
	{
		PrefsManager::CRMRuleListInfo info = itsCRMList->GetElement(itsCRMIndex);
		itsRuleTable->GetData(info.list);
	}

	*firstNewID = itsFirstNewID;
	*lastNewID  = itsLastNewID;

	itsOwnsCRMListFlag = false;
	return itsCRMList;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
CRMRuleListTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
	{
		BeginEditing(cell);
	}
	else
	{
		ScrollForWheel(button, modifiers);
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CRMRuleListTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsAddRowButton && message.Is(JXButton::kPushed))
	{
		AddRow();
	}
	else if (sender == itsRemoveRowButton && message.Is(JXButton::kPushed))
	{
		RemoveRow();
	}

	else
	{
		if (sender == &(GetTableSelection()))
		{
			SwitchDisplay();
		}

		JXStringTable::Receive(sender, message);
	}
}

/******************************************************************************
 AddRow (private)

 ******************************************************************************/

void
CRMRuleListTable::AddRow()
{
	if (EndEditing() && itsRuleTable->EndEditing())
	{
		itsLastNewID++;
		PrefsManager::CRMRuleListInfo info(itsLastNewID, jnew JString("New"),
											 jnew JStyledText::CRMRuleList);
		assert( info.name != nullptr &&
				info.list != nullptr );
		itsCRMList->AppendElement(info);

		JStringTableData* data = GetStringData();
		data->AppendRows(1);

		const JPoint newCell(1, GetRowCount());
		data->SetString(newCell, *info.name);
		BeginEditing(newCell);
	}
}

/******************************************************************************
 RemoveRow (private)

 ******************************************************************************/

void
CRMRuleListTable::RemoveRow()
{
	assert( itsCRMIndex > 0 );

	CancelEditing();
	itsRuleTable->CancelEditing();

	auto info = itsCRMList->GetElement(itsCRMIndex);
	jdelete info.name;
	(info.list)->DeleteAll();
	jdelete info.list;

	if (itsFirstNewID <= info.id && info.id == itsLastNewID)
	{
		itsLastNewID--;
	}
	else if (itsFirstNewID <= info.id && info.id < itsLastNewID)
	{
		JIndex index;
		const bool found =
			PrefsManager::FindCRMRuleListID(*itsCRMList, itsLastNewID, &index);
		assert( found );

		auto info2 = itsCRMList->GetElement(index);
		info2.id   = info.id;
		itsCRMList->SetElement(index, info2);

		itsLastNewID--;
	}

	// itsCRMIndex must be 0 when SwitchDisplay() is called

	const JIndex crmIndex = itsCRMIndex;
	itsCRMIndex = 0;
	itsCRMList->RemoveElement(crmIndex);
	GetStringData()->RemoveRow(crmIndex);
}

/******************************************************************************
 SwitchDisplay (private)

 ******************************************************************************/

void
CRMRuleListTable::SwitchDisplay()
{
	if (itsCRMIndex > 0)
	{
		const PrefsManager::CRMRuleListInfo info = itsCRMList->GetElement(itsCRMIndex);
		itsRuleTable->GetData(info.list);
	}

	JPoint cell;
	if ((GetTableSelection()).GetFirstSelectedCell(&cell))
	{
		itsCRMIndex = cell.y;

		const PrefsManager::CRMRuleListInfo info = itsCRMList->GetElement(itsCRMIndex);
		itsRuleTable->SetData(*(info.list));

		itsRemoveRowButton->Activate();
	}
	else
	{
		itsCRMIndex = 0;
		itsRuleTable->ClearData();
		itsRemoveRowButton->Deactivate();
	}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
CRMRuleListTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXStringTable::ApertureResized(dw,dh);
	SetColWidth(1, GetApertureWidth());
}

/******************************************************************************
 CreateStringTableInput (virtual protected)

 ******************************************************************************/

JXInputField*
CRMRuleListTable::CreateStringTableInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectCell(cell);

	JXInputField* input =
		JXStringTable::CreateStringTableInput(
							cell, enclosure, hSizing, vSizing, x,y, w,h);
	input->SetIsRequired();
	return input;
}
