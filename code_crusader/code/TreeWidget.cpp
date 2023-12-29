/******************************************************************************
 TreeWidget.cpp

	BASE CLASS = JXScrollableWidget

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#include "TreeWidget.h"
#include "Tree.h"
#include "Class.h"
#include "ProjectDocument.h"
#include "SymbolDirector.h"
#include "SymbolList.h"
#include "TreeDirector.h"
#include "EditSearchPathsDialog.h"
#include "globals.h"

#include <jx-af/jx/JXDNDManager.h>
#include <jx-af/jx/JXFileSelection.h>
#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXScrollbar.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXImageCache.h>
#include <jx-af/jx/jXUtil.h>

#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/JEPSPrinter.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jASCIIConstants.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jMouseUtil.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jAssert.h>

bool TreeWidget::itsRaiseWhenSingleMatchFlag = false;
const JSize TreeWidget::kBorderWidth         = 5;

static const JUtf8Byte* kSelectionDataID = "TreeWidget";

/******************************************************************************
 Constructor

 ******************************************************************************/

TreeWidget::TreeWidget
	(
	TreeDirector*		director,
	Tree*				tree,
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
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h)
{
	Class::SetGhostNameColor(JColorManager::GetBlackColor());

	itsDirector = director;
	itsTree     = tree;

	WantInput(true, false, true);	// need Ctrl-Tab

	const JColorID gray75Color = JColorManager::GetGrayColor(75);
	SetBackColor(gray75Color);
	SetFocusColor(gray75Color);

	SetVertStepSize(Class::GetTotalHeight(itsTree, GetFontManager()));

	ListenTo(itsTree);

	JCoordinate w1,h1;
	itsTree->GetBounds(&w1, &h1);
	SetBounds(w1, h1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TreeWidget::~TreeWidget()
{
}

/******************************************************************************
 FindClass

	Searches for classes with the given name, not full name.

	If there is a single match, button determines what to open:

		kJXLeftButton:   source
		kJXMiddleButton: header
		kJXRightButton:  nothing

	If raiseTreeWindow, the Tree window is raised -before- opening the source
	or header file.

 ******************************************************************************/

bool
TreeWidget::FindClass
	(
	const JString&		name,
	const JXMouseButton	button,
	const bool			raiseTreeWindow,
	const bool			reportNotFound,
	const bool			openFileIfSingleMatch,
	const bool			deselectAll
	)
	const
{
	itsTree->SelectClasses(name, deselectAll);

	return ShowSearchResults([button](Class* c)
	{
		if (button == kJXLeftButton)
		{
			c->ViewSource();
		}
		else if (button == kJXMiddleButton)
		{
			c->ViewHeader();
		}
	},
	button, raiseTreeWindow, reportNotFound, openFileIfSingleMatch,
	"ClassNotFound::TreeWidget");
}

/******************************************************************************
 FindFunction

	Searches for classes that implement the given function.

	If there is a single match, button determines what to open:

		kJXLeftButton:   definition
		kJXMiddleButton: declaration
		kJXRightButton:  nothing

	If raiseTreeWindow, the Tree window is raised -before- opening the source
	or header file.

 ******************************************************************************/

bool
TreeWidget::FindFunction
	(
	const JString&		fnName,
	const JXMouseButton	button,
	const bool			raiseTreeWindow,
	const bool			reportNotFound,
	const bool			openFileIfSingleMatch,
	const bool			deselectAll
	)
	const
{
	auto* symbolList = itsDirector->GetProjectDoc()->GetSymbolDirector()->GetSymbolList();

	JArray<JIndex> matchList;
	const bool found =
		symbolList->FindSymbol(
			fnName, JFAID::kInvalidID, JArray<SymbolList::ContextNamespace>(),
			button == kJXMiddleButton || button == kJXRightButton,
			button == kJXLeftButton   || button == kJXRightButton,
			&matchList);

	if (found)
	{
		return FindFunction(matchList, button, raiseTreeWindow,
							reportNotFound, openFileIfSingleMatch, deselectAll);
	}
	else
	{
		if (reportNotFound)
		{
			JGetUserNotification()->ReportError(
				JGetString("FunctionNotFound::TreeWidget"));
		}
		return false;
	}
}

// optimize for SymbolDirector's call

bool
TreeWidget::FindFunction
	(
	const JArray<JIndex>&	matchList,
	const JXMouseButton		button,
	const bool				raiseTreeWindow,
	const bool				reportNotFound,
	const bool				openFileIfSingleMatch,
	const bool				deselectAll
	)
	const
{
	auto* symbolList = itsDirector->GetProjectDoc()->GetSymbolDirector()->GetSymbolList();

	if (deselectAll)
	{
		itsTree->DeselectAll();
	}

	for (const JIndex i : matchList)
	{
		Language lang;
		CtagsUser::Type type;
		JString s = symbolList->GetSymbol(i, &lang, &type);

		if (HasNamespace(lang))
		{
			JStringIterator iter(&s, JStringIterator::kStartAtEnd);
			if (iter.Prev(GetNamespaceOperator(lang)))
			{
				iter.RemoveAllNext();
			}
		}

		itsTree->SelectClasses(s, false, true);
	}

	return ShowSearchResults([this, matchList](Class*)
	{
		if (matchList.GetItemCount() == 1)
		{
			itsDirector->GetProjectDoc()->GetSymbolDirector()->ViewSymbol(matchList.GetFirstItem());
		}
	},
	button, raiseTreeWindow, reportNotFound, openFileIfSingleMatch,
	"FunctionNotFound::TreeWidget");
}

/******************************************************************************
 ShowSearchResults (private)

	If raiseTreeWindow, the Tree window is raised -before- opening the source
	or header file.

 ******************************************************************************/

bool
TreeWidget::ShowSearchResults
	(
	const std::function<void(Class*)>&	singleResultAction,

	const JXMouseButton	button,
	const bool			raiseTreeWindow,
	const bool			reportNotFound,
	const bool			openFileIfSingleMatch,
	const JUtf8Byte*	notFoundID
	)
	const
{
	JRect selRect;
	JSize selCount;
	if (itsTree->GetSelectionCoverage(&selRect, &selCount))
	{
		const_cast<TreeWidget*>(this)->ScrollToRectCentered(selRect, true);

		if (raiseTreeWindow &&
			(selCount > 1 || button == kJXRightButton ||
			 itsRaiseWhenSingleMatchFlag))
		{
			GetWindow()->GetDirector()->Activate();
		}

		if (openFileIfSingleMatch && button != kJXRightButton)
		{
			JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
			const bool ok = itsTree->GetSelectedClasses(&classList);
			assert( ok );
			if (classList.GetItemCount() == 1)
			{
				singleResultAction(classList.GetFirstItem());
			}
		}

		return true;
	}
	else
	{
		if (reportNotFound)
		{
			JGetUserNotification()->ReportError(JGetString(notFoundID));
		}
		return false;
	}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
TreeWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	itsTree->Draw(p, rect);
}

/*****************************************************************************
 Print

	Since Tree::UpdatePlacement() quantizes the y-coordinate of each class,
	we never have to worry that we might cut the frame of an ancestor in half
	at the bottom of the page.

 ******************************************************************************/

void
TreeWidget::Print
	(
	JPagePrinter& p
	)
{
	if (!itsTree->IsEmpty() && p.OpenDocument())
	{
		if (p.WillPrintBlackWhite())
		{
			Class::SetGhostNameColor(JColorManager::GetWhiteColor());
		}

		const JCoordinate lineHeight   = itsTree->GetLineHeight();
		const JCoordinate footerHeight = JRound(1.5 * p.GetLineHeight());

		const JRect bounds    = GetBounds();
		const JSize lineCount = (bounds.height() - 2*kBorderWidth) / lineHeight;

		const JRect pageRect     = p.GetPageRect();
		const JSize linesPerPage = (pageRect.height() - footerHeight) / lineHeight;
		const JSize drawHeight   = linesPerPage * lineHeight;
		JSize pageCount          = lineCount / linesPerPage;
		if (lineCount % linesPerPage != 0)
		{
			pageCount++;
		}

		bool cancelled = false;
		for (JIndex i=1; i<=pageCount; i++)
		{
			if (!p.NewPage())
			{
				cancelled = true;
				break;
			}

			const JString pageNumberStr = "Page " + JString((JUInt64) i);
			p.String(pageRect.left, pageRect.bottom - footerHeight, pageNumberStr,
					 pageRect.width(), JPainter::HAlign::kCenter,
					 footerHeight, JPainter::VAlign::kBottom);
			p.LockFooter(pageRect.height() - drawHeight);

			p.SetPenColor(JColorManager::GetYellowColor());
			p.Rect(p.GetPageRect());
			p.SetPenColor(JColorManager::GetBlackColor());

			const JCoordinate top = (i-1)*drawHeight;
			p.ShiftOrigin(0, 1-kBorderWidth-top);

			const JRect drawRect(top, pageRect.left, top + drawHeight, pageRect.right);
			itsTree->Draw(p, drawRect);
		}

		if (!cancelled)
		{
			p.CloseDocument();
		}

		Class::SetGhostNameColor(JColorManager::GetBlackColor());
	}
}

/*****************************************************************************
 Print

 ******************************************************************************/

void
TreeWidget::Print
	(
	JEPSPrinter& p
	)
{
	if (itsTree->IsEmpty())
	{
		return;
	}

	if (p.PSWillPrintBlackWhite())
	{
		Class::SetGhostNameColor(JColorManager::GetWhiteColor());
	}

	const JRect bounds = GetBounds();

	if (p.WantsPreview())
	{
		JPainter& p1 = p.GetPreviewPainter(bounds);
		itsTree->Draw(p1, bounds);
	}

	if (p.OpenDocument(bounds))
	{
		itsTree->Draw(p, bounds);
		p.CloseDocument();
	}

	Class::SetGhostNameColor(JColorManager::GetBlackColor());
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
TreeWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	GetDocumentManager()->SetActiveProjectDocument(itsDirector->GetProjectDoc());

	itsKeyBuffer.Clear();
	itsExpectDragFlag = false;

	Class* theClass = nullptr;
	if (ScrollForWheel(button, modifiers))
	{
		return;
	}

	else if (itsTree->GetClass(pt, &theClass))
	{
		if (modifiers.shift())
		{
			theClass->ToggleSelected();
		}
		else if (clickCount == 1)
		{
			if (!theClass->IsSelected())
			{
				itsTree->DeselectAll();
				theClass->SetSelected(true);
			}
			itsExpectDragFlag = true;
			itsStartPt        = pt;
		}
		else if (button == kJXLeftButton && clickCount == 2)
		{
			theClass->ViewSource();
		}
		else if (button == kJXMiddleButton && clickCount == 2)
		{
			theClass->ViewHeader();
		}
		else if (button == kJXRightButton && clickCount == 2 &&
				 !theClass->IsGhost())
		{
			itsDirector->ViewFunctionList(theClass);
		}
		else if (button == kJXRightButton && clickCount == 2)
		{
			JGetUserNotification()->ReportError(JGetString("NoGhostFile::TreeWidget"));
		}
	}

	else if (!modifiers.shift())
	{
		itsTree->DeselectAll();
	}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
TreeWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsExpectDragFlag && JMouseMoved(itsStartPt, pt))
	{
		auto* data = jnew JXFileSelection(this, kSelectionDataID);
		BeginDND(pt, buttonStates, modifiers, data);
		itsExpectDragFlag = false;
	}
}

/******************************************************************************
 HitSamePart (virtual protected)

 ******************************************************************************/

bool
TreeWidget::HitSamePart
	(
	const JPoint& pt1,
	const JPoint& pt2
	)
	const
{
	Class* theClass = nullptr;
	return itsTree->HitSameClass(pt1, pt2, &theClass);
}

/******************************************************************************
 GetSelectionData (virtual protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
TreeWidget::GetSelectionData
	(
	JXSelectionData*	data,
	const JString&		id
	)
{
	if (id == kSelectionDataID)
	{
		auto* fileData = dynamic_cast<JXFileSelection*>(data);
		assert( fileData != nullptr );

		DocumentManager* docMgr = GetDocumentManager();

		auto* list = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

		JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
		const bool hasSelection = itsTree->GetSelectedClasses(&classList);
		assert( hasSelection );

		const JSize classCount = classList.GetItemCount();
		JString headerName, sourceName;
		for (JIndex i=1; i<=classCount; i++)
		{
			Class* c = classList.GetItem(i);
			if (c->GetFileName(&headerName))
			{
				auto* s = jnew JString(headerName);
				list->Append(s);

				if (docMgr->GetComplementFile(headerName, itsTree->GetFileType(),
											  &sourceName, itsDirector->GetProjectDoc()))
				{
					s = jnew JString(sourceName);
					list->Append(s);
				}
			}
		}

		fileData->SetData(list);
	}
	else
	{
		JXScrollableWidget::GetSelectionData(data, id);
	}
}

/******************************************************************************
 GetDNDAction (virtual protected)

 ******************************************************************************/

Atom
TreeWidget::GetDNDAction
	(
	const JXContainer*		target,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	return GetDNDManager()->GetDNDActionPrivateXAtom();
}

/******************************************************************************
 HandleDNDResponse (virtual protected)

 ******************************************************************************/

void
TreeWidget::HandleDNDResponse
	(
	const JXContainer*	target,
	const bool		dropAccepted,
	const Atom			action
	)
{
	DisplayCursor(GetDNDManager()->GetDNDFileCursor(dropAccepted, action));
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept text/uri-list.

 ******************************************************************************/

bool
TreeWidget::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	// dropping on ourselves makes no sense since we don't accept files

	if (this == const_cast<JXWidget*>(source))
	{
		return false;
	}

	// we accept drops of type text/uri-list

	const Atom urlXAtom = GetSelectionManager()->GetURLXAtom();

	const JSize typeCount = typeList.GetItemCount();
	for (JIndex i=1; i<=typeCount; i++)
	{
		const Atom a = typeList.GetItem(i);
		if (a == urlXAtom)
		{
			*action = GetDNDManager()->GetDNDActionPrivateXAtom();
			return true;
		}
	}

	return false;
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept text/uri-list, we don't bother to check typeList.

 ******************************************************************************/

void
TreeWidget::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (!selMgr->GetData(GetDNDManager()->GetDNDSelectionName(),
						 time, selMgr->GetURLXAtom(),
						 &returnType, &data, &dataLength, &delMethod))
	{
		return;
	}

	if (returnType != selMgr->GetURLXAtom())
	{
		selMgr->DeleteData(&data, delMethod);
		return;
	}

	JPtrArray<JString> dirList(JPtrArrayT::kDeleteAll),
					   urlList(JPtrArrayT::kDeleteAll);
	JXUnpackFileNames((char*) data, dataLength, &dirList, &urlList);

	const JSize fileCount = dirList.GetItemCount();
	for (JIndex i=fileCount; i>=1; i--)
	{
		const JString* name = dirList.GetItem(i);
		if (!JDirectoryExists(*name))
		{
			dirList.DeleteItem(i);
		}
	}

	if (dirList.IsEmpty() && urlList.IsEmpty())
	{
		JGetUserNotification()->ReportError(
			JGetString("OnlyDropFolders::TreeWidget"));
	}
	else if (dirList.IsEmpty())
	{
		JXReportUnreachableHosts(urlList);
	}
	else
	{
		itsDirector->GetProjectDoc()->EditSearchPaths(&dirList);
	}

	selMgr->DeleteData(&data, delMethod);
}

/******************************************************************************
 HandleFocusEvent (virtual protected)

 ******************************************************************************/

void
TreeWidget::HandleFocusEvent()
{
	JXScrollableWidget::HandleFocusEvent();
	itsKeyBuffer.Clear();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
TreeWidget::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	GetDocumentManager()->SetActiveProjectDocument(itsDirector->GetProjectDoc());

	// open source

	if (c == kJReturnKey)
	{
		itsKeyBuffer.Clear();
		itsTree->ViewSelectedSources();
	}

	// open header

	else if (c == '\t' &&
			 !modifiers.GetState(kJXMetaKeyIndex)   &&
			 modifiers.GetState(kJXControlKeyIndex) &&
			 !modifiers.shift())
	{
		itsKeyBuffer.Clear();
		itsTree->ViewSelectedHeaders();
	}

	// incremental search for class name

	else if (c == ' ')
	{
		itsKeyBuffer.Clear();
	}
	else if (!itsTree->IsEmpty() && c.IsPrint() &&
			 !modifiers.meta() && !modifiers.control())
	{
		itsKeyBuffer.Append(c);

		Class* selClass = nullptr;
		if (itsTree->ClosestVisibleMatch(itsKeyBuffer, &selClass))
		{
			itsTree->DeselectAll();
			selClass->SetSelected(true);
			ScrollToRectCentered(selClass->GetFrame(), true);
		}
	}

	else
	{
		JXScrollableWidget::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
TreeWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTree && message.Is(Tree::kBoundsChanged))
	{
		JCoordinate w,h;
		itsTree->GetBounds(&w, &h);
		SetBounds(w, h);
		Refresh();
	}
	else if (sender == itsTree && message.Is(Tree::kPrepareForParse))
	{
		Hide();
	}
	else if (sender == itsTree && message.Is(Tree::kParseFinished))
	{
		Show();
	}

	else if (sender == itsTree && message.Is(Tree::kFontSizeChanged))
	{
		const auto* info =
			dynamic_cast<const Tree::FontSizeChanged*>(&message);
		assert( info != nullptr );

		JXScrollbar *hScrollbar, *vScrollbar;
		const bool ok = GetScrollbars(&hScrollbar, &vScrollbar);
		assert( ok );
		vScrollbar->PrepareForScaledMaxValue(info->GetVertScaleFactor());

		SetVertStepSize(Class::GetTotalHeight(itsTree, GetFontManager()));
	}

	else if (sender == itsTree && message.Is(Tree::kClassSelected))
	{
		const auto* selection =
			dynamic_cast<const Tree::ClassSelected*>(&message);
		assert( selection != nullptr );
		RefreshRect((selection->GetClass())->GetFrame());
	}
	else if (sender == itsTree && message.Is(Tree::kClassDeselected))
	{
		const auto* selection =
			dynamic_cast<const Tree::ClassDeselected*>(&message);
		assert( selection != nullptr );
		RefreshRect((selection->GetClass())->GetFrame());
	}
	else if (sender == itsTree && message.Is(Tree::kAllClassesDeselected))
	{
		Refresh();
	}

	else if (sender == itsTree && message.Is(Tree::kNeedsRefresh))
	{
		Refresh();
	}

	else if (sender == itsTree && message.Is(Tree::kChanged))
	{
		const JRect ap = GetAperture();

		JPtrArray<Class> sel(JPtrArrayT::kForgetAll);
		Class* closest = nullptr;
		JCoordinate miny = 0;
		if (itsTree->GetSelectedClasses(&sel))
		{
			bool visible  = false;
			const JSize count = sel.GetItemCount();
			for (JIndex i=1; i<=count; i++)
			{
				Class* c    = sel.GetItem(i);
				const JRect r = c->GetFrame();
				if (ap.Contains(r))
				{
					visible = true;
					break;
				}
				else if (ap.bottom < r.bottom &&
						 (closest == nullptr || r.top - ap.bottom < miny))
				{
					miny    = r.top - ap.bottom;
					closest = c;
				}
				else if (r.bottom < ap.top &&
						 (closest == nullptr || ap.top - r.bottom < miny))
				{
					miny    = ap.top - r.bottom;
					closest = c;
				}
			}

			if (!visible && closest != nullptr)
			{
				ScrollToRect(closest->GetFrame());
			}
		}
	}

	else
	{
		JXScrollableWidget::Receive(sender, message);
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
TreeWidget::ReadSetup
	(
	std::istream&			setInput,
	const JFileVersion	setVers
	)
{
	if (10 <= setVers && setVers <= 21)
	{
		JPoint scrollPt;
		setInput >> scrollPt;
		ScrollTo(scrollPt);
	}

	if (setVers >= 22)
	{
		ReadScrollSetup(setInput);
	}
	else if (setVers >= 18)
	{
		JXScrollbar *hScrollbar, *vScrollbar;
		const bool ok = GetScrollbars(&hScrollbar, &vScrollbar);
		assert( ok );
		hScrollbar->ReadSetup(setInput);
		vScrollbar->ReadSetup(setInput);
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
TreeWidget::WriteSetup
	(
	std::ostream& setOutput
	)
	const
{
	setOutput << ' ';
	WriteScrollSetup(setOutput);
	setOutput << ' ';
}
