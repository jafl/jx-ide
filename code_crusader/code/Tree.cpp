/******************************************************************************
 Tree.cpp

	This class encapsulates the inheritance tree of a set of Classes.

	Derived classes must implement:

		ViewSelectedSources
			Open the files containing the definitions of the selected classes.

		ViewSelectedHeaders
			Open the files containing the declarations of the selected classes.

	Derived classes can override:

		StreamOut
			Write out extra data after calling Tree::StreamOut().

	BASE CLASS = JContainer

	Copyright © 1995-99 John Lindal.

 ******************************************************************************/

#include "Tree.h"
#include "Class.h"
#include "TreeDirector.h"
#include "SymbolUpdatePG.h"
#include "DirList.h"
#include "globals.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXFontManager.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXTextSelection.h>
#include <jx-af/jcore/JPainter.h>
#include <jx-af/jcore/JLatentPG.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JAliasArray.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JSimpleProcess.h>
#include <jx-af/jcore/JThreadPG.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <boost/fiber/operations.hpp>
#include <jx-af/jcore/jAssert.h>

const JSize kBlockSize = 1024;

// Code Mill info

static const JString kCodeMillProgramName(CODE_MILL_BINARY, JString::kNoCopy);
#ifndef CODE_CRUSADER_UNIT_TEST
const JFileVersion kCodeMillDataVersion = 0;
#endif

// JBroadcaster message types

const JUtf8Byte* Tree::kChanged              = "Changed::Tree";
const JUtf8Byte* Tree::kBoundsChanged        = "BoundsChanged::Tree";
const JUtf8Byte* Tree::kNeedsRefresh         = "NeedsRefresh::Tree";
const JUtf8Byte* Tree::kFontSizeChanged      = "FontSizeChanged::Tree";

const JUtf8Byte* Tree::kUpdateFoundChanges   = "UpdateFoundChanges::Tree";

const JUtf8Byte* Tree::kClassSelected        = "ClassSelected::Tree";
const JUtf8Byte* Tree::kClassDeselected      = "ClassDeselected::Tree";
const JUtf8Byte* Tree::kAllClassesDeselected = "AllClassesDeselected::Tree";

/******************************************************************************
 Constructor

	In the 2nd version, dirList can be nullptr

 ******************************************************************************/

Tree::Tree
	(
	ClassStreamInFn		streamInFn,
	TreeDirector*		director,
	const Language		lang,
	const TextFileType	fileType,
	const JSize			marginWidth
	)
	:
	JContainer(),
	itsLanguage(lang),
	itsFileType(fileType),
	itsMarginWidth(marginWidth)
{
	TreeX(director, streamInFn);
}

#ifndef CODE_CRUSADER_UNIT_TEST

Tree::Tree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symStream,
	const JFileVersion	origSymVers,
	ClassStreamInFn		streamInFn,
	TreeDirector*		director,
	const Language		lang,
	const TextFileType	fileType,
	const JSize			marginWidth,
	DirList*			dirList
	)
	:
	JContainer(),
	itsLanguage(lang),
	itsFileType(fileType),
	itsMarginWidth(marginWidth)
{
JIndex i;

	TreeX(director, streamInFn);

	std::istream* symInput        = (projVers <= 41 ? &projInput : symStream);
	const JFileVersion symVers    = (projVers <= 41 ? projVers   : origSymVers);
	const bool useSetProjData = setInput == nullptr || setVers < 71;
	const bool useSymProjData = symInput == nullptr || symVers < 71;

/* symbol file */

	bool needsReparse = projVers < 97;

	if (projVers < 71)
	{
		projInput >> itsWidth >> itsHeight;
	}
	if (!useSymProjData)	// overwrite
	{
		*symInput >> itsWidth >> itsHeight;
	}

/* settings file */

	if (projVers < 71)
	{
		bool showLoneClasses, showLoneStructs, showEnums;
		if (projVers >= 12)
		{
			projInput >> JBoolFromString(showLoneClasses);
			if (projVers >= 13)
			{
				projInput >> JBoolFromString(showLoneStructs);
			}
		}
		if (projVers >= 10)
		{
			projInput >> JBoolFromString(showEnums);
		}

		if (projVers >= 19)
		{
			projInput >> JBoolFromString(itsNeedsMinimizeMILinksFlag);
		}
		else
		{
			itsNeedsMinimizeMILinksFlag = true;
		}
	}
	if (!useSetProjData)	// overwrite
	{
		if (setVers < 87)
		{
			bool showLoneClasses, showLoneStructs, showEnums;
			*setInput >> JBoolFromString(showLoneClasses)
					  >> JBoolFromString(showLoneStructs)
					  >> JBoolFromString(showEnums);
		}
		*setInput >> JBoolFromString(itsNeedsMinimizeMILinksFlag);
	}

/* symbol file */

	if (symInput != nullptr)
	{
		bool classNamesSorted = true;
		if (symVers < 7)
		{
			*symInput >> JBoolFromString(classNamesSorted);
		}

		// get number of classes in tree

		JSize classCount;
		*symInput >> classCount;

		// read in each class

		itsClassesByFull->SetBlockSize(classCount+1);	// avoid repeated realloc
		itsVisibleByName->SetBlockSize(classCount+1);

		for (i=1; i<=classCount; i++)
		{
			Class* newClass = streamInFn(*symInput, symVers, this);
			itsClassesByFull->Append(newClass);
			if (newClass->IsVisible())
			{
				itsVisibleByName->InsertSorted(newClass);
			}
		}

		itsClassesByFull->SetBlockSize(kBlockSize);
		itsVisibleByName->SetBlockSize(kBlockSize);

		if (symVers >= 18)
		{
			const JSize geomCount = itsVisibleByName->GetItemCount();
			itsVisibleByGeom->SetBlockSize(geomCount+1);	// avoid repeated realloc
			for (i=1; i<=geomCount; i++)
			{
				JIndex j;
				*symInput >> j;
				itsVisibleByGeom->Append(itsVisibleByName->GetItem(j));
			}
			itsVisibleByGeom->SetBlockSize(kBlockSize);
		}

		for (i=1; i<=classCount; i++)
		{
			itsClassesByFull->GetItem(i)->FindParentsAfterRead();
		}

		// We can only sort this after calling FindParentsAfterRead(),
		// because this function uses the order saved in the file.

		if (!classNamesSorted || symVers < 43)
		{
			itsClassesByFull->Sort();
		}
	}

/* project file */

	// We read this way down here because in version <= 41,
	// everything was in a single file.

	if (dirList != nullptr)
	{
		dirList->ReadDirectories(projInput, projVers);
	}

	if (projVers < 40)
	{
		// toss file name and modification date

		JSize fileCount;
		projInput >> fileCount;

		JString name;
		time_t t;
		for (i=1; i<=fileCount; i++)
		{
			projInput >> name >> t;
		}
	}

/* symbol file */

	// check for change in file suffixes

	if (projVers >= 9)
	{
		JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
		if (projVers < 71)
		{
			projInput >> suffixList;
		}
		if (!useSymProjData)	// overwrite
		{
			*symInput >> suffixList;
		}
		if (!JSameStrings(*itsSuffixList, suffixList, JString::kCompareCase))
		{
			needsReparse = true;
		}
	}

	if (needsReparse)
	{
		NextUpdateMustReparseAll();
	}
}

#endif

// private

