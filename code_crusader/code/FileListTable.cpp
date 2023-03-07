/******************************************************************************
 FileListTable.cpp

	Stores a unique ID + the modification time for each file.

	BASE CLASS = JXFileListTable

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "FileListTable.h"
#include "SymbolUpdatePG.h"
#include "ProjectTree.h"
#include "ProjectNode.h"
#include "DirList.h"
#include "SymbolDirector.h"
#include "SymbolList.h"
#include "CTreeDirector.h"
#include "CTree.h"
#include "DTreeDirector.h"
#include "DTree.h"
#include "GoTreeDirector.h"
#include "GoTree.h"
#include "JavaTreeDirector.h"
#include "JavaTree.h"
#include "PHPTreeDirector.h"
#include "PHPTree.h"
#include "globals.h"
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

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
	JXFileListTable(scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsFileInfo = jnew JArray<FileInfo>(1024);
	assert( itsFileInfo != nullptr );

	itsFileUsage              = nullptr;
	itsReparseAllFlag         = false;
	itsChangedDuringParseFlag = false;
	itsLastUniqueID           = JFAID::kMinID;

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

	Update the list of files.

	*** This often runs in the update thread.

 ******************************************************************************/

bool
FileListTable::Update
	(
	std::ostream&		link,
	ProjectTree*		fileTree,
	const DirList&	dirList,
	SymbolDirector*	symbolDir,
	CTreeDirector*	cTreeDir,
	DTreeDirector*	dTreeDir,
	GoTreeDirector*	goTreeDir,
	JavaTreeDirector*	javaTreeDir,
	PHPTreeDirector*	phpTreeDir
	)
{
JIndex i;

	// get everybody ready

	SymbolUpdatePG pg(link, 10);

	SymbolList* symbolList = symbolDir->GetSymbolList();
	CTree* cTree           = cTreeDir->GetCTree();
	DTree* dTree           = dTreeDir->GetDTree();
	GoTree* goTree         = goTreeDir->GetGoTree();
	JavaTree* javaTree     = javaTreeDir->GetJavaTree();
	PHPTree* phpTree       = phpTreeDir->GetPHPTree();

	const bool reparseAll = itsReparseAllFlag             ||
							symbolList->NeedsReparseAll() ||
							cTree->NeedsReparseAll()      ||
							dTree->NeedsReparseAll()      ||
							goTree->NeedsReparseAll()     ||
							javaTree->NeedsReparseAll()   ||
							phpTree->NeedsReparseAll();

	if (reparseAll)
	{
		RemoveAllFiles();
	}
	symbolDir->PrepareForListUpdate(reparseAll, pg);
	cTreeDir->PrepareForTreeUpdate(reparseAll);
	dTreeDir->PrepareForTreeUpdate(reparseAll);
	goTreeDir->PrepareForTreeUpdate(reparseAll);
	javaTreeDir->PrepareForTreeUpdate(reparseAll);
	phpTreeDir->PrepareForTreeUpdate(reparseAll);

	itsChangedDuringParseFlag = reparseAll;

	// create array to track which files still exist

	JArray<bool> fileUsage(1000);
	itsFileUsage = &fileUsage;

	const JSize origFileCount = itsFileInfo->GetElementCount();
	for (i=1; i<=origFileCount; i++)
	{
		fileUsage.AppendElement(false);
	}

	// process each file

	ScanAll(fileTree, dirList, symbolList, cTree, dTree, goTree, javaTree, phpTree, pg);

	// collect files that no longer exist

	const JPtrArray<JString>& fileNameList = GetFullNameList();

	const JSize fileCount = itsFileInfo->GetElementCount();
	JPtrArray<JString> deadFileNameList(JPtrArrayT::kDeleteAll, fileCount+1);	// +1 to avoid passing zero
	JArray<JFAID_t> deadFileIDList(fileCount+1);

	for (i=1; i<=fileCount; i++)
	{
		if (!fileUsage.GetElement(i))
		{
			deadFileNameList.Append(*(fileNameList.GetElement(i)));
			deadFileIDList.AppendElement((itsFileInfo->GetElement(i)).id);
		}
	}

	itsFileUsage = nullptr;

	// remove non-existent files

	RemoveFiles(deadFileNameList);
	symbolDir->ListUpdateFinished(deadFileIDList, pg);
	cTreeDir->TreeUpdateFinished(deadFileIDList);
	dTreeDir->TreeUpdateFinished(deadFileIDList);
	goTreeDir->TreeUpdateFinished(deadFileIDList);
	javaTreeDir->TreeUpdateFinished(deadFileIDList);
	phpTreeDir->TreeUpdateFinished(deadFileIDList);

	if (!deadFileIDList.IsEmpty())
	{
		itsChangedDuringParseFlag = true;
	}

	return itsChangedDuringParseFlag;
}

