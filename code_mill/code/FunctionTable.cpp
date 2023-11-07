/******************************************************************************
 FunctionTable.cpp

	<Description>

	BASE CLASS = public JXTable

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#include "FunctionTable.h"
#include "MemberFunction.h"
#include "Class.h"

#include "globals.h"

#include <jx-af/jx/JXColHeaderWidget.h>
#include <jx-af/jx/JXColorManager.h>

#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/JTableSelection.h>

#include <signal.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kDefColWidth       = 100;
const JCoordinate kUsedColWidth      = 60;
const JCoordinate kSignatureColWidth = 200;
const JCoordinate kDefRowHeight      = 20;
const JCoordinate kHMarginWidth      = 5;
const JCoordinate kBulletRadius      = 4;

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

FunctionTable::FunctionTable
	(
	Class* 				list,
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
	JXTable(kDefRowHeight, kDefColWidth, scrollbarSet,
			enclosure, hSizing, vSizing, x,y, w,h),
	itsList(list),
	itsNeedsAdjustment(true)
{
	ListenTo(itsList, std::function([this](const JListT::ItemsInserted&)
	{
		const JSize delta = itsList->GetItemCount() - GetRowCount();
		if (delta != 0)
		{
			AppendCols(delta);
			itsNeedsAdjustment = true;
		}
	}));

	SetRowBorderInfo(0, JColorManager::GetBlackColor());
	SetColBorderInfo(0, JColorManager::GetBlackColor());
	AppendCols(kFSignature);
	AppendRows(itsList->GetItemCount());
	
	SetColWidth(kFUsed, kUsedColWidth);
	SetColWidth(kFSignature, kSignatureColWidth);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

FunctionTable::~FunctionTable()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
FunctionTable::HandleMouseDown
	(
	const JPoint& pt,
	const JXMouseButton button,
	const JSize clickCount,
	const JXButtonStates& buttonStates,
	const JXKeyModifiers& modifiers
	)
{
	if (ScrollForWheel(button, modifiers))
	{
		return;
	}

	JPoint cell;
	if (GetCell(pt, &cell))
	{
		MemberFunction* fn	= itsList->GetItem(cell.y);
		fn->ShouldBeUsed(!fn->IsUsed());
		TableRefresh();
	}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
FunctionTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	AdjustColumnWidths();
	
	if (JRound(cell.y/2)*2 != cell.y)
	{
		p.SetPenColor(JColorManager::GetGrayColor(95));
		p.SetFilling(true);
		p.Rect(rect);
		p.SetFilling(false);
	}

	const MemberFunction* fn	= itsList->GetItem(cell.y);

	if (cell.x == kFUsed)
	{
		if (fn->IsUsed())
		{
			JRect r(rect.ycenter(), rect.xcenter(), rect.ycenter(), rect.xcenter());
			r.Expand(kBulletRadius, kBulletRadius);
			p.SetPenColor(JColorManager::GetBlackColor());
			p.SetFilling(true);
			p.Ellipse(r);
		}
		return;
	}

	JFontStyle style;
	JPainter::HAlign halign	= JPainter::HAlign::kLeft;

	if (fn->IsRequired())
	{
		style.italic = true;
	}

	if (fn->IsProtected())
	{
		style.color = JColorManager::GetBrownColor();
	}

	JString str;
	if (cell.x == kFReturnType)
	{
		str	= fn->GetReturnType();
	}
	else if (cell.x == kFFunctionName)
	{
		str	= fn->GetFnName();
	}
	else if (cell.x == kFSignature)
	{
		str = fn->GetSignature();
	}

	JRect r = rect;
	r.left  += kHMarginWidth;
	r.right -= kHMarginWidth;

	JFont font = JFontManager::GetDefaultMonospaceFont();
	font.SetStyle(style);
	p.SetFont(font);
	p.String(r, str, halign, JPainter::VAlign::kCenter);
}

/******************************************************************************
 HandleKeyPress (virtual protected)

 ******************************************************************************/

void
FunctionTable::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == ' ')
	{
		GetTableSelection().ClearSelection();
	}
	else
	{
		JXTable::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 AdjustColumnWidths (private)

 ******************************************************************************/

void
FunctionTable::AdjustColumnWidths()
{
	if (!itsNeedsAdjustment)
	{
		return;
	}
	itsNeedsAdjustment = false;

	JFontManager* fontMgr = GetFontManager();
	const JFont& font     = JFontManager::GetDefaultMonospaceFont();

	const JSize count	= itsList->GetItemCount();
	for (JIndex i = 1; i <= count; i++)
	{
		const MemberFunction* fn	= itsList->GetItem(i);
		JSize width	= font.GetStringWidth(fontMgr, fn->GetReturnType());
		JCoordinate adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFReturnType))
		{
			SetColWidth(kFReturnType, adjWidth);
		}
			
		width    = font.GetStringWidth(fontMgr, fn->GetFnName());
		adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFFunctionName))
		{
			SetColWidth(kFFunctionName, adjWidth);
		}

		width    = font.GetStringWidth(fontMgr, fn->GetSignature());
		adjWidth = width + 2 * kHMarginWidth;
		if (adjWidth > GetColWidth(kFSignature))
		{
			SetColWidth(kFSignature, adjWidth);
		}
	}
}