void
Tree::TreeX
	(
	TreeDirector*	director,
	ClassStreamInFn	streamInFn
	)
{
	itsDirector = director;
	itsFontSize = JFontManager::GetDefaultFontSize();

	itsClassesByFull = jnew JPtrArray<Class>(JPtrArrayT::kDeleteAll, kBlockSize);
	assert( itsClassesByFull != nullptr );
	itsClassesByFull->SetCompareFunction(CompareClassFullNames);
	itsClassesByFull->SetSortOrder(JListT::kSortAscending);

	itsVisibleByGeom = jnew JPtrArray<Class>(JPtrArrayT::kForgetAll, kBlockSize);
	assert( itsVisibleByGeom != nullptr );

	itsVisibleByName = jnew JPtrArray<Class>(JPtrArrayT::kForgetAll, kBlockSize);
	assert( itsVisibleByName != nullptr );
	itsVisibleByName->SetCompareFunction(CompareClassNames);
	itsVisibleByName->SetSortOrder(JListT::kSortAscending);

#ifndef CODE_CRUSADER_UNIT_TEST
	itsSuffixList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsSuffixList != nullptr );
	GetPrefsManager()->GetFileSuffixes(itsFileType, itsSuffixList);
#else
	itsSuffixList = nullptr;
#endif

	itsCollapsedList = nullptr;

	InstallCollection(itsClassesByFull);

	itsWidth = itsHeight = 0;
	itsBroadcastClassSelFlag = true;

	itsDrawMILinksOnTopFlag = true;

	itsMinimizeMILinksFlag      = false;
	itsNeedsMinimizeMILinksFlag = false;
	itsMinimizeMILinksThread    = nullptr;

	itsReparseAllFlag           = false;
	itsChangedDuringParseFlag   = false;
	itsBeganEmptyFlag           = false;

	itsStreamInFn = streamInFn;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Tree::~Tree()
{
	while (itsMinimizeMILinksThread != nullptr)		// director triggered cancel
	{
		boost::this_fiber::yield();
	}

	jdelete itsClassesByFull;
	jdelete itsVisibleByGeom;
	jdelete itsVisibleByName;
	jdelete itsCollapsedList;
	jdelete itsSuffixList;
}

/******************************************************************************
 StreamOut (virtual)

	dirList can be nullptr

 ******************************************************************************/

void
Tree::StreamOut
	(
	std::ostream&	projOutput,
	std::ostream*	setOutput,
	std::ostream*	symOutput,
	const DirList*	dirList
	)
	const
{
#ifndef CODE_CRUSADER_UNIT_TEST

JIndex i;

/* project file */

	if (dirList != nullptr)
	{
		dirList->WriteDirectories(projOutput);
	}

/* symbol file */

	if (symOutput != nullptr)
	{
		*symOutput << ' ' << itsWidth;
		*symOutput << ' ' << itsHeight;
	}

/* settings file */

	if (setOutput != nullptr)
	{
		*setOutput << ' ' << JBoolToString(itsNeedsMinimizeMILinksFlag);
	}

/* symbol file */

	if (symOutput != nullptr)
	{
		// write the number of classes in tree

		const JSize classCount = itsClassesByFull->GetItemCount();
		*symOutput << ' ' << classCount;

		// write each class

		for (i=1; i<=classCount; i++)
		{
			*symOutput << ' ';
			(itsClassesByFull->GetItem(i))->StreamOut(*symOutput);
		}

		// write geometry ordering

		const JSize geomCount = itsVisibleByGeom->GetItemCount();
		assert( geomCount == itsVisibleByName->GetItemCount() );
		for (i=1; i<=geomCount; i++)
		{
			JIndex j;
			const bool found = itsVisibleByName->Find(itsVisibleByGeom->GetItem(i), &j);
			assert( found );
			*symOutput << ' ' << j;
		}

		// save file suffixes so we can check if they change

		*symOutput << ' ' << *itsSuffixList;

		*symOutput << ' ';
	}

#endif
}

/******************************************************************************
 FileTypesChanged

	We can't use Receive() because this must be called -before-
	ProjectDocument updates everything.

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

void
Tree::FileTypesChanged
	(
	const PrefsManager::FileTypesChanged& info
	)
{
	if (info.Changed(itsFileType))
	{
		GetPrefsManager()->GetFileSuffixes(itsFileType, itsSuffixList);
		NextUpdateMustReparseAll();
	}
}

#endif

/******************************************************************************
 PrepareForUpdate (virtual)

	Get ready to parse files that have changed or been created and to
	throw out classes in files that no longer exist.

	*** This runs in the update fiber.

 ******************************************************************************/

void
Tree::PrepareForUpdate
	(
	const bool reparseAll
	)
{
	assert( !itsReparseAllFlag || reparseAll );

	while (itsMinimizeMILinksThread != nullptr)		// director triggered cancel
	{
		boost::this_fiber::yield();
	}

	// save collapsed classes

	itsCollapsedList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsCollapsedList != nullptr );
	SaveCollapsedClasses(itsCollapsedList);

	if (reparseAll)
	{
		// toss everything

		itsClassesByFull->DeleteAll();
		itsVisibleByGeom->RemoveAll();
		itsVisibleByName->RemoveAll();

		Broadcast(UpdateFoundChanges());
	}

	itsChangedDuringParseFlag = reparseAll;
	itsBeganEmptyFlag         = itsClassesByFull->IsEmpty();
}

/******************************************************************************
 UpdateFinished (virtual)

	Cleans up after updating files.  Returns true if changes were found.

	*** This runs in the update fiber.

 ******************************************************************************/

bool
Tree::UpdateFinished
	(
	const JArray<JFAID_t>&	deadFileList,
	JProgressDisplay&		pg
	)
{
	// toss files that no longer exist

	const JSize fileCount = deadFileList.GetItemCount();
	for (JIndex i=1; i<=fileCount; i++)
	{
		RemoveFile(deadFileList.GetItem(i));
		pg.IncrementProgress();
	}

	// restore collapsed classes

	const bool forceRecalcVisible = RestoreCollapsedClasses(*itsCollapsedList);
	jdelete itsCollapsedList;
	itsCollapsedList = nullptr;

	// rebuild tree

	if (itsChangedDuringParseFlag)
	{
		RebuildTree();
	}
	else if (forceRecalcVisible)
	{
		RecalcVisible();
	}

	if (itsChangedDuringParseFlag)
	{
		Broadcast(Changed());
	}

	itsReparseAllFlag = false;
	return itsChangedDuringParseFlag;
}

/******************************************************************************
 FileChanged

	Returns true if the file should be parsed.

	Not private because it is called by FileListTable::ParseFile().

 ******************************************************************************/

void
Tree::FileChanged
	(
	const JString&		fileName,
	const TextFileType	fileType,
	const JFAID_t		id
	)
{
	if (fileType == itsFileType)
	{
		if (!itsBeganEmptyFlag)
		{
			RemoveFile(id);
		}

		try
		{
			ParseFile(fileName, id);
		}
		catch (const std::exception& e)
		{
			std::cerr << fileName << ": " << e.what() << std::endl;
		}
	}
}

/******************************************************************************
 RemoveFile (private)

	Remove the specified file and all associated classes.

	*** Caller must call RebuildTree().

 ******************************************************************************/

void
Tree::RemoveFile
	(
	const JFAID_t id
	)
{
	const JSize classCount = itsClassesByFull->GetItemCount();
	for (JIndex i=classCount; i>=1; i--)
	{
		const Class* theClass = itsClassesByFull->GetItem(i);
		JFAID_t fileID;
		if (theClass->GetFileID(&fileID) && fileID == id)
		{
			itsClassesByFull->DeleteItem(i);
			itsVisibleByGeom->Remove(theClass);
			itsVisibleByName->Remove(theClass);

			if (!itsChangedDuringParseFlag)
			{
				Broadcast(UpdateFoundChanges());
			}
			itsChangedDuringParseFlag = true;
		}
	}
}

/******************************************************************************
 SaveCollapsedClasses (private)

	Save the names of the collapsed classes in the given string list.

 ******************************************************************************/

