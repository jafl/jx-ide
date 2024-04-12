/******************************************************************************
 FileListTable.cpp

	Stores a unique ID + the modification time for each file.

	BASE CLASS = JXFileListTable

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "FileListTable.h"
#include "ProjectTree.h"
#include "ProjectNode.h"
#include "DirList.h"
#include "SymbolDirector.h"
#include "SymbolList.h"
#include "TreeDirector.h"
#include "Tree.h"
#include "globals.h"
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

// JBroadcaster message types

const JUtf8Byte* FileListTable::kUpdateFoundChanges = "UpdateFoundChanges::FileListTable";
const JUtf8Byte* FileListTable::kUpdateDone         = "UpdateDone::FileListTable";

/******************************************************************************
 Constructor

 ******************************************************************************/

FileListTable::FileListTable
	(
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
	JXFileListTable(scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsFileUsage(nullptr),
	itsReparseAllFlag(false),
	itsChangedDuringParseCount(0),
	itsLastUniqueID(JFAID::kMinID)
{
	itsFileInfo = jnew JArray<FileInfo>(1024);

	ListenTo(GetFullNameDataList());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileListTable::~FileListTable()
{
	jdelete itsFileInfo;
}

/******************************************************************************
 Update

	Update the list of files.  Returns the fraction of files updated.

	*** This runs in the update fiber.

 ******************************************************************************/

JFloat
FileListTable::Update
	(
	JProgressDisplay&				pg,
	ProjectTree*					fileTree,
	const DirList&					dirList,
	SymbolDirector*					symbolDir,
	const JPtrArray<TreeDirector>&	treeDirList
	)
{
	itsChangedDuringParseCount = 0;

	// get everybody ready

	SymbolList* symbolList = symbolDir->GetSymbolList();

	JPtrArray<Tree> treeList(JPtrArrayT::kForgetAll);
	for (auto* dir : treeDirList)
	{
		treeList.Append(dir->GetTree());
	}

	const bool reparseAll = itsReparseAllFlag             ||
							symbolList->NeedsReparseAll() ||
							std::any_of(begin(treeList), end(treeList),
								[](Tree* tree){ return tree->NeedsReparseAll(); });

	if (reparseAll)
	{
		RemoveAllFiles();
		Broadcast(UpdateFoundChanges());
	}
	symbolDir->PrepareForListUpdate(reparseAll, pg);
	for (auto* dir : treeDirList)
	{
		dir->PrepareForTreeUpdate(reparseAll);
	}

	// create array to track which files still exist

	JArray<bool> fileUsage(1000);
	itsFileUsage = &fileUsage;

	const JSize origFileCount = itsFileInfo->GetItemCount();
	for (JIndex i=1; i<=origFileCount; i++)
	{
		fileUsage.AppendItem(false);
	}

	// process each file

	ScanAll(fileTree, dirList, symbolList, treeList, pg);

	// collect files that no longer exist

	const JPtrArray<JString>& fileNameList = GetFullNameList();

	const JSize fileCount = itsFileInfo->GetItemCount();
	JPtrArray<JString> deadFileNameList(JPtrArrayT::kDeleteAll, fileCount+1);	// +1 to avoid passing zero
	JArray<JFAID_t> deadFileIDList(fileCount+1);

	for (JIndex i=1; i<=fileCount; i++)
	{
		if (!fileUsage.GetItem(i))
		{
			deadFileNameList.Append(*fileNameList.GetItem(i));
			deadFileIDList.AppendItem(itsFileInfo->GetItem(i).id);
		}
	}

	itsFileUsage = nullptr;

	// remove non-existent files

	if (!deadFileNameList.IsEmpty())
	{
		pg.FixedLengthProcessBeginning(deadFileNameList.GetItemCount() * (2 + treeDirList.GetItemCount()),
			JGetString("CleaningUp::FileListTable"), false, false);
	}

	RemoveFiles(deadFileNameList, &pg);
	symbolDir->ListUpdateFinished(deadFileIDList, pg);
	for (auto* dir : treeDirList)
	{
		dir->TreeUpdateFinished(deadFileIDList, pg);
	}

	if (!deadFileIDList.IsEmpty())
	{
		pg.ProcessFinished();
		if (!reparseAll)
		{
			itsChangedDuringParseCount += deadFileIDList.GetItemCount();
		}
	}

	itsReparseAllFlag = false;
	Broadcast(UpdateDone());

	return reparseAll ? 1.0 : JFloat(itsChangedDuringParseCount) / itsFileInfo->GetItemCount();
}

/******************************************************************************
 ScanAll (private)

	Recursively searches for files to parse, starting from each of the
	given directories.

	We do not recurse into symbolic links because this could produce loops,
	which I'm just too lazy to feel like catching, and the user can always
	add those directories separately.

	*** This runs in the update fiber.

 ******************************************************************************/

void
FileListTable::ScanAll
	(
	ProjectTree*			fileTree,
	const DirList&			dirList,
	SymbolList*				symbolList,
	const JPtrArray<Tree>&	treeList,
	JProgressDisplay&		pg
	)
{
	const JSize dirCount = dirList.GetItemCount();
	if (dirCount > 0 || fileTree->GetProjectRoot()->HasChildren())
	{
		pg.VariableLengthProcessBeginning(JGetString("ParsingFiles::FileListTable"), false, false);

		JPtrArray<JString> allSuffixList(JPtrArrayT::kDeleteAll);
		GetPrefsManager()->GetAllFileSuffixes(&allSuffixList);

		JString fullPath;
		bool recurse;
		for (JIndex i=1; i<=dirCount; i++)
		{
			if (dirList.GetFullPath(i, &fullPath, &recurse))
			{
				ScanDirectory(fullPath, recurse,
							  allSuffixList, symbolList,
							  treeList, pg);
			}
		}

		fileTree->ParseFiles(this, allSuffixList, symbolList, treeList, pg);

		pg.ProcessFinished();
	}
}

/******************************************************************************
 ScanDirectory (private -- recursive)

	*** This runs in the update fiber.

 ******************************************************************************/

void
FileListTable::ScanDirectory
	(
	const JString&				origPath,
	const bool					recurse,
	const JPtrArray<JString>&	allSuffixList,
	SymbolList*					symbolList,
	const JPtrArray<Tree>&		treeList,
	JProgressDisplay&			pg
	)
{
	JString path;
	JDirInfo* info;
	if (!JGetTrueName(origPath, &path) ||
		!JDirInfo::Create(path, &info))
	{
		return;
	}

	const JSize count = info->GetEntryCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JDirEntry& entry = info->GetEntry(i);

		// If it's a directory, recurse.

		if (entry.IsDirectory() && recurse &&
			!entry.IsLink() && !JIsVCSDirectory(entry.GetName()))
		{
			ScanDirectory(entry.GetFullName(), recurse,
						  allSuffixList, symbolList,
						  treeList, pg);
		}

		// If it's a file ending in one of the suffixes, parse it.

		else if (entry.IsFile())
		{
			JString trueName = entry.GetFullName();
			time_t modTime   = entry.GetModTime();

			if (entry.IsWorkingLink())
			{
				bool ok = JGetTrueName(entry.GetFullName(), &trueName);
				assert( ok );
				ok = JGetModificationTime(trueName, &modTime);
				assert( ok );
			}

			ParseFile(trueName, allSuffixList, modTime,  symbolList, treeList, pg);
		}

		pg.IncrementProgress();
	}

	jdelete info;
}

/******************************************************************************
 ParseFile

	Not private because called by FileNode.

	*** This runs in the update fiber.

 ******************************************************************************/

void
FileListTable::ParseFile
	(
	const JString&				fullName,
	const JPtrArray<JString>&	allSuffixList,
	const time_t				modTime,
	SymbolList*					symbolList,
	const JPtrArray<Tree>&		treeList,
	JProgressDisplay&			pg
	)
{
	if (PrefsManager::FileMatchesSuffix(fullName, allSuffixList))
	{
		const TextFileType fileType = GetPrefsManager()->GetFileType(fullName);
		JFAID_t id;
		if (AddFile(fullName, fileType, modTime, &id))
		{
			if (itsChangedDuringParseCount == 0)
			{
				Broadcast(UpdateFoundChanges());
			}
			itsChangedDuringParseCount++;
			pg.ProcessContinuing();

			symbolList->FileChanged(fullName, fileType, id);
			pg.ProcessContinuing();

			for (auto* tree : treeList)
			{
				tree->FileChanged(fullName, fileType, id);
			}
		}
	}
}

/******************************************************************************
 AddFile (private)

	Returns true if the file is new or changed.  *id is always set to
	the id of the file.

	*** This runs in the update fiber.

 ******************************************************************************/

bool
FileListTable::AddFile
	(
	const JString&		fullName,
	const TextFileType	fileType,
	const time_t		modTime,
	JFAID_t*			id
	)
{
	if (ExcludeFromFileList(fileType))
	{
		return false;
	}

	JIndex index;
	const bool isNew = JXFileListTable::AddFile(fullName, &index);
	FileInfo info    = itsFileInfo->GetItem(index);
	*id              = info.id;

	itsFileUsage->SetItem(index, true);
	if (isNew)
	{
		return true;
	}
	else if (modTime != info.modTime)
	{
		info.modTime = modTime;
		itsFileInfo->SetItem(index, info);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 GetFileName

	We assert that the id is valid to save everybody else from having to do it.

 ******************************************************************************/

const JString&
FileListTable::GetFileName
	(
	const JFAID_t id
	)
	const
{
	JIndex index;
	const bool found = IDToIndex(id, &index);
	assert( found );
	return *GetFullNameList().GetItem(index);
}

/******************************************************************************
 GetFileID

 ******************************************************************************/

bool
FileListTable::GetFileID
	(
	const JString&	trueName,
	JFAID_t*		id
	)
	const
{
	JString target(trueName);
	JIndex index;
	if (GetFullNameList().SearchSorted(&target, JListT::kAnyMatch, &index))
	{
		*id = itsFileInfo->GetItem(index).id;
		return true;
	}
	else
	{
		*id = JFAID::kInvalidID;
		return false;
	}
}

/******************************************************************************
 IDToIndex (private)

	Return the index of the file with the specified ID.

 ******************************************************************************/

bool
FileListTable::IDToIndex
	(
	const JFAID_t	id,
	JIndex*			index
	)
	const
{
	const JSize count = itsFileInfo->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const FileInfo info = itsFileInfo->GetItem(i);
		if (info.id == id)
		{
			*index = i;
			return true;
		}
	}

	*index = 0;
	return false;
}

/******************************************************************************
 GetUniqueID (private)

	Return an unused ID.  Return zero if all ID's have been used.

 ******************************************************************************/

JFAID_t
FileListTable::GetUniqueID()
	const
{
	if (itsFileInfo->IsEmpty())
	{
		return JFAID::kMinID;
	}

	const JFAID_t prevUniqueID = JMax(itsLastUniqueID, itsFileInfo->GetItemCount());

	// this is relevant to the outmost do-while loop

	enum Range
	{
		kAboveLastUniqueID,
		kBelowLastUniqueID,
		kEmpty
	};
	Range idRange = kAboveLastUniqueID;

	do
	{
		// try the larger Id's first

		JFAID_t firstId, lastId;

		if (idRange == kAboveLastUniqueID && prevUniqueID < JFAID::kMaxID)
		{
			firstId = prevUniqueID + 1;
			lastId  = JFAID::kMaxID;
		}
		else if (idRange == kAboveLastUniqueID)
		{
			idRange = kBelowLastUniqueID;
			firstId = JFAID::kMinID;
			lastId  = JFAID::kMaxID;
		}
		else
		{
			assert( idRange == kBelowLastUniqueID );
			firstId = JFAID::kMinID;
			lastId  = prevUniqueID;
		}

		// try all possible id's in the given range

		for (JFAID_t id=firstId; id<=lastId; id++)
		{
			JIndex index;
			if (!IDToIndex(id, &index))
			{
				itsLastUniqueID = id;
				return id;
			}
		}

		if (idRange == kAboveLastUniqueID)
		{
			idRange = kBelowLastUniqueID;
		}
		else if (idRange == kBelowLastUniqueID)
		{
			idRange = kEmpty;
		}
	}
		while (idRange != kEmpty);

	// We've tried everything.  It's time to give up.

	return JFAID::kInvalidID;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
FileListTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetFullNameDataList() && message.Is(JListT::kItemsInserted))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemsInserted*>(&message);
		assert( info != nullptr );
		FilesAdded(*info);
	}
	else if (sender == GetFullNameDataList() && message.Is(JListT::kItemsRemoved))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemsRemoved*>(&message);
		assert( info != nullptr );
		itsFileInfo->RemoveItems(*info);
		if (itsFileUsage != nullptr)
		{
			itsFileUsage->RemoveItems(*info);
		}
	}

	else if (sender == GetFullNameDataList() && message.Is(JListT::kItemMoved))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemMoved*>(&message);
		assert( info != nullptr );
		itsFileInfo->MoveItemToIndex(*info);
		if (itsFileUsage != nullptr)
		{
			itsFileUsage->MoveItemToIndex(*info);
		}
	}
	else if (sender == GetFullNameDataList() && message.Is(JListT::kItemsSwapped))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemsSwapped*>(&message);
		assert( info != nullptr );
		itsFileInfo->SwapItems(*info);
		if (itsFileUsage != nullptr)
		{
			itsFileUsage->SwapItems(*info);
		}
	}
	else if (sender == GetFullNameDataList() && message.Is(JListT::kSorted))
	{
		assert_msg( 0, "FileListTable can't handle full sort of file list" );
	}

	else if (sender == GetFullNameDataList() && message.Is(JListT::kItemsChanged))
	{
		const auto* info =
			dynamic_cast<const JListT::ItemsChanged*>(&message);
		assert( info != nullptr );

		for (JIndex i=info->GetFirstIndex(); i<=info->GetLastIndex(); i++)
		{
			UpdateFileInfo(i);
		}
	}

	JXFileListTable::Receive(sender, message);
}

