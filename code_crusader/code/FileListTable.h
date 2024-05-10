/******************************************************************************
 FileListTable.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_FileListTable
#define _H_FileListTable

#include <jx-af/jx/JXFileListTable.h>
#include "TextFileType.h"
#include <jx-af/jcore/JFAIndex.h>
#include <jx-af/jcore/JFAID.h>
#include <jx-af/jcore/jTime.h>

class JProgressDisplay;
class ProjectTree;
class DirList;
class SymbolDirector;
class SymbolList;
class TreeDirector;
class Tree;

class FileListTable : public JXFileListTable
{
public:

	FileListTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~FileListTable() override;

	JFloat	Update(JProgressDisplay& pg,
				   ProjectTree* fileTree, const DirList& dirList,
				   SymbolDirector* symbolDir,
				   const JPtrArray<TreeDirector>& treeDirList);

	const JString&	GetFileName(const JFAID_t id) const;
	bool			GetFileID(const JString& trueName, JFAID_t* id) const;

	void	ReadSetup(std::istream& projInput, const JFileVersion projVers,
					  std::istream* symInput, const JFileVersion symVers);
	void	WriteSetup(std::ostream& projOutput, std::ostream* symOutput) const;

	// for loading updated symbols

	void	ReadSetup(std::istream& input, const JFileVersion vers);

	// called by FileNode

	void	ParseFile(const JString& fullName, const JPtrArray<JString>& allSuffixList,
					  const time_t modTime,
					  SymbolList* symbolList, const JPtrArray<Tree>& treeList,
					  JProgressDisplay& pg);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	struct FileInfo
	{
		JFAID_t	id;
		time_t	modTime;

		FileInfo()
			:
			id(JFAID::kInvalidID), modTime(0)
		{ };

		FileInfo(const JFAID_t id_, const time_t t)
			:
			id(id_), modTime(t)
		{ };
	};

private:

	JArray<FileInfo>*	itsFileInfo;
	JArray<bool>*		itsFileUsage;		// nullptr unless updating files; on stack
	bool				itsReparseAllFlag;	// true => flush all on next update
	JSize				itsChangedDuringParseCount;
	JSize				itsNewOrDeletedDuringParseCount;
	mutable JFAID_t		itsLastUniqueID;

private:

	void	ScanAll(ProjectTree* fileTree, const DirList& dirList,
					SymbolList* symbolList, const JPtrArray<Tree>& treeList,
					JProgressDisplay& pg);
	void	ScanDirectory(const JString& path, const bool recurse,
						  const JPtrArray<JString>& allSuffixList,
						  SymbolList* symbolList, const JPtrArray<Tree>& treeList,
						  JProgressDisplay& pg);
	bool	AddFile(const JString& fullName, const TextFileType fileType,
					const time_t modTime, JFAID_t* id);

	bool	IDToIndex(const JFAID_t id, JIndex* index) const;
	JFAID_t	GetUniqueID() const;

	void	FilesAdded(const JListT::ItemsInserted& info);
	void	UpdateFileInfo(const JIndex index);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kUpdateFoundChanges;
	static const JUtf8Byte* kUpdateDone;

	class UpdateFoundChanges : public JBroadcaster::Message
	{
	public:

		UpdateFoundChanges()
			:
			JBroadcaster::Message(kUpdateFoundChanges)
		{ };
	};

	class UpdateDone : public JBroadcaster::Message
	{
	public:

		UpdateDone()
			:
			JBroadcaster::Message(kUpdateDone)
		{ };
	};
};

#endif
