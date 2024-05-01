/******************************************************************************
 SourceText.cpp

	Implements a source code viewer.

	BASE CLASS = TextDisplayBase

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "SourceText.h"
#include "SourceDirector.h"
#include "CommandDirector.h"
#include "CommandInput.h"
#include "CStyler.h"
#include "sharedUtil.h"
#include "globals.h"
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jAssert.h>

#include "SourceText-Search.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

SourceText::SourceText
	(
	SourceDirector*		srcDir,
	CommandDirector*	cmdDir,
	JXMenuBar*			menuBar,
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
	TextDisplayBase(jnew StyledText(this, enclosure->GetFontManager()),
					  kSelectableText, true, menuBar, scrollbarSet,
					  enclosure, hSizing, vSizing, x,y, w,h),
	itsSrcDir(srcDir),
	itsCmdDir(cmdDir),
	itsStyler(nullptr)
{
	SetBackColor(GetPrefsManager()->GetColor(PrefsManager::kBackColorIndex));
	SetSingleFocusWidget();

	JXTextMenu* searchMenu = AppendSearchMenu(menuBar);
	itsSearchMenuItemOffset = searchMenu->GetItemCount();
	searchMenu->ShowSeparatorAfter(itsSearchMenuItemOffset);
	searchMenu->AppendMenuItems(kSearchMenuStr);
	ListenTo(searchMenu);
	ConfigureSearchMenu(searchMenu, itsSearchMenuItemOffset);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SourceText::~SourceText()
{
}

/******************************************************************************
 HandleKeyPress

 *****************************************************************************/

void
SourceText::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c.IsPrint() ||
		c == kJDeleteKey || c == kJForwardDeleteKey ||
		c == kJReturnKey || c == kJTabKey)
	{
		itsCmdDir->TransferKeyPressToInput(c, keySym, modifiers);
	}
	else
	{
		TextDisplayBase::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 SetFileType

 ******************************************************************************/

void
SourceText::SetFileType
	(
	const TextFileType type
	)
{
	GetStyler(GetLanguage(type), &itsStyler);

	if (GetText()->GetText().IsEmpty())
	{
		return;
	}

	if (itsStyler != nullptr)
	{
		GetText()->RestyleAll();
	}
	else
	{
		JFont font = GetText()->GetFont(1);
		font.ClearStyle();
		SelectAll();
		SetCurrentFont(font);
		SetCaretLocation(1);
	}
}

/******************************************************************************
 SourceText::StyledText (protected)

 ******************************************************************************/

SourceText::StyledText::~StyledText()
{
	jdelete itsTokenStartList;
}

void
SourceText::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&		text,
	JRunArray<JFont>*	styles,
	TextRange*			recalcRange,
	TextRange*			redrawRange,
	const bool		deletion
	)
{
	if (itsOwner->itsStyler != nullptr)
	{
		itsOwner->itsStyler->UpdateStyles(this, text, styles,
										  recalcRange, redrawRange,
										  deletion, itsTokenStartList);
	}
	else
	{
		itsTokenStartList->RemoveAll();
	}
}

/******************************************************************************
 BoundsResized (virtual protected)

 ******************************************************************************/

void
SourceText::BoundsResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	TextDisplayBase::BoundsResized(dw,dh);

	if (!WillBreakCROnly() && HasSelection())
	{
		TEScrollToSelection(true);
	}
	else if (!WillBreakCROnly())
	{
		const JCoordinate line = itsSrcDir->GetCurrentExecLine();
		if (line > 0)
		{
			GoToLine(line);
		}
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
SourceText::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXMouseButton b = button;
	if (button == kJXMiddleButton && clickCount == 2)
	{
		b = kJXLeftButton;
	}
	itsLastClickCount = clickCount;

	TextDisplayBase::HandleMouseDown(pt, b, clickCount, buttonStates, modifiers);
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
SourceText::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	TextDisplayBase::HandleMouseUp(pt, button, buttonStates, modifiers);
	if (button == kJXMiddleButton && itsLastClickCount == 2)
	{
		JString text;
		if (GetSelection(&text) && !text.Contains("\n"))
		{
			itsCmdDir->DisplayExpression(text);
		}
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SourceText::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXTextMenu* searchMenu;
	const bool ok = GetSearchMenu(&searchMenu);
	assert( ok );

	if (sender == searchMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateCustomSearchMenuItems();
	}
	else if (sender == searchMenu && message.Is(JXMenu::kItemSelected))
	{
		auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		if (HandleCustomSearchMenuItems(selection->GetIndex()))
		{
			return;
		}
	}

	TextDisplayBase::Receive(sender, message);
}

/******************************************************************************
 UpdateCustomSearchMenuItems (private)

 ******************************************************************************/

void
SourceText::UpdateCustomSearchMenuItems()
{
	JXTextMenu* searchMenu;
	const bool ok = GetSearchMenu(&searchMenu);
	assert( ok );

	searchMenu->EnableItem(itsSearchMenuItemOffset + kGoToLineCmd);

	if (!GetText()->IsEmpty())
	{
		searchMenu->EnableItem(itsSearchMenuItemOffset + kBalanceCmd);

		if (itsSrcDir->IsMainSourceWindow())
		{
			searchMenu->EnableItem(itsSearchMenuItemOffset + kGoToCurrentLineCmd);
		}
	}
}

/******************************************************************************
 HandleCustomSearchMenuItems (private)

 ******************************************************************************/

bool
SourceText::HandleCustomSearchMenuItems
	(
	const JIndex origIndex
	)
{
	const JInteger index = JInteger(origIndex) - JInteger(itsSearchMenuItemOffset);
	Focus();

	if (index == kBalanceCmd)
	{
		BalanceFromSelection(this, kCLang);
		return true;
	}

	else if (index == kGoToLineCmd)
	{
		AskForLine();
		return true;
	}
	else if (index == kGoToCurrentLineCmd)
	{
		const JCoordinate line = itsSrcDir->GetCurrentExecLine();
		if (line > 0)
		{
			GoToLine(line);
		}
		return true;
	}

	else
	{
		return false;
	}
}

/******************************************************************************
 UpgradeSearchMenuToolBarID (virtual protected)

 ******************************************************************************/

static const JUtf8Byte* kToolbarIDMap[] =
{
	"GDBBalanceGroupingSymbol",	"BalanceGroupingSymbol::SourceText",
	"GDBGoToLine",				"GoToLine::SourceText",
	"GDBGoToCurrentLine",		"GoToCurrentLine::SourceText",
};

const JSize kToolbarIDMapCount = sizeof(kToolbarIDMap) / sizeof(JUtf8Byte*);

bool
SourceText::UpgradeSearchMenuToolBarID
	(
	JString* s
	)
{
	if (!s->StartsWith("GDB"))
	{
		return false;
	}

	for (JUnsignedOffset i=0; i<kToolbarIDMapCount; i+=2)
	{
		if (*s == kToolbarIDMap[i])
		{
			*s = kToolbarIDMap[i+1];
			return true;
		}
	}

	return false;
}