/******************************************************************************
 FilesAdded (private)

 ******************************************************************************/

void
FileListTable::FilesAdded
	(
	const JListT::ItemsInserted& info
	)
{
	const JPtrArray<JString>& fileNameList = GetFullNameList();

	for (JIndex i = info.GetFirstIndex(); i <= info.GetLastIndex(); i++)
	{
		time_t t;
		const bool ok = JGetModificationTime(*fileNameList.GetItem(i), &t);
		assert( ok );

		itsFileInfo->InsertItemAtIndex(i, FileInfo(GetUniqueID(), t));
		if (itsFileUsage != nullptr)
		{
			itsFileUsage->InsertItemAtIndex(i, true);
		}
	}
}

/******************************************************************************
 UpdateFileInfo (private)

	The file name changed, so we generate a new ID.

 ******************************************************************************/

void
FileListTable::UpdateFileInfo
	(
	const JIndex index
	)
{
	FileInfo info = itsFileInfo->GetItem(index);

	info.id = GetUniqueID();

	const JString& fileName = *(GetFullNameList().GetItem(index));
	const bool ok           = JGetModificationTime(fileName, &(info.modTime));
	assert( ok );

	itsFileInfo->SetItem(index, info);
	if (itsFileUsage != nullptr)
	{
		itsFileUsage->SetItem(index, true);
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
FileListTable::ReadSetup
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		symInput,
	const JFileVersion	symVers
	)
{
	std::istream* input          = (projVers <= 41 ? &projInput : symInput);
	const JFileVersion vers = (projVers <= 41 ? projVers   : symVers);
	if (vers < 40 || input == nullptr)
	{
		itsReparseAllFlag = true;
	}
	else
	{
		ReadSetup(*input, vers);
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
FileListTable::ReadSetup
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	RemoveAllFiles();

	JSize fileCount;
	input >> fileCount;

	StopListening(GetFullNameDataList());

	JString fileName;
	for (JIndex i=1; i<=fileCount; i++)
	{
		JFAID_t id;
		time_t t;
		input >> fileName >> id >> t;

		JIndex index;
		const bool isNew = JXFileListTable::AddFile(fileName, &index);
		assert( isNew );

		itsFileInfo->InsertItemAtIndex(index, FileInfo(id, t));
	}

	ListenTo(GetFullNameDataList());

	if (vers >= 80)
	{
		input >> itsLastUniqueID;
	}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
FileListTable::WriteSetup
	(
	std::ostream& projOutput,
	std::ostream* symOutput
	)
	const
{
	if (symOutput != nullptr)
	{
		const JSize fileCount = itsFileInfo->GetItemCount();
		*symOutput << ' ' << fileCount;

		const JPtrArray<JString>& fileNameList = GetFullNameList();

		for (JIndex i=1; i<=fileCount; i++)
		{
			*symOutput << ' ' << *(fileNameList.GetItem(i));

			const FileInfo info = itsFileInfo->GetItem(i);
			*symOutput << ' ' << info.id;
			*symOutput << ' ' << info.modTime;
		}

		*symOutput << ' ' << itsLastUniqueID;
		*symOutput << ' ';
	}
}