void
Tree::SaveCollapsedClasses
	(
	JPtrArray<JString>* list
	)
	const
{
	const JSize count = itsClassesByFull->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const Class* theClass = itsClassesByFull->GetItem(i);
		if (theClass->IsCollapsed())
		{
			list->Append(theClass->GetFullName());
		}
	}
}

/******************************************************************************
 RestoreCollapsedClasses (private)

	Collapse the classes in the given list.

	*** Caller must call RecalcVisible().

 ******************************************************************************/

bool
Tree::RestoreCollapsedClasses
	(
	const JPtrArray<JString>& list
	)
{
	bool changed = false;

	const JSize count = list.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JString* fullName = list.GetItem(i);
		Class* theClass;
		if (FindClass(*fullName, &theClass) && !theClass->IsCollapsed())
		{
			theClass->SetCollapsed(true);
			changed = true;
		}
	}

	return changed;
}

/******************************************************************************
 AddClass (private)

	*** Caller must ensure that RebuildTree() gets called.

 ******************************************************************************/

void
Tree::AddClass
	(
	Class* newClass
	)
{
	itsClassesByFull->InsertSorted(newClass);
	if (newClass->IsVisible())
	{
		itsVisibleByName->InsertSorted(newClass);
		itsVisibleByGeom->RemoveAll();
	}

	if (!itsChangedDuringParseFlag)
	{
		Broadcast(UpdateFoundChanges());
	}
	itsChangedDuringParseFlag = true;
}

/******************************************************************************
 RebuildTree (private)

	Recalculates all parents and then places all classes.

	Because one is not required to fully qualify every parent
	(e.g. JBroadcaster::Message), some classes can not find their parents
	immediately.  Thus, we keep looping until nobody can find any more
	of their parents, and then let them create ghosts.

	With this algorithm, progress is guaranteed at every step.
	In each pass, the following classes will find their parents:
		1)	All classes inheriting from classes at global scope or
			inheriting from fully qualified classes.
		2)	All classes inheriting from (1)
		3)	All classes inheriting from (2)
		4)  ...

 ******************************************************************************/

void
Tree::RebuildTree()
{
JIndex i;

	// delete ghost classes and clear all parent-child connections

	JSize classCount = itsClassesByFull->GetItemCount();
	for (i=classCount; i>=1; i--)
	{
		Class* theClass = itsClassesByFull->GetItem(i);
		if (theClass->IsGhost())
		{
			itsClassesByFull->DeleteItem(i);
			itsVisibleByGeom->Remove(theClass);
			itsVisibleByName->Remove(theClass);
			classCount--;
		}
		else
		{
			theClass->ClearConnections();
		}
	}

	// Keep looking for parents as long as somebody finds another parent.
	// Don't allow ghosts since not all parents have been found.

	bool progress;
	do
	{
		progress = false;
		for (i=1; i<=classCount; i++)
		{
			progress = itsClassesByFull->GetItem(i)->FindParents(false) || progress;
		}
	}
	while (progress);

	// Now that no more progress is being made, allow ghosts to be created.
	// (Inserting ghosts does not cause classes to be skipped.)

	for (i=1; i<=classCount; i++)
	{
		itsClassesByFull->GetItem(i)->FindParents(true);
		classCount = GetItemCount();
	}

	// Now that all parents have been found, we can hide lone classes and
	// classes whose parents are hidden or collapsed.

	RecalcVisible(false, true);
}

/******************************************************************************
 RecalcVisible (private)

	Hides classes whose parents are hidden or collapsed.

	If necessary or requested, it rebuilds the list of visible classes
	and adjusts their placement in the tree.

 ******************************************************************************/

void
Tree::RecalcVisible
	(
	const bool forceRebuildVisible,
	const bool forcePlaceAll
	)
{
	const JSize classCount = itsClassesByFull->GetItemCount();
	bool rebuildVisible    = forceRebuildVisible;

	// hide classes whose parents are hidden or collapsed

	bool changed;
	do
	{
		changed = false;
		for (JIndex i=1; i<=classCount; i++)
		{
			Class* theClass = itsClassesByFull->GetItem(i);
			Class* parent;
			if (theClass->GetParent(1, &parent))
			{
				if (parent->IsCollapsed() && theClass->IsVisible())
				{
					rebuildVisible = changed = true;
					theClass->SetVisible(false);
				}
				else if (!parent->IsCollapsed() &&
						 parent->IsVisible() != theClass->IsVisible())
				{
					rebuildVisible = changed = true;
					theClass->SetVisible(parent->IsVisible());
				}
			}
		}
	}
		while (changed);

	// rebuild the list of visible classes

	if (rebuildVisible)
	{
		itsVisibleByName->RemoveAll();
		for (JIndex i=1; i<=classCount; i++)
		{
			Class* theClass = itsClassesByFull->GetItem(i);
			if (theClass->IsVisible())
			{
				itsVisibleByName->InsertSorted(theClass);
			}
		}
	}

	// Now that we know which classes are visible, we can optimize the arrangement
	// to minimize the lengths of multiple inheritance links.

	if (rebuildVisible || itsVisibleByGeom->IsEmpty())
	{
		itsVisibleByGeom->CopyPointers(*itsVisibleByName,
									   itsVisibleByGeom->GetCleanUpAction(), false);

		// move class trees to top, to simplify visual searching

		const JSize classCount = itsVisibleByGeom->GetItemCount();

		JIndex min = 1;
		for (JIndex i = classCount; i >= min; i--)
		{
			Class* theClass = itsVisibleByGeom->GetItem(i);
			if (!theClass->HasParents() && theClass->HasChildren())
			{
				itsVisibleByGeom->MoveItemToIndex(i, 1);
				i++;		// recheck new element at this index
				min++;		// ignore newly moved element
			}
		}

		// optimize further

		MinimizeMILinks();
	}

	// place classes in the shape of a tree

	if (rebuildVisible || forcePlaceAll)
	{
		PlaceAll();
	}
}

/******************************************************************************
 PlaceAll (private)

	Calculates the placement of all classes, assuming all parents have
	been found.  Normally only called by RecalcVisible().

	rootGeom can be nullptr.  It is only needed by MinimizeMILinks().
	It contains information about root classes that have children
	and is sorted by Class*.

 ******************************************************************************/

void
Tree::PlaceAll
	(
	JArray<RootGeom>* rootGeom
	)
{
	itsWidth  = 0;
	itsHeight = itsMarginWidth;

	if (rootGeom != nullptr)
	{
		rootGeom->RemoveAll();
		rootGeom->SetCompareFunction(CompareRGClassPtrs);
	}

	const JSize classCount = itsVisibleByGeom->GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		Class* theClass = itsVisibleByGeom->GetItem(i);
		if (!theClass->HasParents())
		{
			const JCoordinate prevH = itsHeight;
			PlaceClass(theClass, itsMarginWidth, &itsHeight, &itsWidth);
			if (rootGeom != nullptr && theClass->HasChildren())
			{
				rootGeom->InsertSorted(RootGeom(theClass, prevH, itsHeight - prevH));
			}
		}
	}

	itsWidth  += itsMarginWidth;
	itsHeight += itsMarginWidth;

	Broadcast(BoundsChanged());
}

/******************************************************************************
 PlaceClass (private, recursive)

	Build the class tree whose root is the given class.
	Updates y to include the height of the new subtree.

 ******************************************************************************/

