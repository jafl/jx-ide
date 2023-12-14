/******************************************************************************
 TextDisplayBase.cpp

	BASE CLASS = JXTEBase

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "TextDisplayBase.h"
#include "globals.h"
#include "sharedUtil.h"
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXStyledText.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TextDisplayBase::TextDisplayBase
	(
	const Type			type,
	const bool			breakCROnly,
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
	JXTEBase(type,
			 jnew JXStyledText(false, false, enclosure->GetFontManager()),
			 true, breakCROnly, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	TextDisplayBaseX(menuBar, scrollbarSet);
}

TextDisplayBase::TextDisplayBase
	(
	JXStyledText*		text,
	const Type			type,
	const bool			breakCROnly,
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
	JXTEBase(type, text, true, breakCROnly, scrollbarSet,
			 enclosure, hSizing, vSizing, x,y, w,h)
{
	TextDisplayBaseX(menuBar, scrollbarSet);
}

// private

void
TextDisplayBase::TextDisplayBaseX
	(
	JXMenuBar*		menuBar,
	JXScrollbarSet*	scrollbarSet
	)
{
	WantInput(true, false);

	scrollbarSet->GetVScrollbar()->SetScrollDelay(0);

	AppendEditMenu(menuBar);

	AdjustFont(this);

	ShouldAllowDragAndDrop(true);
	GetText()->SetCharacterInWordFunction(::IsCharacterInWord);
	SetPTPrinter(GetPTPrinter());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TextDisplayBase::~TextDisplayBase()
{
}

/******************************************************************************
 AdjustFont (static)

	Shared with CommandInput.

 ******************************************************************************/

void
TextDisplayBase::AdjustFont
	(
	JXTEBase* te
	)
{
	JFontManager* fontMgr = te->TEGetFontManager();

	JString name;
	JSize size;
	GetPrefsManager()->GetDefaultFont(&name, &size);
	te->GetText()->SetDefaultFont(
		JFontManager::GetFont(name, size, GetPrefsManager()->GetColor(PrefsManager::kTextColorIndex)));

	// colors

	te->SetFocusColor(GetPrefsManager()->GetColor(PrefsManager::kBackColorIndex));
	te->SetCaretColor(GetPrefsManager()->GetColor(PrefsManager::kCaretColorIndex));
	te->SetSelectionColor(GetPrefsManager()->GetColor(PrefsManager::kSelColorIndex));
	te->SetSelectionOutlineColor(GetPrefsManager()->GetColor(PrefsManager::kSelLineColorIndex));

	// tab width

	const JSize tabCharCount    = GetPrefsManager()->GetTabCharCount();
	const JCoordinate charWidth = te->GetText()->GetDefaultFont().GetCharWidth(fontMgr, JUtf8Character(' '));
	te->SetDefaultTabWidth(tabCharCount * charWidth);
	te->GetText()->SetCRMTabCharCount(tabCharCount);
}

/******************************************************************************
 UpgradeSearchMenuToolBarID (virtual protected)

 ******************************************************************************/

bool
TextDisplayBase::UpgradeSearchMenuToolBarID
	(
	JString* s
	)
{
	return false;
}
