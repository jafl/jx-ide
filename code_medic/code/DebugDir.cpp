/******************************************************************************
 DebugDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "DebugDir.h"
#include "TextDisplayBase.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DebugDir::DebugDir()
	:
	JXWindowDirector(JXGetApplication()),
	itsFile("/tmp/code_medic_log")
{
	itsLink = GetLink();
	ListenTo(itsLink);

	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DebugDir::~DebugDir()
{
	if (HasPrefsManager())
	{
		GetPrefsManager()->SaveWindowSize(kDebugWindSizeID, GetWindow());
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
DebugDir::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 450,500, JGetString("WindowTitle::DebugDir::JXLayout"));
	window->SetMinSize(150, 150);
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "Code_Medic_Debug");

	itsCopyButton =
		jnew JXTextButton(JGetString("itsCopyButton::DebugDir::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 450,20);

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 450,480);
	assert( scrollbarSet != nullptr );

	itsText =
		jnew JXStaticText(JString::empty, false, true, true, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 450,480);

// end JXLayout

	window->SetCloseAction(JXWindow::kDeactivateDirector);
	GetPrefsManager()->GetWindowSize(kDebugWindSizeID, window);

	TextDisplayBase::AdjustFont(itsText);

	ListenTo(itsCopyButton);
}

/******************************************************************************
 GetName (virtual)

 ******************************************************************************/

const JString&
DebugDir::GetName()
	const
{
	return JGetString("WindowsMenuText::DebugDir");
}

/******************************************************************************
 Receive

 ******************************************************************************/

static const JString kLogPrefix("=== ");

void
DebugDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink)
	{
		itsText->SetCaretLocation(itsText->GetText()->GetText().GetCharacterCount() + 1);

		if (message.Is(Link::kDebugOutput))
		{
			auto& msg = dynamic_cast<const Link::DebugOutput&>(message);

			const Link::DebugType type = msg.GetType();
			if (type == Link::kCommandType)
			{
				itsText->SetCurrentFontColor(JColorManager::GetLightBlueColor());
				itsText->Paste(itsLink->GetCommandPrompt() + " ");
				itsFile << itsLink->GetCommandPrompt() << " ";
			}
			else if (type == Link::kOutputType)
			{
				itsText->SetCurrentFontColor(JColorManager::GetBlueColor());
			}
			else if (type == Link::kLogType)
			{
				itsText->Paste(kLogPrefix);
				itsFile << kLogPrefix.GetBytes();
			}

			itsText->Paste(msg.GetText());
			itsText->SetCurrentFontColor(JColorManager::GetBlackColor());
			itsText->Paste(JString::newline);

			itsFile << msg.GetText();
			itsFile << std::endl;
		}
		else if (!message.Is(Link::kUserOutput))
		{
			itsText->Paste(kLogPrefix);
			itsText->Paste(JString(message.GetType(), JString::kNoCopy));
			itsText->Paste(JString::newline);

			itsFile << kLogPrefix;
			itsFile << message.GetType();
			itsFile << std::endl;
		}
	}

	else if (sender == itsCopyButton && message.Is(JXButton::kPushed))
	{
		itsText->SelectAll();
		itsText->Copy();
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
DebugDir::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsLink && !IsShuttingDown())
	{
		itsLink = GetLink();
		ListenTo(itsLink);

		itsText->GetText()->SetText(JString::empty);
	}
	else
	{
		JXWindowDirector::ReceiveGoingAway(sender);
	}
}
