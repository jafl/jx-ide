/******************************************************************************
 FunctionMenu.cpp

	This menu is an action menu, so all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXTextMenu

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "FunctionMenu.h"
#include "FnMenuUpdater.h"
#include "sharedUtil.h"
#include "globals.h"

#if defined CODE_CRUSADER
#include "TextDocument.h"
#include "TextEditor.h"
#endif

#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXTEBase.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FunctionMenu::FunctionMenu
	(
	JXFileDocument*		doc,
	const TextFileType	type,
	JXTEBase*			te,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu(CtagsUser::GetFunctionMenuTitle(type),
			   enclosure, hSizing, vSizing, x,y, w,h)
{
	FunctionMenuX(doc, type, te);
}

FunctionMenu::FunctionMenu
	(
	JXFileDocument*		doc,
	const TextFileType	type,
	JXTEBase*			te,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	FunctionMenuX(doc, type, te);
}

// private

void
FunctionMenu::FunctionMenuX
	(
	JXFileDocument*		doc,
	const TextFileType	type,
	JXTEBase*			te
	)
{
	itsDoc            = doc;
	itsFileType       = kUnknownFT;
	itsTE             = te;
	itsCaretItemIndex = 0;

	itsLineIndexList = jnew JArray<JIndex>(100);
	assert( itsLineIndexList != nullptr );

	itsLineLangList = jnew JArray<Language>(100);
	assert( itsLineLangList != nullptr );

	SetEmptyMenuItems();
	SetUpdateAction(kDisableNone);
	ListenTo(this);

	TextChanged(type, JString::empty);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FunctionMenu::~FunctionMenu()
{
	jdelete itsLineIndexList;
	jdelete itsLineLangList;
}

/******************************************************************************
 TextChanged

 ******************************************************************************/

void
FunctionMenu::TextChanged
	(
	const TextFileType	type,
	const JString&			fileName
	)
{
	itsFileType    = type;
	itsNeedsUpdate = true;

	SetTitle(CtagsUser::GetFunctionMenuTitle(type), nullptr, false);

#ifdef CODE_MEDIC

	itsFileName = fileName;

#endif
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
FunctionMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateMenu();
	}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleSelection(selection->GetIndex());
	}

	else
	{
		JXTextMenu::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateMenu (private)

 ******************************************************************************/

void
FunctionMenu::UpdateMenu()
{
	FnMenuUpdater* updater          = GetFnMenuUpdater();
	const JXKeyModifiers& modifiers = GetDisplay()->GetLatestKeyModifiers();

	bool sort = updater->WillSortFnNames();
	if (modifiers.meta())
	{
		sort = !sort;
	}

	bool includeNS = updater->WillIncludeNamespace();
	if (modifiers.shift())
	{
		includeNS = !includeNS;
	}

	const bool pack = updater->WillPackFnNames();

	if (itsNeedsUpdate || sort != itsSortFlag ||
		includeNS != itsIncludeNSFlag || pack != itsPackFlag)
	{
		#if defined CODE_CRUSADER

		assert( itsDoc != nullptr );

		JString fileName;
		bool deleteFile = false;
		if (itsDoc->NeedsSave())
		{
			itsDoc->SafetySave(JXDocumentManager::kTimer);
			if (!itsDoc->GetSafetySaveFileName(&fileName) &&
				(JCreateTempFile(&fileName)).OK())
			{
				// directory may not be writable

				deleteFile = true;

				// itsDoc can't be TextDocument, because Code Medic uses us

				auto* textDoc = dynamic_cast<TextDocument*>(itsDoc);
				assert( textDoc != nullptr );

				std::ofstream output(fileName.GetBytes());
				textDoc->GetTextEditor()->GetText()->GetText().Print(output);
			}
		}
		else
		{
			bool onDisk;
			fileName = itsDoc->GetFullName(&onDisk);
			if (!onDisk)
			{
				fileName.Clear();
			}
		}

		#elif defined CODE_MEDIC

		const JString& fileName = itsFileName;

		#endif

		if (!fileName.IsEmpty())
		{
			updater->UpdateMenu(fileName, itsFileType, sort, includeNS, pack,
								this, itsLineIndexList, itsLineLangList);
		}
		else
		{
			this->RemoveAllItems();
			itsLineIndexList->RemoveAll();
			itsLineLangList->RemoveAll();
		}

		if (IsEmpty())
		{
			SetEmptyMenuItems();
		}

		itsNeedsUpdate    = false;
		itsSortFlag       = sort;
		itsIncludeNSFlag  = includeNS;
		itsPackFlag       = pack;
		itsCaretItemIndex = 0;						// nothing to remove
		itsTE->GetText()->DeactivateCurrentUndo();	// force another TextChanged

		#if defined CODE_CRUSADER

		if (deleteFile)
		{
			JRemoveFile(fileName);
		}

		#endif
	}

	// mark caret location

	if (!sort && !itsLineIndexList->IsEmpty())
	{
		if (itsCaretItemIndex > 0)
		{
			ShowSeparatorAfter(itsCaretItemIndex, false);
			itsCaretItemIndex = 0;
		}

		const JIndex lineIndex =
			itsTE->VisualLineIndexToCRLineIndex(
						itsTE->GetLineForChar(itsTE->GetInsertionCharIndex()));

		const JSize count = GetItemCount();
		assert( count == itsLineIndexList->GetElementCount() );
		assert( count == itsLineLangList->GetElementCount() );

		for (JIndex i=1; i<=count; i++)
		{
			if (itsLineIndexList->GetElement(i) > lineIndex)
			{
				itsCaretItemIndex = i-1;
				if (itsCaretItemIndex > 0)
				{
					ShowSeparatorAfter(itsCaretItemIndex, true);
				}
				break;
			}
		}
	}
}

/******************************************************************************
 HandleSelection (private)

	ctags can't consider wrapped lines.

 ******************************************************************************/

void
FunctionMenu::HandleSelection
	(
	const JIndex index
	)
{
	JIndexRange r;
	r.first = r.last = itsLineIndexList->GetElement(index);
	SelectLines(itsTE, r);
	ScrollForDefinition(itsTE, itsLineLangList->GetElement(index));
}

/******************************************************************************
 SetEmptyMenuItems (private)

 ******************************************************************************/

void
FunctionMenu::SetEmptyMenuItems()
{
	JString name = CtagsUser::GetFunctionMenuTitle(itsFileType);
	name.ToLower();

	const JUtf8Byte* map[] =
	{
		"name", name.GetBytes()
	};
	const JString menuItems = JGetString("EmptyMenu::FunctionMenu", map, sizeof(map));
	SetMenuItems(menuItems.GetBytes());
}