void
Tree::PlaceClass
	(
	Class*				theClass,
	const JCoordinate	x,
	JCoordinate*		y,
	JCoordinate*		maxWidth
	)
{
	const JCoordinate origY = *y;
	const JCoordinate w     = theClass->GetTotalWidth();

	bool foundDescendants = false;
	const JSize classCount = itsVisibleByGeom->GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		Class* aClass = itsVisibleByGeom->GetItem(i);
		Class* firstParent;
		if (aClass->GetParent(1, &firstParent) && theClass == firstParent)
		{
			foundDescendants = true;
			PlaceClass(aClass, x+w, y, maxWidth);
		}
	}

	const JCoordinate h = theClass->GetTotalHeight();
	if (foundDescendants)
	{
		// By quantizing the y-coordinate, we simplify pagination.

		JCoordinate newY = (*y+origY - h)/2;
		newY -= (newY - origY) % h;
		theClass->SetCoords(x, newY);
	}
	else
	{
		theClass->SetCoords(x, *y);
		*y += h;

		if (x+w > *maxWidth)
		{
			*maxWidth = x+w;
		}
	}
}

/******************************************************************************
 ForceMinimizeMILinks

 ******************************************************************************/

void
Tree::ForceMinimizeMILinks()
{
	if (itsNeedsMinimizeMILinksFlag)
	{
		const bool saveFlag    = itsMinimizeMILinksFlag;
		itsMinimizeMILinksFlag = true;
		RecalcVisible(true);
		itsMinimizeMILinksFlag = saveFlag;
	}
}

/******************************************************************************
 MinimizeMILinks (private)

	Rearrange each connected subset of trees to minimize the lengths
	of the connecting MI links.

 ******************************************************************************/

void
Tree::MinimizeMILinks()
{
	itsNeedsMinimizeMILinksFlag = !itsMinimizeMILinksFlag;

	const JSize classCount = itsVisibleByGeom->GetItemCount();
	if (classCount == 0)
	{
		return;
	}

	JArray<RootGeom> rootGeom;
	PlaceAll(&rootGeom);

	JPtrArray<Class> visByGeom(*itsVisibleByGeom, JPtrArrayT::kForgetAll),
					 newByGeom(*itsVisibleByGeom, JPtrArrayT::kForgetAll);

	boost::fibers::buffered_channel<JBroadcaster::Message*> channel(kJBufferedChannelCapacity);
	std::atomic_bool cancelFlag = false;
	JThreadPG threadPG(&channel, &cancelFlag, 1000);
	threadPG.VariableLengthProcessBeginning(JGetString("MinMILengthsProgress::Tree"), true, false);

	itsMinimizeMILinksThread = jnew std::thread([this, classCount, &rootGeom, &visByGeom, &newByGeom, &threadPG]()
	{
		// optimize each disjoint subset of trees connected by MI

		JArray<bool> marked(classCount);
		for (JIndex i=1; i<=classCount; i++)
		{
			marked.AppendItem(false);
		}

		for (JIndex i=1; i<=classCount; i++)
		{
			Class* theClass = visByGeom.GetItem(i);
			if (theClass->GetParentCount() > 1 && !marked.GetItem(i))
			{
				// find the roots that this class connects

				JArray<RootMIInfo> rootList;
				FindRoots(theClass, rootGeom, &rootList);
				marked.SetItem(i, true);

				// find all the other roots in this connected subset of trees
				// (must check GetItemCount() each time since list grows)

				for (JIndex j=1; j <= rootList.GetItemCount(); j++)
				{
					const RootMIInfo rootInfo = rootList.GetItem(j);
					FindMIClasses(rootInfo.root, &marked, rootGeom, &rootList, visByGeom);
				}

				// find the best ordering of the roots
				// (rootCount could be 1 if MI is within a single tree)

				const JSize rootCount = rootList.GetItemCount();
				if (rootCount > 1)
				{
//					std::cout << "# of roots: " << rootCount << std::endl;
					JArray<JIndex> rootOrder(rootCount);
					if (( itsMinimizeMILinksFlag &&
						 !ArrangeRootsDynamicProgramming(rootList, &rootOrder, threadPG)) ||
						(!itsMinimizeMILinksFlag &&
						 !ArrangeRootsGreedyNumberOfLinks(rootList, &rootOrder)))
					{
						for (JIndex j=1; j<=rootCount; j++)
						{
							jdelete rootList.GetItem(j).connList;
						}
						newByGeom.CleanOut();
						itsNeedsMinimizeMILinksFlag = true;
						break;
					}

					// rearrange the classes so PlaceAll() will place the trees
					// next to each other

					for (JIndex j=1; j<=rootCount; j++)
					{
						RootMIInfo info = rootList.GetItem(rootOrder.GetItem(j));
						newByGeom.Remove(info.root);
						newByGeom.InsertAtIndex(j, info.root);
					}
				}

				// clean up

				for (JIndex j=1; j<=rootCount; j++)
				{
					jdelete rootList.GetItem(j).connList;
				}
			}
		}

		threadPG.ProcessFinished();
	});

	threadPG.WaitForProcessFinished(itsDirector->GetMinimizeMILinksPG());
	itsMinimizeMILinksThread->join();

	jdelete itsMinimizeMILinksThread;
	itsMinimizeMILinksThread = nullptr;

	if (!newByGeom.IsEmpty())
	{
		itsVisibleByGeom->CopyPointers(newByGeom, itsVisibleByGeom->GetCleanUpAction(), false);
	}
}

/******************************************************************************
 ArrangeRootsGreedyNumberOfLinks (private static)

	Find the ordering of the given root classes that approximately
	minimizes the length of the MI connections.  The algorithm starts with
	the largest root, based on primary inheritance, and then tries each
	root, in order of descending # of links into the already placed items,
	above and below the already placed roots.

 ******************************************************************************/

inline bool
indexListIncludes
	(
	const JArray<JIndex>&	list,
	const JIndex			i
	)
{
	for (auto j : list)
	{
		if (j == i)
		{
			return true;
		}
	}

	return false;
}

JCoordinate
Tree::TerminateOrPassThrough
	(
	JArray<RootConn>*	connList,
	const JIndex		rootIndex,
	const JCoordinate	h
	)
{
	JCoordinate v = 0;
	for (JIndex i=connList->GetItemCount(); i>0; i--)
	{
		const auto conn = connList->GetItem(i);
		if (conn.otherRoot == rootIndex)
		{
			connList->RemoveItem(i);
		}
		else
		{
			v += h;
		}
	}

	return v;
}

bool
Tree::ArrangeRootsGreedyNumberOfLinks
	(
	const JArray<RootMIInfo>&	rootList,
	JArray<JIndex>*				rootOrder
	)
{
	JIndex i = 0, maxConnIndex = 0;
	JSize connCountMax = 0;
	JCoordinate hMax   = -1;
	for (const auto& root : rootList)
	{
		i++;
		if (root.connList->GetItemCount() > connCountMax ||
			(root.connList->GetItemCount() == connCountMax && root.h > hMax))
		{
			maxConnIndex = i;
			connCountMax = root.connList->GetItemCount();
			hMax         = root.h;
		}
	}

	rootOrder->AppendItem(maxConnIndex);

	const JSize rootCount = rootList.GetItemCount();
	while (rootOrder->GetItemCount() < rootCount)
	{
		// find root with max # of links into already placed roots

		maxConnIndex = 0;
		connCountMax = 0;
		hMax         = -1;
		for (JIndex i=1; i<=rootCount; i++)
		{
			if (indexListIncludes(*rootOrder, i))
			{
				continue;
			}

			const auto root = rootList.GetItem(i);

			JSize connCount = 0;
			for (const auto& conn : *root.connList)
			{
				if (indexListIncludes(*rootOrder, conn.otherRoot))
				{
					connCount++;
				}
			}

			if (connCount > connCountMax ||
				(connCount == connCountMax && root.h > hMax))
			{
				maxConnIndex = i;
				connCountMax = connCount;
				hMax         = root.h;
			}
		}

		// try adding this root above the placed roots

		const auto root = rootList.GetItem(maxConnIndex);
		auto connList(*root.connList);
		JCoordinate above = 0;
		for (const auto& conn : connList)
		{
			above += root.h - conn.dy;
		}

		for (const auto rootIndex : *rootOrder)
		{
			const auto otherRoot = rootList.GetItem(rootIndex);
			for (const auto& otherConn : *otherRoot.connList)
			{
				if (otherConn.otherRoot == maxConnIndex)
				{
					above += otherConn.dy;
				}
			}

			above += TerminateOrPassThrough(&connList, rootIndex, otherRoot.h);
		}

		// try adding this root below the placed roots

		connList = *root.connList;
		JCoordinate below = 0;
		for (const auto& conn : connList)
		{
			below += conn.dy;
		}

		for (JIndex j=rootOrder->GetItemCount(); j>0; j--)
		{
			const JIndex rootIndex = rootOrder->GetItem(j);
			const auto otherRoot   = rootList.GetItem(rootIndex);
			for (const auto& otherConn : *otherRoot.connList)
			{
				if (otherConn.otherRoot == maxConnIndex)
				{
					below += otherRoot.h - otherConn.dy;
				}
			}

			below += TerminateOrPassThrough(&connList, rootIndex, otherRoot.h);
		}

		// pick the placement that minimizes the total link length

		if (above < below)
		{
			rootOrder->PrependItem(maxConnIndex);
		}
		else
		{
			rootOrder->AppendItem(maxConnIndex);
		}
	}

	return true;
}

