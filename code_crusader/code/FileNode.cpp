/******************************************************************************
 FileNode.cpp

	BASE CLASS = FileNodeBase

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "FileNode.h"
#include "ProjectTree.h"
#include "ProjectDocument.h"
#include "FileListTable.h"
#include "DiffFileDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jcore/JProgressDisplay.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FileNode::FileNode
	(
	ProjectTree*	tree,
	const JString&	fileName
	)
	:
	FileNodeBase(tree, kFileNT, fileName)
{
}

FileNode::FileNode
	(
	std::istream&			input,
	const JFileVersion	vers,
	ProjectNode*		parent
	)
	:
	FileNodeBase(input, vers, parent, kFileNT)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileNode::~FileNode()
{
}

/******************************************************************************
 OpenFile (virtual)

 ******************************************************************************/

void
FileNode::OpenFile()
	const
{
	JString fullName;
	if (GetFullName(&fullName))
	{
		GetDocumentManager()->OpenSomething(fullName);
	}
	else
	{
		ReportNotFound();
	}
}

/******************************************************************************
 ParseFiles (virtual)

	*** This runs in the update fiber.

 ******************************************************************************/

bool
FileNode::ParseFiles
	(
	FileListTable*				parser,
	const JPtrArray<JString>&	allSuffixList,
	SymbolList*					symbolList,
	const JPtrArray<Tree>&		treeList,
	JProgressDisplay&			pg
	)
	const
{
	JString fullName, trueName;
	if (GetFullName(&fullName) && JGetTrueName(fullName, &trueName))
	{
		if (!ParseFile(trueName, parser, allSuffixList, symbolList, treeList, pg))
		{
			return false;
		}

		const TextFileType type = GetPrefsManager()->GetFileType(trueName);
		if (GetDocumentManager()->GetComplementFile(trueName, type, &fullName,
														GetProjectDoc(), false) &&
			JGetTrueName(fullName, &trueName) &&
			!ParseFile(trueName, parser, allSuffixList, symbolList, treeList, pg))
		{
			return false;
		}
	}
	return FileNodeBase::ParseFiles(parser, allSuffixList, symbolList, treeList, pg);
}

/******************************************************************************
 ParseFile (private)

	*** This runs in the update fiber.

 ******************************************************************************/

bool
FileNode::ParseFile
	(
	const JString&				fullName,
	FileListTable*				parser,
	const JPtrArray<JString>&	allSuffixList,
	SymbolList*					symbolList,
	const JPtrArray<Tree>&		treeList,
	JProgressDisplay&			pg
	)
	const
{
	time_t t;
	JGetModificationTime(fullName, &t);
	parser->ParseFile(fullName, allSuffixList, t, symbolList, treeList, pg);

	return pg.IncrementProgress();
}

/******************************************************************************
 OpenComplementFile (virtual)

 ******************************************************************************/

void
FileNode::OpenComplementFile()
	const
{
	JString fullName;
	if (GetFullName(&fullName))
	{
		const TextFileType type = GetPrefsManager()->GetFileType(fullName);
		if (type == kHTMLFT || type == kXMLFT)
		{
			JXGetWebBrowser()->ShowFileContent(fullName);
		}
		else
		{
			GetDocumentManager()->OpenComplementFile(fullName, type);
		}
	}
	else
	{
		const JUtf8Byte* map[] =
	{
			"name", GetFileName().GetBytes()
	};
		const JString msg = JGetString("ComplFileNotFound::FileNode", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
	}
}

/******************************************************************************
 ViewPlainDiffs (virtual)

 ******************************************************************************/

void
FileNode::ViewPlainDiffs
	(
	const bool silent
	)
	const
{
	JString fullName;
	if (GetFullName(&fullName))
	{
		GetDiffFileDialog()->ViewDiffs(true, fullName, silent);
	}
	else
	{
		ReportNotFound();
	}
}

/******************************************************************************
 ViewVCSDiffs (virtual)

 ******************************************************************************/

void
FileNode::ViewVCSDiffs
	(
	const bool silent
	)
	const
{
	JString fullName;
	if (GetFullName(&fullName))
	{
		(GetDiffFileDialog())->ViewVCSDiffs(fullName, silent);
	}
	else
	{
		ReportNotFound();
	}
}

/******************************************************************************
 CreateFilesForTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

void
FileNode::CreateFilesForTemplate
	(
	std::istream&			input,
	const JFileVersion	vers
	)
	const
{
	FileNodeBase::CreateFilesForTemplate(input, vers);

	bool exists;
	input >> JBoolFromString(exists);
	if (exists)
	{
		JString relName, data;
		input >> relName >> data;

		JString path, name;
		JSplitPathAndName(relName, &path, &name);

		const auto* projTree = dynamic_cast<const ProjectTree*>(GetTree());
		assert( projTree != nullptr );

		const JString& basePath = (projTree->GetProjectDoc())->GetFilePath();
		path = JCombinePathAndName(basePath, path);

		if (!JDirectoryExists(path))
		{
			JError err = JNoError();
			if (!JCreateDirectory(path, &err))
			{
				const JUtf8Byte* map[] =
				{
					"name", relName.GetBytes(),
					"err",  err.GetMessage().GetBytes()
				};
				const JString msg = JGetString("CreateFileFailedWithError::FileNode", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
				return;
			}
		}

		const JString fullName = JCombinePathAndName(path, name);
		if (JFileExists(fullName))
		{
			const JUtf8Byte* map[] =
			{
				"name", fullName.GetBytes()
			};
			const JString msg = JGetString("WarnReplaceFile::FileNode", map, sizeof(map));
			if (!JGetUserNotification()->AskUserNo(msg))
			{
				return;
			}
		}

		std::ofstream output(fullName.GetBytes());
		if (output.good())
		{
			data.Print(output);
		}
		else
		{
			const JUtf8Byte* map[] =
			{
				"name", relName.GetBytes()
			};
			const JString msg = JGetString("CreateFileFailed::FileNode", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
		}
	}
}

/******************************************************************************
 SaveFilesInTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

#ifdef _J_UNIX
static const JUtf8Byte* kPathPrefix = "./";
#endif

void
FileNode::SaveFilesInTemplate
	(
	std::ostream& output
	)
	const
{
	FileNodeBase::SaveFilesInTemplate(output);

	const JString& relName = GetFileName();
	JString fullName;
	if (relName.StartsWith(kPathPrefix) && GetFullName(&fullName))
	{
		JString data;
		JReadFile(fullName, &data);
		output << JBoolToString(true) << ' ' << relName << ' ' << data;
	}
	else
	{
		output << JBoolToString(false);
	}
}