/******************************************************************************
 ScanAll (private)

	Recursively searches for files to parse, starting from each of the
	given directories.

	We do not recurse into symbolic links because this could produce loops,
	which I'm just too lazy to feel like catching, and the user can always
	add those directories separately.

	*** This often runs in the update thread.

 ******************************************************************************/

void
FileListTable::ScanAll
	(
	ProjectTree*		fileTree,
	const DirList&		dirList,
	SymbolList*			symbolList,
	CTree*				cTree,
	DTree*				dTree,
	GoTree*				goTree,
	JavaTree*			javaTree,
	PHPTree*			phpTree,
	JProgressDisplay&	pg
	)
{
	const JSize dirCount = dirList.GetElementCount();
	if (dirCount > 0 || (fileTree->GetProjectRoot())->HasChildren())
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
							  cTree, dTree, goTree, javaTree, phpTree,
							  pg);
			}
		}

		fileTree->ParseFiles(this, allSuffixList, symbolList,
							 cTree, dTree, goTree, javaTree, phpTree, pg);

		pg.ProcessFinished();
	}
}

/******************************************************************************
 ScanDirectory (private -- recursive)

	*** This often runs in the update thread.

 ******************************************************************************/

void
FileListTable::ScanDirectory
	(
	const JString&				origPath,
	const bool					recurse,
	const JPtrArray<JString>&	allSuffixList,
	SymbolList*					symbolList,
	CTree*						cTree,
	DTree*						dTree,
	GoTree*						goTree,
	JavaTree*					javaTree,
	PHPTree*					phpTree,
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
						  cTree, dTree, goTree, javaTree, phpTree,
						  pg);
		}

		// If it's a file ending in one of the suffixes, parse it.

		else if (entry.IsFile())
		{
			JString trueName = entry.GetFullName();
			time_t modTime   = entry.GetModTime();

			if (entry.IsWorkingLink())
			{
				const bool ok = JGetTrueName(entry.GetFullName(), &trueName);
				assert( ok );
				const JError err = JGetModificationTime(trueName, &modTime);
				assert_ok( err );
			}

			ParseFile(trueName, allSuffixList, modTime,
					  symbolList, cTree, dTree, goTree, javaTree, phpTree);
		}

		pg.IncrementProgress();
	}

	jdelete info;
}

/******************************************************************************
 ParseFile

	Not private because called by FileNode.

	*** This often runs in the update thread.

 ******************************************************************************/

void
FileListTable::ParseFile
	(
	const JString&				fullName,
	const JPtrArray<JString>&	allSuffixList,
	const time_t				modTime,
	SymbolList*					symbolList,
	CTree*						cTree,
	DTree*						dTree,
	GoTree*						goTree,
	JavaTree*					javaTree,
	PHPTree*					phpTree
	)
{
	if (PrefsManager::FileMatchesSuffix(fullName, allSuffixList))
	{
		const TextFileType fileType = GetPrefsManager()->GetFileType(fullName);
		JFAID_t id;
		if (AddFile(fullName, fileType, modTime, &id))
		{
			itsChangedDuringParseFlag = true;

			symbolList->FileChanged(fullName, fileType, id);
			cTree->FileChanged(fullName, fileType, id);
			dTree->FileChanged(fullName, fileType, id);
			goTree->FileChanged(fullName, fileType, id);
			javaTree->FileChanged(fullName, fileType, id);
			phpTree->FileChanged(fullName, fileType, id);
		}
	}
}

/******************************************************************************
 AddFile (private)

	Returns true if the file is new or changed.  *id is always set to
	the id of the file.

	*** This often runs in the update thread.

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
	FileInfo info    = itsFileInfo->GetElement(index);
	*id              = info.id;

	itsFileUsage->SetElement(index, true);
	if (isNew)
	{
		return true;
	}
	else if (modTime != info.modTime)
	{
		info.modTime = modTime;
		itsFileInfo->SetElement(index, info);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 UpdateFinished

	The worker thread updates its data, and we need to do the same.

 ******************************************************************************/