/******************************************************************************
 ArrangeRootsDynamicProgramming (private static)

	Find the ordering of the given root classes that minimizes the length
	of the MI connections.  The algorithm is based on the concept of
	dynamic programming to avoid factorial runtime -- but it can still be
	really slow!

	Returns false if the user cancels.

 ******************************************************************************/

bool
Tree::ArrangeRootsDynamicProgramming
	(
	const JArray<RootMIInfo>&	rootList,
	JArray<JIndex>*				rootOrder,
	JProgressDisplay&			pg
	)
{
	const JSize rootCount = rootList.GetItemCount();
	std::cout << "# roots: " << rootCount << std::endl;

	JArray<RootSubset> l1(100), l2(100);
	JArray<RootSubset> *list1 = &l1, *list2 = &l2;

	l1.SetCompareFunction(CompareRSContent);
	l2.SetCompareFunction(CompareRSContent);

	auto* content = jnew JArray<bool>(rootCount);
	assert( content != nullptr );
	for (JIndex i=1; i<=rootCount; i++)
	{
		content->AppendItem(false);
	}

	auto* order = jnew JArray<JIndex>(rootCount);
	assert( order != nullptr );

	list1->AppendItem(RootSubset(content, order, 0));

	do
	{
		const JSize subsetCount = list1->GetItemCount();
		std::cout << "do loop: " << subsetCount << std::endl;

		for (JIndex subsetIndex=1; subsetIndex<=subsetCount; subsetIndex++)
		{
			const RootSubset* subset = list1->GetCArray() + subsetIndex-1;
			const JSize subsetSize   = (subset->order)->GetItemCount();
			for (JIndex newRootIndex=1; newRootIndex<=rootCount; newRootIndex++)
			{
				if (subset->content->GetItem(newRootIndex))
				{
					continue;
				}

				// try adding this root to the end of the subset

				JSize newLinkLength = subset->linkLength;

				// add length of links from new root

				const RootMIInfo* rootPtr   = rootList.GetCArray();
				const RootMIInfo* rootInfo  = rootPtr + newRootIndex-1;
				const JCoordinate newHeight = rootInfo->h;
				const JSize linkCount       = rootInfo->connList->GetItemCount();
				const RootConn* connInfo    = rootInfo->connList->GetCArray();
				for (JUnsignedOffset i=0; i<linkCount; i++)
				{
					if (subset->content->GetItem(connInfo[i].otherRoot))
					{
						newLinkLength += connInfo[i].dy;
					}
					else
					{
						newLinkLength += newHeight - connInfo[i].dy;
					}
				}

				// add length of links continuing past new root

				for (JIndex i=1; i<=subsetSize; i++)
				{
					const JIndex j = subset->order->GetItem(i);

					const RootMIInfo* rootInfo = rootPtr + j-1;
					const JSize linkCount      = rootInfo->connList->GetItemCount();
					const RootConn* connInfo   = rootInfo->connList->GetCArray();
					for (JIndex k=0; k<linkCount; k++)
					{
						const JIndex otherRoot = connInfo[k].otherRoot;
						if (otherRoot != newRootIndex &&
							!subset->content->GetItem(otherRoot))
						{
							newLinkLength += newHeight;
						}
					}
				}

				// if this hasn't been tried or it beats the previous record, save it

				subset->content->SetItem(newRootIndex, true);

				bool found;
				const JIndex i = list2->SearchSortedOTI(*subset, JListT::kAnyMatch, &found);
				if (found && newLinkLength < list2->GetCArray()[i-1].linkLength)
				{
					RootSubset foundSubset = list2->GetItem(i);
					*foundSubset.content  = *subset->content;
					*foundSubset.order    = *subset->order;
					foundSubset.order->AppendItem(newRootIndex);
					foundSubset.linkLength = newLinkLength;
					list2->SetItem(i, foundSubset);
				}
				else if (!found)
				{
					auto* newContent = jnew JArray<bool>(*subset->content);
					auto* newOrder   = jnew JArray<JIndex>(*subset->order);
					newOrder->AppendItem(newRootIndex);

					list2->InsertItemAtIndex(i, RootSubset(newContent, newOrder, newLinkLength));
				}

				subset->content->SetItem(newRootIndex, false);
			}

			if (!pg.IncrementProgress())
			{
				CleanList(list1);
				CleanList(list2);
				return false;
			}
		}

		// swap and repeat

		CleanList(list1);

		JArray<RootSubset>* tempList = list1;
		list1 = list2;
		list2 = tempList;
	}
		while (list1->GetItemCount() > 1);

	// return the result

	RootSubset completeSet = list1->GetFirstItem();
	*rootOrder = *completeSet.order;
	jdelete completeSet.content;
	jdelete completeSet.order;

	return true;
}

/******************************************************************************
 CleanList (private static)

	Deletes all objects in the list and then empties it.

 ******************************************************************************/

void
Tree::CleanList
	(
	JArray<RootSubset>* list
	)
{
	const JSize count = list->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		RootSubset subset = list->GetItem(i);
		jdelete subset.content;
		jdelete subset.order;
	}

	list->RemoveAll();
}

/******************************************************************************
 FindMIClasses (private static)

	For each primary child of the given class, check if it connects
	trees via MI and then recurse on it.

 ******************************************************************************/

void
Tree::FindMIClasses
	(
	Class*					theClass,
	JArray<bool>*			marked,
	const JArray<RootGeom>&	rootGeom,
	JArray<RootMIInfo>*		rootList,
	const JPtrArray<Class>&	visibleByGeom
	)
{
	const JSize classCount = visibleByGeom.GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		Class* c                = visibleByGeom.GetItem(i);
		const JSize parentCount = c->GetParentCount();
		for (JIndex parentIndex=1; parentIndex<=parentCount; parentIndex++)
		{
			Class* p;
			const bool ok = c->GetParent(parentIndex, &p);
			assert( ok );

			if (p == theClass && !marked->GetItem(i))
			{
				FindRoots(c, rootGeom, rootList);
				marked->SetItem(i, true);
			}
			if (parentIndex == 1 && p == theClass && c->HasChildren())
			{
				FindMIClasses(c, marked, rootGeom, rootList, visibleByGeom);
			}
		}
	}
}

/******************************************************************************
 FindRoots (private static)

	If the given class connects two or more trees via MI, store information
	about each connection.

 ******************************************************************************/

