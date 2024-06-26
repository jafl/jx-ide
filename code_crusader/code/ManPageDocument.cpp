/******************************************************************************
 ManPageDocument.cpp

	BASE CLASS = TextDocument

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "ManPageDocument.h"
#include "TextEditor.h"
#include "ViewManPageDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jcore/JProcess.h>
#include <jx-af/jcore/jTextUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/jAssert.h>

JPtrArray<JString>		ManPageDocument::theManCmdList(JPtrArrayT::kDeleteAll);
JPtrArray<TextDocument>	ManPageDocument::theManDocList(JPtrArrayT::kForgetAll);

/******************************************************************************
 Constructor function (static)

	First searches the open man pages to see if the requested one already
	exists.  If not, creates a new one and activates it.

	returnDoc can be nullptr.

 ******************************************************************************/

bool
ManPageDocument::Create
	(
	ManPageDocument**	returnDoc,
	const JString&		origPageName,
	const JString&		pageIndex,
	const bool			apropos
	)
{
	JString pageName = origPageName;
	pageName.TrimWhitespace();
	if (pageName.IsEmpty())
	{
		if (returnDoc != nullptr)
		{
			*returnDoc = nullptr;
		}
		return false;
	}

	GetViewManPageDialog()->AddToHistory(pageName, pageIndex, apropos);

	// check if the man page is already open

	theManCmdList.SetCompareFunction(JCompareStringsCaseSensitive);

	JString cmd;
	if (apropos)
	{
		cmd = GetCmd2(pageName);
	}
	else
	{
		cmd = GetCmd1(pageName, pageIndex);
	}

	JIndex i;
	if (theManCmdList.SearchSorted(&cmd, JListT::kAnyMatch, &i))
	{
		auto* doc = dynamic_cast<ManPageDocument*>(theManDocList.GetItem(i));
		assert( doc != nullptr );
		doc->Activate();
		if (returnDoc != nullptr)
		{
			*returnDoc = doc;
		}
		return true;
	}

	// If !apropos, we can't check cmd #2 until cmd #1 fails.

	// create a new one

	ManPageDocument* trueDoc;
	auto* doc = jnew ManPageDocument(pageName, pageIndex, apropos, &trueDoc);

	if (doc != trueDoc)
	{
		const bool ok = doc->Close();
		assert( ok );
	}

	if (returnDoc != nullptr)
	{
		*returnDoc = trueDoc;
	}

	if (trueDoc != nullptr)
	{
		trueDoc->Activate();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Constructor (protected)

	If the result is already displayed in a different document, we
	return that in *trueDoc.  If there is nothing relevant, *trueDoc == nullptr.
	Otherwise, *trueDoc == this.

 ******************************************************************************/

ManPageDocument::ManPageDocument
	(
	const JString&		pageName,
	const JString&		pageIndex,
	const bool			apropos,
	ManPageDocument**	trueDoc
	)
	:
	TextDocument(kManPageFT, "ManPageHelp")
{
	theManCmdList.SetCompareFunction(JCompareStringsCaseSensitive);

	*trueDoc = nullptr;

	auto* cmd = jnew JString;

	bool success = false;
	if (!apropos)
	{
		*cmd = GetCmd1(pageName, pageIndex);

		// Create() already searched theManCmdList

		JProcess* p;
		int fromFD;
		const JError execErr =
			JProcess::Create(&p, *cmd,
							 kJIgnoreConnection, nullptr,
							 kJCreatePipe, &fromFD, kJTossOutput);
		if (!execErr.OK())
		{
			execErr.ReportIfError();
			*trueDoc = this;
			return;
		}
		else
		{
			std::ifstream input;
			JString tempName;
			if (JConvertToStream(fromFD, &input, &tempName))
			{
				JReadUNIXManOutput(input, GetTextEditor()->GetText());
				p->WaitUntilFinished();
				success = p->SuccessfulFinish();

				input.close();
				JRemoveFile(tempName);
			}

			jdelete p;
		}
	}

	if (!success)
	{
		*cmd = GetCmd2(pageName);

		JIndex i;
		if (theManCmdList.SearchSorted(cmd, JListT::kAnyMatch, &i))
		{
			auto* doc = dynamic_cast<ManPageDocument*>(theManDocList.GetItem(i));
			assert( doc != nullptr );
			*trueDoc = doc;
			return;
		}

		JProcess* p;
		int fromFD;
		const JError execErr =
			JProcess::Create(&p, *cmd,
							 kJIgnoreConnection, nullptr,
							 kJCreatePipe, &fromFD, kJTossOutput);
		if (!execErr.OK())
		{
			execErr.ReportIfError();
			*trueDoc = this;
			return;
		}
		else
		{
			std::ifstream input;
			JString tempName, text;
			if (JConvertToStream(fromFD, &input, &tempName))
			{
				input >> std::ws;
				text.SetMinLgSize(12);
				while (!input.eof() && !input.fail())
				{
					text += JReadLine(input);
					text += "\n\n";
					input >> std::ws;
				}
				text.TrimWhitespace();

				input.close();
				JRemoveFile(tempName);
			}

			// We can't check the return value of "man -k" because it is
			// always 1.  We can't use "apropos" either because it always
			// returns 0.

			jdelete p;
			GetTextEditor()->GetText()->SetText(text);
		}
	}

	itsIgnoreNameChangedFlag = true;
	FileChanged(*cmd, false);
	itsIgnoreNameChangedFlag = false;

	GetTextEditor()->SetWritable(false);

	*trueDoc = this;

	JIndex index;
	theManCmdList.InsertSorted(cmd, true, &index);
	theManDocList.InsertAtIndex(index, this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ManPageDocument::~ManPageDocument()
{
	RemoveFromManPageList(this);
}

/******************************************************************************
 Receive (protected)

	Listen for calls to FileChanged().

 ******************************************************************************/

void
ManPageDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXFileDocument::kNameChanged) &&
		!itsIgnoreNameChangedFlag)
	{
		RemoveFromManPageList(this);
	}

	TextDocument::Receive(sender, message);
}

/******************************************************************************
 RemoveFromManPageList (static private)

 ******************************************************************************/

void
ManPageDocument::RemoveFromManPageList
	(
	ManPageDocument* doc
	)
{
	JIndex i;
	if (theManDocList.Find(doc, &i))
	{
		theManCmdList.DeleteItem(i);
		theManDocList.RemoveItem(i);
	}
}

/******************************************************************************
 GetCmd1 (static protected)

 ******************************************************************************/

JString
ManPageDocument::GetCmd1
	(
	const JString&	pageName,
	const JString&	pageIndex
	)
{
	JString cmd("man ");
	if (!pageIndex.IsEmpty())
	{
		cmd += JPrepArgForExec(pageIndex);
		cmd += " ";
	}
	cmd += JPrepArgForExec(pageName);
	return cmd;
}

/******************************************************************************
 GetCmd2 (static protected)

 ******************************************************************************/

JString
ManPageDocument::GetCmd2
	(
	const JString& pageName
	)
{
#ifdef _J_MACOS
	JString cmd("apropos ");	// need to replace apropos, because default is broken
#else
	JString cmd("man -k ");
#endif
	cmd += JPrepArgForExec(pageName);
	return cmd;
}