void
FileListTable::UpdateFinished()
{
	itsReparseAllFlag = false;
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
	return *((GetFullNameList()).GetElement(index));
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
	if ((GetFullNameList()).SearchSorted(&target, JListT::kAnyMatch, &index))
	{
		*id = (itsFileInfo->GetElement(index)).id;
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
	const JSize count = itsFileInfo->GetElementCount();
	for (JIndex i=1; i<=count; i++)
	{
		const FileInfo info = itsFileInfo->GetElement(i);
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

	const JFAID_t prevUniqueID = JMax(itsLastUniqueID, (JFAID_t) itsFileInfo->GetElementCount());

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
	if (sender == GetFullNameDataList() && message.Is(JListT::kElementsInserted))
	{
		const auto* info =
			dynamic_cast<const JListT::ElementsInserted*>(&message);
		assert( info != nullptr );
		FilesAdded(*info);
	}
	else if (sender == GetFullNameDataList() && message.Is(JListT::kElementsRemoved))
	{
		const auto* info =
			dynamic_cast<const JListT::ElementsRemoved*>(&message);
		assert( info != nullptr );
		itsFileInfo->RemoveElements(*info);
		if (itsFileUsage != nullptr)
		{
			itsFileUsage->RemoveElements(*info);
		}
	}

	else if (sender == GetFullNameDataList() && message.Is(JListT::kElementMoved))
	{
		const auto* info =
			dynamic_cast<const JListT::ElementMoved*>(&message);
		assert( info != nullptr );
		itsFileInfo->MoveElementToIndex(*info);
		if (itsFileUsage != nullptr)
		{
			itsFileUsage->MoveElementToIndex(*info);
		}
	}
	else if (sender == GetFullNameDataList() && message.Is(JListT::kElementsSwapped))
	{
		const auto* info =
			dynamic_cast<const JListT::ElementsSwapped*>(&message);
		assert( info != nullptr );
		itsFileInfo->SwapElements(*info);
		if (itsFileUsage != nullptr)
		{
			itsFileUsage->SwapElements(*info);
		}
	}
	else if (sender == GetFullNameDataList() && message.Is(JListT::kSorted))
	{
		assert_msg( 0, "FileListTable can't handle full sort of file list" );
	}

	else if (sender == GetFullNameDataList() && message.Is(JListT::kElementsChanged))
	{
		const auto* info =
			dynamic_cast<const JListT::ElementsChanged*>(&message);
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
	const JListT::ElementsInserted& info
	)
{
	const JPtrArray<JString>& fileNameList = GetFullNameList();

	for (JIndex i = info.GetFirstIndex(); i <= info.GetLastIndex(); i++)
	{
		time_t t;
		const JError err = JGetModificationTime(*(fileNameList.GetElement(i)), &t);
		assert_ok( err );

		itsFileInfo->InsertElementAtIndex(i, FileInfo(GetUniqueID(), t));
		if (itsFileUsage != nullptr)
		{
			itsFileUsage->InsertElementAtIndex(i, true);
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
	FileInfo info = itsFileInfo->GetElement(index);

	info.id = GetUniqueID();

	const JString& fileName = *(GetFullNameList().GetElement(index));
	const JError err        = JGetModificationTime(fileName, &(info.modTime));
	assert_ok( err );

	itsFileInfo->SetElement(index, info);
	if (itsFileUsage != nullptr)
	{
		itsFileUsage->SetElement(index, true);
	}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
FileListTable::ReadSetup
	(
	std::istream&			projInput,
	const JFileVersion	projVers,
	std::istream*			symInput,
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
	std::istream&			input,
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

		itsFileInfo->InsertElementAtIndex(index, FileInfo(id, t));
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
		const JSize fileCount = itsFileInfo->GetElementCount();
		*symOutput << ' ' << fileCount;

		const JPtrArray<JString>& fileNameList = GetFullNameList();

		for (JIndex i=1; i<=fileCount; i++)
		{
			*symOutput << ' ' << *(fileNameList.GetElement(i));

			const FileInfo info = itsFileInfo->GetElement(i);
			*symOutput << ' ' << info.id;
			*symOutput << ' ' << info.modTime;
		}

		*symOutput << ' ' << itsLastUniqueID;
		*symOutput << ' ';
	}
}