void
Tree::FindRoots
	(
	Class*					theClass,
	const JArray<RootGeom>&	rootGeom,
	JArray<RootMIInfo>*		rootList
	)
{
	const JSize parentCount = theClass->GetParentCount();
	if (parentCount < 2)
	{
		return;
	}

	JIndex primaryRootIndex   = 0;
	JCoordinate primaryRootDy = 0;
	for (JIndex parentIndex=1; parentIndex<=parentCount; parentIndex++)
	{
		// find primary root for this parent

		Class* parent;
		bool ok = theClass->GetParent(parentIndex, &parent);
		assert( ok );

		Class* root = parent;
		while (root->HasParents())
		{
			ok = root->GetParent(1, &root);
			assert( ok );
		}

		// find the geometry information

		JIndex geomIndex;
		ok = rootGeom.SearchSorted(RootGeom(root,0,0), JListT::kAnyMatch, &geomIndex);
		assert( ok );
		const RootGeom geom = rootGeom.GetItem(geomIndex);

		// if root is not already in master list, append it

		JIndex rootIndex;
		if (!FindRoot(root, *rootList, &rootIndex))
		{
			const RootMIInfo info(root, geom.h, jnew JArray<RootConn>);
			assert( info.connList != nullptr );
			rootList->AppendItem(info);
			rootIndex = rootList->GetItemCount();
		}

		if (parentIndex == 1)
		{
			primaryRootIndex = rootIndex;

			JCoordinate x,y;
			theClass->GetCoords(&x, &y);
			primaryRootDy = y - geom.top;
		}
		else if (rootIndex != primaryRootIndex)
		{
			// add information about connection between trees

			RootMIInfo info = rootList->GetItem(primaryRootIndex);
			info.connList->AppendItem(RootConn(primaryRootDy, rootIndex));

			JCoordinate x,y;
			parent->GetCoords(&x, &y);
			info = rootList->GetItem(rootIndex);
			info.connList->AppendItem(RootConn(y - geom.top, primaryRootIndex));
		}
	}
}

/******************************************************************************
 FindRoot (private static)

	Returns true if rootList contains the given root.  We have to do
	a linear search because roots are appended to the list as they are
	encountered to allow the list to grow as the overall search for roots
	proceeds. (see MinimizeMILinks())  This is also required because the
	indicies stored in RootConn index into rootList.

 ******************************************************************************/

bool
Tree::FindRoot
	(
	Class*						root,
	const JArray<RootMIInfo>&	rootList,
	JIndex*						index
	)
{
	const JSize count = rootList.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (root == rootList.GetItem(i).root)
		{
			*index = i;
			return true;
		}
	}

	*index = 0;
	return false;
}

/******************************************************************************
 FindClass

	Returns true if a class with the given full name exists.

 ******************************************************************************/

bool
Tree::FindClass
	(
	const JString&	fullName,
	const Class**	theClass
	)
	const
{
	return FindClass(fullName, const_cast<Class**>(theClass));
}

bool
Tree::FindClass
	(
	const JString&	fullName,
	Class**			theClass
	)
	const
{
	bool found = false;
	JIndex index   = 0;
	if (!IsEmpty())
	{
		Class target(fullName);
		found = itsClassesByFull->SearchSorted(&target, JListT::kFirstMatch, &index);
	}

	if (found)
	{
		*theClass = itsClassesByFull->GetItem(index);
		return true;
	}
	else
	{
		*theClass = nullptr;
		return false;
	}
}

/******************************************************************************
 IsUniqueClassName

	Returns true if there exists a single class with the given name,
	not full name.

 ******************************************************************************/

bool
Tree::IsUniqueClassName
	(
	const JString&	name,
	const Class**	theClass
	)
	const
{
	*theClass = nullptr;

	const JSize classCount = itsClassesByFull->GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		Class* c = itsClassesByFull->GetItem(i);
		if (c->GetName() == name)
		{
			if (*theClass == nullptr)
			{
				*theClass = c;
			}
			else
			{
				*theClass = nullptr;
				return false;
			}
		}
	}

	return *theClass != nullptr;
}

/******************************************************************************
 ClosestVisibleMatch

	Returns the closest visible match for the given name prefix.

 ******************************************************************************/

bool
Tree::ClosestVisibleMatch
	(
	const JString&	prefixStr,
	Class**		theClass
	)
	const
{
	if (IsEmpty())
	{
		*theClass = nullptr;
		return false;
	}

	Class target(prefixStr);
	bool found;
	JIndex index =
		itsVisibleByName->SearchSortedOTI(&target, JListT::kFirstMatch, &found);
	if (index > itsVisibleByName->GetItemCount())		// insert beyond end of list
	{
		index = itsVisibleByName->GetItemCount();
	}
	*theClass = itsVisibleByName->GetItem(index);
	return true;
}

/******************************************************************************
 FindParent

	Does "container" or any of its parents contain a class called "parentName"?

	Returns true if a class "parentName" exists and the namespace is a class
	that is an ancestor of the class "container".

	"parent" returns the class "parentName".
	"nameSpace" returns the namespace of "parentName".

	Called by Class::FindParent().

 ******************************************************************************/

bool
Tree::FindParent
	(
	const JString&	parentName,
	const Class*	container,
	Class**			parent,
	JString*		nameSpace
	)
	const
{
	const JUtf8Byte* namespaceOp = GetNamespaceOperator(itsLanguage);

	JString parentSuffix(namespaceOp);
	parentSuffix += parentName;

	for (auto* c : *itsClassesByFull)
	{
		const JString& cName = c->GetFullName();
		if (cName.EndsWith(parentSuffix) &&
			parentSuffix.GetCharacterCount() < cName.GetCharacterCount())
		{
			JString prefixClassName = cName;
			JStringIterator iter(&prefixClassName, JStringIterator::kStartAtEnd);
			iter.Prev(parentSuffix);
			iter.RemoveAllNext();
			iter.Invalidate();

			const Class* prefixClass = nullptr;
			if (FindClass(prefixClassName, &prefixClass) &&
				(prefixClass == container ||
				 prefixClass->IsAncestor(container)))
			{
				*parent    = c;
				*nameSpace = prefixClassName + namespaceOp;
				return true;
			}
		}
	}

	*parent = nullptr;
	nameSpace->Clear();
	return false;
}

/******************************************************************************
 IndexToClassAfterRead (private)

	Called by Class when we call Class::FindParentsAfterRead().

	Not inline so it is next to ClassToIndexForWrite().  Both functions
	must use the same list of classes.

 ******************************************************************************/

Class*
Tree::IndexToClassAfterRead
	(
	const JIndex index
	)
	const
{
	return itsClassesByFull->GetItem(index);
}

/******************************************************************************
 ClassToIndexForWrite (private)

	Called by Class when we call Class::StreamOut().

 ******************************************************************************/

JIndex
Tree::ClassToIndexForWrite
	(
	const Class* theClass
	)
	const
{
	JIndex index;
	const bool found = itsClassesByFull->Find(theClass, &index);
	assert( found );
	return index;
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

bool
Tree::HasSelection()
	const
{
	const JSize classCount = itsVisibleByName->GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		if ((itsVisibleByName->GetItem(i))->IsSelected())
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 GetSelectedClasses

	Do *not* call DeleteAll() on the classList that is returned!

 ******************************************************************************/

bool
Tree::GetSelectedClasses
	(
	JPtrArray<Class>* classList
	)
	const
{
	classList->RemoveAll();
	classList->SetCleanUpAction(JPtrArrayT::kForgetAll);

	const JSize classCount = itsVisibleByName->GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		Class* c = itsVisibleByName->GetItem(i);
		if (c->IsSelected())
		{
			classList->Append(c);
		}
	}

	return !classList->IsEmpty();
}

/******************************************************************************
 DeselectAll

 ******************************************************************************/

void
Tree::DeselectAll()
{
	itsBroadcastClassSelFlag = false;

	const JSize count = itsClassesByFull->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		(itsClassesByFull->GetItem(i))->SetSelected(false);
	}

	itsBroadcastClassSelFlag = true;

	Broadcast(AllClassesDeselected());
}

