/******************************************************************************
 TEScriptMenu.cpp

	Appends items to JXFSDirMenu.

	BASE CLASS = JXFSDirMenu

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#include "TEScriptMenu.h"
#include "TextEditor.h"
#include "globals.h"
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kMenuSuffixStr =
	"  New script"
	"| Open directory";

enum	// in reverse order since from end of menu
{
	kOpenDirectoryCmdOffset = 0,
	kNewScriptCmdOffset
};

/******************************************************************************
 Constructor

 *****************************************************************************/

TEScriptMenu::TEScriptMenu
	(
	TextEditor*		te,
	const JString&		path,
	const JString&		title,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFSDirMenu(path, title, enclosure, hSizing, vSizing, x, y, w, h),
	itsTE(te)
{
	TEScriptMenuX();
}

TEScriptMenu::TEScriptMenu
	(
	TextEditor*	te,
	const JString&	path,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXFSDirMenu(path, owner, itemIndex, enclosure),
	itsTE(te)
{
	TEScriptMenuX();
}

// private

void
TEScriptMenu::TEScriptMenuX()
{
	JDirInfo* info;
	if (GetDirInfo(&info))
	{
		info->SetWildcardFilter(JString("*~ #*#", JString::kNoCopy), true);
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

TEScriptMenu::~TEScriptMenu()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
TEScriptMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
	{
		if (!UpdateSelf())
		{
			return;
		}
	}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		const auto* info =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert(info != nullptr);
		if (HandleSelection(info->GetIndex()))
		{
			return;
		}
	}

	JXFSDirMenu::Receive(sender, message);

	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
	{
		ShowSeparatorAfter(GetItemCount());
		AppendMenuItems(kMenuSuffixStr);
	}
}

/******************************************************************************
 UpdateSelf (private)

 ******************************************************************************/

bool
TEScriptMenu::UpdateSelf()
{
	JDirInfo* info;
	if (GetDirInfo(&info) &&
		(info->GoTo(itsTE->GetScriptPath())).OK())
	{
		info->SetDirEntryFilter(ShowExecutables);
		info->ForceUpdate();	// permissions may change
		return true;
	}
	else
	{
		ClearMenu();
		return false;
	}
}

/******************************************************************************
 ShowExecutables (static private)

 ******************************************************************************/

bool
TEScriptMenu::ShowExecutables
	(
	const JDirEntry& entry
	)
{
	return (entry.IsDirectory() && !JIsVCSDirectory(entry.GetName())) ||
				 (entry.IsFile() && entry.IsExecutable());
}

/******************************************************************************
 HandleSelection (private)

 ******************************************************************************/

bool
TEScriptMenu::HandleSelection
	(
	const JIndex index
	)
{
	if (index == GetItemCount() - kNewScriptCmdOffset)
	{
		JDirInfo* info;
		if (GetDirInfo(&info))
		{
			JString origName = info->GetDirectory();
			origName         = JCombinePathAndName(origName, JString("script", JString::kNoCopy));

			JString fullName;
			if (JGetChooseSaveFile()->SaveFile(JGetString("SavePrompt::TEScriptMenu"), JString::empty,
												 origName, &fullName))
			{
				std::ofstream output(fullName.GetBytes());
				output.close();
				JSetPermissions(fullName, 0744);
				GetDocumentManager()->OpenTextDocument(fullName);
			}
		}
		return true;
	}
	else if (index == GetItemCount() - kOpenDirectoryCmdOffset)
	{
		JDirInfo* info;
		if (GetDirInfo(&info))
		{
			(JXGetWebBrowser())->ShowFileLocation(info->GetDirectory());
		}
		return true;
	}
	else
	{
		return false;
	}
}
