/******************************************************************************
 FileNameDisplay.cpp

	BASE CLASS = JXFileInput

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "FileNameDisplay.h"
#include "FileDragSource.h"
#include "TextDocument.h"
#include "util.h"
#include <jx-af/jx/JXMenu.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FileNameDisplay::FileNameDisplay
	(
	TextDocument*		doc,
	FileDragSource*	dragSource,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFileInput(jnew StyledText(this, enclosure->GetFontManager()),
				enclosure, hSizing, vSizing, x,y, w,h)
{
	assert( dragSource != nullptr );	// force ordering of JXLayout code

	itsTE             = nullptr;
	itsUnfocusAction  = kCancel;
	itsDoc            = doc;
	itsDragSource     = dragSource;
	itsDiskModFlag    = false;
	itsCBHasFocusFlag = false;
	SetIsRequired(false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileNameDisplay::~FileNameDisplay()
{
}

/******************************************************************************
 SetTE

	We are created before the text editor so we can't get the pointer
	in our constructor.

 ******************************************************************************/

void
FileNameDisplay::SetTE
	(
	JXTEBase* te
	)
{
	itsTE = te;

	// this re-orders the list so text editor gets focus

	WantInput(false);
	WantInput(true, WantsTab(), true);
}

/******************************************************************************
 DiskCopyIsModified

 ******************************************************************************/

void
FileNameDisplay::DiskCopyIsModified
	(
	const bool mod
	)
{
	itsDiskModFlag = mod;
	UpdateDisplay(HasFocus());
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
FileNameDisplay::UpdateDisplay
	(
	const bool hasFocus
	)
{
	itsCBHasFocusFlag = hasFocus;
	if (!hasFocus && itsDiskModFlag)
	{
		SetFontStyle(JColorManager::GetDarkRedColor());
	}
	else if (!hasFocus)
	{
		SetFontStyle(JColorManager::GetBlackColor());
	}
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
FileNameDisplay::HandleFocusEvent()
{
	UpdateDisplay(true);					// allow JXFileInput to control text style
	itsDragSource->ProvideDirectSave(this);

	itsOrigFile = GetText()->GetText();
	GetText()->SetText(itsOrigFile);		// force recalc of styles

	JXFileInput::HandleFocusEvent();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
FileNameDisplay::HandleUnfocusEvent()
{
	JXFileInput::HandleUnfocusEvent();

	bool saved = false;

	JString fullName;
	if (itsUnfocusAction != kCancel &&
		JExpandHomeDirShortcut(GetText()->GetText(), &fullName))
	{
		if (JIsRelativePath(fullName) &&
			!JGetChooseSaveFile()->SaveFile(JGetString("SaveAsPrompt::FileNameDisplay"), JString::empty, fullName, &fullName))
		{
			fullName.Clear();
		}

		if (!fullName.IsEmpty() &&
			!JSameDirEntry(fullName, itsOrigFile) &&
			itsDoc->SaveInNewFile(fullName))
		{
			saved = true;

			bool onDisk;
			fullName = itsDoc->GetFullName(&onDisk);
			GetText()->SetText(fullName);

			if (itsUnfocusAction == kRename)
			{
				CleanUpAfterRename(itsOrigFile, fullName);
			}
		}
	}

	if (!saved)
	{
		GetText()->SetText(itsOrigFile);
	}

	UpdateDisplay(false);			// take control of text style
	itsDragSource->ProvideDirectSave(nullptr);
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

bool
FileNameDisplay::InputValid()
{
	const JString& text = GetText()->GetText();
	if (itsUnfocusAction == kCancel)
	{
		return true;
	}
	else if (text.IsEmpty())
	{
		JGetUserNotification()->ReportError(JGetString("EmptyError::FileNameDisplay"));
		return false;
	}
	else if (text.EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
	{
		JGetUserNotification()->ReportError(JGetString("NoFileName::FileNameDisplay"));
		return false;
	}
	else if (JIsRelativePath(text))
	{
		return true;	// open Choose File dialog
	}

	JString path, fullPath, name;
	JSplitPathAndName(text, &path, &name);
	if (!JExpandHomeDirShortcut(path, &fullPath))
	{
		return false;
	}

	const JString fullName = JCombinePathAndName(fullPath, name);
	bool onDisk;
	const JString origFullName = itsDoc->GetFullName(&onDisk);
	if (onDisk && JSameDirEntry(origFullName, fullName))
	{
		itsUnfocusAction = kCancel;
		return true;
	}

	if (JFileExists(fullName))
	{
		const JUtf8Byte* map[] =
		{
			"f", text.GetBytes()
		};
		const JString msg = JGetString("OKToReplace::FileNameDisplay", map, sizeof(map));
		if (!JGetUserNotification()->AskUserNo(msg))
		{
			return false;
		}
	}

	if (!JDirectoryExists(fullPath))
	{
		const JError err = JCreateDirectory(fullPath);
		if (!err.OK())
		{
			err.ReportIfError();
			return false;
		}
	}
	else if (!JDirectoryWritable(fullPath))
	{
		const JError err = JAccessDenied(fullPath);
		err.ReportIfError();
		return false;
	}

	if (itsUnfocusAction == kRename)
	{
		const JError err = JRenameVCS(origFullName, fullName);
		err.ReportIfError();
		return err.OK();
	}
	else
	{
		return true;
	}
}

/******************************************************************************
 HandleKeyPress (virtual)

	Escape cancels the changes.
	Return and tab switch focus to the text editor.
	Ctrl-tab is handled by the text editor.

 ******************************************************************************/

void
FileNameDisplay::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJEscapeKey)
	{
		itsTE->Focus();
	}
	else if (c == '\t' &&
			 !modifiers.GetState(kJXMetaKeyIndex)   &&
			 modifiers.GetState(kJXControlKeyIndex) &&
			 !modifiers.shift())
	{
		itsTE->Focus();
		itsTE->HandleKeyPress(c, keySym, modifiers);
	}
	else if ((c == '\r' || c == '\n') &&
			 !modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex)) &&
			 !modifiers.shift())
	{
		itsUnfocusAction =
			modifiers.GetState(JXMenu::AdjustNMShortcutModifier(kJXControlKeyIndex)) ?
			kRename : kSaveAs;
		itsTE->Focus();				// trigger HandleUnfocusEvent()
		itsUnfocusAction = kCancel;
	}
	else
	{
		JXFileInput::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Don't let JXFileInput adjust the styles unless we have focus.

 ******************************************************************************/

void
FileNameDisplay::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const bool			deletion
	)
{
	if (itsCBField->itsCBHasFocusFlag)
	{
		JXFileInput::StyledText::AdjustStylesBeforeBroadcast(
			text, styles, recalcRange, redrawRange, deletion);
	}
}