/******************************************************************************
 GetSelectionCoverage

	Returns true if there are any selected classes, and then sets *coverage
	to the coverage of these classes, and *selCount to the number of classes.

 ******************************************************************************/

bool
Tree::GetSelectionCoverage
	(
	JRect*	coverage,
	JSize*	selCount
	)
	const
{
	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
	{
		*coverage = (classList.GetFirstItem())->GetFrame();

		const JSize count = classList.GetItemCount();
		for (JIndex i=2; i<=count; i++)
		{
			*coverage = JCovering(*coverage, (classList.GetItem(i))->GetFrame());
		}

		*selCount = classList.GetItemCount();
		return true;
	}
	else
	{
		coverage->Set(0,0,0,0);
		*selCount = 0;
		return false;
	}
}

/******************************************************************************
 SelectClasses

	Selects all the classes with the given name or the full name.

 ******************************************************************************/

void
Tree::SelectClasses
	(
	const JString&	name,
	const bool		deselectAll,
	const bool		useFullName
	)
{
	if (deselectAll)
	{
		DeselectAll();
	}

	bool changed = false;
	for (auto* c : *itsClassesByFull)
	{
		if (( useFullName && c->GetFullName() == name) ||
			(!useFullName && c->GetName() == name))
		{
			c->SetSelected(true);
			if (!c->IsVisible())
			{
				changed = true;
				c->ForceVisible();
			}
		}
	}

	if (changed)
	{
		RecalcVisible(true);		// ForceVisible() can uncollapse
		Broadcast(Changed());
	}
}

/******************************************************************************
 SelectParents

	Selects the parents of the selected classes.

 ******************************************************************************/

void
Tree::SelectParents()
{
	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
	{
		bool changed = false;
		for (auto* c : classList)
		{
			const JSize parentCount = c->GetParentCount();
			for (JIndex j=1; j<=parentCount; j++)
			{
				Class* p;
				const bool ok = c->GetParent(j, &p);
				assert( ok );
				p->SetSelected(true);
				if (!p->IsVisible())
				{
					changed = true;
					p->ForceVisible();
				}
			}
		}

		if (changed)
		{
			RecalcVisible(true);		// ForceVisible() can uncollapse
			Broadcast(Changed());
		}
	}
}

/******************************************************************************
 SelectDescendants

	Selects all descendants of the selected classes.

 ******************************************************************************/

void
Tree::SelectDescendants()
{
	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
	{
		bool changed = false;
		for (auto* c : classList)
		{
			for (auto* c1 : *itsClassesByFull)
			{
				if (c->IsAncestor(c1))
				{
					c1->SetSelected(true);
					if (!c1->IsVisible())
					{
						changed = true;
						c1->ForceVisible();
					}
				}
			}
		}

		if (changed)
		{
			RecalcVisible(true);		// ForceVisible() can uncollapse
			Broadcast(Changed());
		}
	}
}

/******************************************************************************
 ViewSelectedSources

	Opens the source file for each selected class.

 ******************************************************************************/

void
Tree::ViewSelectedSources()
	const
{
	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
	{
		for (auto* c : classList)
		{
			c->ViewSource();
		}
	}
}

/******************************************************************************
 ViewSelectedHeaders

	Opens the header file for each selected class.

 ******************************************************************************/

void
Tree::ViewSelectedHeaders()
	const
{
	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
	{
		for (auto* c : classList)
		{
			c->ViewHeader();
		}
	}
}

/******************************************************************************
 ViewSelectedFunctionLists

	Opens a function list window for each selected class.

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

void
Tree::ViewSelectedFunctionLists()
	const
{
	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
	{
		for (auto* c : classList)
		{
			itsDirector->ViewFunctionList(c);
		}
	}
}

#endif

/******************************************************************************
 CopySelectedClassNames

	Copies the names of the selected classes to the text clipboard.

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

void
Tree::CopySelectedClassNames()
	const
{
	JPtrArray<JString> nameList(JPtrArrayT::kForgetAll);

	const JPtrArray<Class>& classList = GetVisibleClasses();
	for (auto* c : classList)
	{
		if (c->IsSelected())
		{
			const JString& name = c->GetFullName();
			nameList.Append(const_cast<JString*>(&name));
		}
	}

	if (!nameList.IsEmpty())
	{
		auto* data = jnew JXTextSelection(itsDirector->GetDisplay(), nameList);
		assert( data != nullptr );

		((itsDirector->GetDisplay())->GetSelectionManager())->SetData(kJXClipboardName, data);
	}
}

#endif

/******************************************************************************
 DeriveFromSelected

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

void
Tree::DeriveFromSelected()
	const
{
	JPtrArray<Class> parentList(JPtrArrayT::kForgetAll);
	if (!GetSelectedClasses(&parentList))
	{
		return;
	}

	if (!JProgramAvailable(kCodeMillProgramName))
	{
		JGetUserNotification()->DisplayMessage(JGetString("RequiresCodeMill::Tree"));
		return;
	}

	// build data files describing ancestor classes

	JString outputPath;
	JPtrArray<JString> argList(JPtrArrayT::kDeleteAll);
	bool success = true;

	JPtrArray<Class> ancestorList(JPtrArrayT::kForgetAll);

	const JSize parentCount = parentList.GetItemCount();
	JString dataFileName, srcFileName;
	for (JIndex i=1; i<=parentCount; i++)
	{
		Class* parent = parentList.GetItem(i);

		ancestorList.RemoveAll();
		CollectAncestors(parent, &ancestorList);
		ancestorList.Prepend(parent);		// first element must be base class

		JError err = JNoError();
		if (!JCreateTempFile(&dataFileName, &err))
		{
			err.ReportIfError();
			success = false;
			break;
		}
		argList.Append(dataFileName);

		std::ofstream output(dataFileName.GetBytes());
		output << kCodeMillDataVersion << std::endl;

		const JSize ancestorCount = ancestorList.GetItemCount();
		output << ancestorCount << std::endl;

		for (JIndex j=1; j<=ancestorCount; j++)
		{
			Class* ancestor = ancestorList.GetItem(j);
			if (ancestor->GetFileName(&srcFileName))
			{
				output << ancestor->GetFullName() << '\t' << srcFileName << std::endl;

				if (outputPath.IsEmpty())
				{
					JString name;
					JSplitPathAndName(srcFileName, &outputPath, &name);
				}
			}
		}

		if (output.fail())
		{
			success = false;
			break;
		}
	}

	// invoke Code Mill

	const JSize fileCount = argList.GetItemCount();
	if (success && fileCount > 0)
	{
		argList.Prepend(JPrepArgForExec(outputPath));
		argList.Prepend("--output_path");
		argList.Prepend("--delete");
		argList.Prepend(kCodeMillProgramName);
		JSimpleProcess::Create(argList, true);
	}
	else if (fileCount > 0)
	{
		for (JIndex i=1; i<=fileCount; i++)
		{
			remove(argList.GetItem(i)->GetBytes());
		}
	}
}

#endif

/******************************************************************************
 CollectAncestors (private)

 ******************************************************************************/

void
Tree::CollectAncestors
	(
	Class* 				cbClass,
	JPtrArray<Class>*	list
	)
	const
{
	const JSize parentCount = cbClass->GetParentCount();
	for (JIndex i=1; i<=parentCount; i++)
	{
		Class* parent;
		if (cbClass->GetParent(i, &parent))
		{
			bool found = false;

			const JSize count = list->GetItemCount();
			for (JIndex j=1; j<=count; j++)
			{
				if (CompareClassFullNames(parent, list->GetItem(j)) ==
					std::weak_ordering::equivalent)
				{
					found = true;
				}
			}

			if (!found)
			{
				list->Append(parent);
				CollectAncestors(parent, list);
			}
		}
	}
}

/******************************************************************************
 GetMenuInfo

 ******************************************************************************/

void
Tree::GetMenuInfo
	(
	bool* hasSelection,
	bool* canCollapse,
	bool* canExpand
	)
	const
{
	*hasSelection = false;
	*canCollapse  = false;
	*canExpand    = false;

	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
	{
		*hasSelection = true;

		const JSize classCount = classList.GetItemCount();
		for (JIndex i=1; i<=classCount; i++)
		{
			Class* c = classList.GetItem(i);
			if (c->IsCollapsed())
			{
				*canExpand = true;
			}
			else if (c->HasPrimaryChildren())
			{
				*canCollapse = true;
			}
		}
	}
}

/******************************************************************************
 CollapseExpandSelectedClasses

 ******************************************************************************/

void
Tree::CollapseExpandSelectedClasses
	(
	const bool collapse
	)
{
	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	if (GetSelectedClasses(&classList))
	{
		bool changed       = false;
		const JSize classCount = classList.GetItemCount();
		for (JIndex i=1; i<=classCount; i++)
		{
			Class* c = classList.GetItem(i);
			if ((!collapse || c->HasPrimaryChildren()) &&
				c->IsCollapsed() != collapse)
			{
				changed = true;
				c->SetCollapsed(collapse);
			}
		}

		if (changed)
		{
			RecalcVisible();
			Broadcast(Changed());
		}
	}
}

/******************************************************************************
 ExpandAllClasses

 ******************************************************************************/

void
Tree::ExpandAllClasses()
{
	bool changed       = false;
	const JSize classCount = itsClassesByFull->GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		Class* c = itsClassesByFull->GetItem(i);
		if (c->IsCollapsed())
		{
			changed = true;
			c->SetCollapsed(false);
		}
	}

	if (changed)
	{
		RecalcVisible();
		Broadcast(Changed());
	}
}

/******************************************************************************
 GetClass

	Return the class that contains the given point.

 ******************************************************************************/

bool
Tree::GetClass
	(
	const JPoint&	pt,
	Class**		theClass
	)
	const
{
	const JSize classCount = itsVisibleByName->GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		Class* c = itsVisibleByName->GetItem(i);
		if (c->Contains(pt))
		{
			*theClass = c;
			return true;
		}
	}

	*theClass = nullptr;
	return false;
}

/******************************************************************************
 HitSameClass

	Return the class that contains both points.

 ******************************************************************************/

bool
Tree::HitSameClass
	(
	const JPoint&	pt1,
	const JPoint&	pt2,
	Class**		theClass
	)
	const
{
	const JSize classCount = itsVisibleByName->GetItemCount();
	for (JIndex i=1; i<=classCount; i++)
	{
		Class* c        = itsVisibleByName->GetItem(i);
		const bool c1 = c->Contains(pt1);
		const bool c2 = c->Contains(pt2);
		if (c1 && c2)
		{
			*theClass = c;
			return true;
		}
		else if (c1 || c2)	// quit now if it contains only one pt
		{
			*theClass = nullptr;
			return false;
		}
	}

	*theClass = nullptr;
	return false;
}

/******************************************************************************
 BroadcastSelectionChange (private)

	Called by Class.

 ******************************************************************************/

void
Tree::BroadcastSelectionChange
	(
	Class*		theClass,
	const bool	isSelected
	)
{
	if (itsBroadcastClassSelFlag && isSelected)
	{
		Broadcast(ClassSelected(theClass));
	}
	else if (itsBroadcastClassSelFlag)
	{
		Broadcast(ClassDeselected(theClass));
	}
}

/******************************************************************************
 Draw

 ******************************************************************************/

void
Tree::Draw
	(
	JPainter&		p,
	const JRect&	rect
	)
	const
{
	p.SetFontSize(itsFontSize);

	if (!itsDrawMILinksOnTopFlag)
	{
		for (auto* c : *itsVisibleByName)
		{
			c->DrawMILinks(p, rect);
		}
	}

	for (auto* c : *itsVisibleByName)
	{
		c->Draw(p, rect);
	}

	if (itsDrawMILinksOnTopFlag)
	{
		for (auto* c : *itsVisibleByName)
		{
			c->DrawMILinks(p, rect);
		}
	}

	for (auto* c : *itsVisibleByName)
	{
		c->DrawText(p, rect);
	}
}

/******************************************************************************
 GetProjectDoc

 ******************************************************************************/

ProjectDocument*
Tree::GetProjectDoc()
	const
{
	return itsDirector->GetProjectDoc();
}

/******************************************************************************
 GetFontManager

	We don't assert in the constructor because TextTree doesn't provide one.

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

JFontManager*
Tree::GetFontManager()
	const
{
	return itsDirector->GetDisplay()->GetFontManager();
}

#endif

/******************************************************************************
 SetFontSize

 ******************************************************************************/

#ifndef CODE_CRUSADER_UNIT_TEST

void
Tree::SetFontSize
	(
	const JSize size
	)
{
	if (size != itsFontSize)
	{
		const JSize origSize = itsFontSize;
		const JFloat h1      = Class::GetTotalHeight(this, GetFontManager());

		itsFontSize = size;

		for (auto* c : *itsClassesByFull)
		{
			c->CalcFrame();
		}

		const JFloat h2 = Class::GetTotalHeight(this, GetFontManager());
		Broadcast(FontSizeChanged(origSize, size, h2/h1));	// scale scrolltabs first
		PlaceAll();
	}
}

#endif

/******************************************************************************
 GetLineHeight

	Get the quantization step size for the vertical placement of classes.

 ******************************************************************************/

JCoordinate
Tree::GetLineHeight()
	const
{
	return itsClassesByFull->GetFirstItem()->GetTotalHeight();
}

/******************************************************************************
 CompareClassFullNames (static private)

 ******************************************************************************/

std::weak_ordering
Tree::CompareClassFullNames
	(
	Class* const & c1,
	Class* const & c2
	)
{
	return JCompareStringsCaseSensitive(
			const_cast<JString*>(&c1->GetFullName()),
			const_cast<JString*>(&c2->GetFullName()));
}

/******************************************************************************
 CompareClassNames (static private)

 ******************************************************************************/

std::weak_ordering
Tree::CompareClassNames
	(
	Class* const & c1,
	Class* const & c2
	)
{
	return JCompareStringsCaseInsensitive(
			const_cast<JString*>(&c1->GetName()),
			const_cast<JString*>(&c2->GetName()));
}

/******************************************************************************
 CompareRGClassPtrs (static private)

	We sort the actual pointers because that's all we need to look up.

 ******************************************************************************/

std::weak_ordering
Tree::CompareRGClassPtrs
	(
	const RootGeom& i1,
	const RootGeom& i2
	)
{
	return i1.root <=> i2.root;
}

/******************************************************************************
 CompareRSContent (static private)

	We compare the boolean content arrays.

 ******************************************************************************/

std::weak_ordering
Tree::CompareRSContent
	(
	const RootSubset& s1,
	const RootSubset& s2
	)
{
	const bool* b1 = s1.content->GetCArray();
	const bool* b2 = s2.content->GetCArray();

	const JSize count = s1.content->GetItemCount();
	for (JIndex i=1; i<=count; i++, b1++, b2++)
	{
		if (!*b1 && *b2)
		{
			return std::weak_ordering::less;
		}
		else if (*b1 && !*b2)
		{
			return std::weak_ordering::greater;
		}
	}

	return std::weak_ordering::equivalent;
}
