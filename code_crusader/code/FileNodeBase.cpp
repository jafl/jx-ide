/******************************************************************************
 FileNodeBase.cpp

	BASE CLASS = ProjectNode

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "FileNodeBase.h"
#include "ProjectDocument.h"
#include "ProjectTable.h"
#include "ProjectTree.h"
#include "FileNode.h"
#include "LibraryNode.h"
#include "globals.h"
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FileNodeBase::FileNodeBase
	(
	ProjectTree*			tree,
	const ProjectNodeType	type,
	const JString&			fileName
	)
	:
	ProjectNode(tree, type, JString::empty, false),
	itsFileName(fileName)
{
	JString path, name;
	JSplitPathAndName(fileName, &path, &name);
	SetName(name);
}

FileNodeBase::FileNodeBase
	(
	std::istream&			input,
	const JFileVersion		vers,
	ProjectNode*			parent,
	const ProjectNodeType	type
	)
	:
	ProjectNode(input, vers, parent, type, false)
{
	input >> itsFileName;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileNodeBase::~FileNodeBase()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
FileNodeBase::StreamOut
	(
	std::ostream& output
	)
	const
{
	ProjectNode::StreamOut(output);
	output << itsFileName << '\n';
}

/******************************************************************************
 SetFileName

 ******************************************************************************/

void
FileNodeBase::SetFileName
	(
	const JString& fileName
	)
{
	if (fileName != itsFileName)
	{
		itsFileName = fileName;

		JString path, name;
		JSplitPathAndName(fileName, &path, &name);
		const bool nameChanged = name != GetName();
		SetName(name);

		JTree* tree;
		if (!nameChanged && GetTree(&tree))
		{
			tree->BroadcastChange(this);
		}
	}
}

/******************************************************************************
 GetFullName (virtual)

	Returns true if this file can be found.

 ******************************************************************************/

bool
FileNodeBase::GetFullName
	(
	JString* fullName
	)
	const
{
	return GetFullName(itsFileName, fullName);
}

/******************************************************************************
 GetFullName (protected)

	Returns true if the specified file can be found.

 ******************************************************************************/

bool
FileNodeBase::GetFullName
	(
	const JString&	fileName,
	JString*		fullName
	)
	const
{
	const auto* projTree = dynamic_cast<const ProjectTree*>(GetTree());
	assert( projTree != nullptr );

	const JString& basePath = (projTree->GetProjectDoc())->GetFilePath();
	return !fileName.IsEmpty() &&
				JConvertToAbsolutePath(fileName, basePath, fullName);
}

/******************************************************************************
 CalledByFindFile (virtual protected)

 ******************************************************************************/

bool
FileNodeBase::CalledByFindFile
	(
	const JString&	fullName,
	ProjectNode**	node
	)
{
	JString name;
	if (GetFullName(&name) &&
		JSameDirEntry(name, fullName))
	{
		*node = this;
		return true;
	}
	else
	{
		*node = nullptr;
		return false;
	}
}

/******************************************************************************
 IncludedInMakefile (virtual)

	Derived classes must override if they affect Make.files.

 ******************************************************************************/

bool
FileNodeBase::IncludedInMakefile()
	const
{
	const TextFileType type = GetPrefsManager()->GetFileType(itsFileName);
	return IncludeInMakeFiles(type);
}

/******************************************************************************
 BuildMakeFiles (virtual)

 ******************************************************************************/

void
FileNodeBase::BuildMakeFiles
	(
	JString*				text,
	JPtrArray<JTreeNode>*	invalidList,
	JPtrArray<JString>*		libFileList,
	JPtrArray<JString>*		libProjPathList
	)
	const
{
	if (IncludedInMakefile())
	{
		JString fullName;
		if (invalidList == nullptr || GetFullName(&fullName))
		{
			JString root, suffix;
			JSplitRootAndSuffix(itsFileName, &root, &suffix);
			*text += ".";
			*text += suffix;
			*text += " ";
			*text += root;
			*text += "\n";
		}
		else
		{
			invalidList->Append(const_cast<FileNodeBase*>(this));
		}
	}

	ProjectNode::BuildMakeFiles(text, invalidList, libFileList, libProjPathList);
}

/******************************************************************************
 IncludedInCMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

bool
FileNodeBase::IncludedInCMakeData()
	const
{
	const TextFileType type = GetPrefsManager()->GetFileType(itsFileName);
	return IncludeInCMakeSource(type) || IncludeInCMakeHeader(type);
}

/******************************************************************************
 BuildCMakeData (virtual)

 ******************************************************************************/

void
FileNodeBase::BuildCMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	if (IncludedInCMakeData())
	{
		JString fullName;
		if (GetFullName(&fullName))
		{
			const TextFileType type = GetPrefsManager()->GetFileType(itsFileName);
			if (IncludeInCMakeSource(type))
			{
				src->Append(" ");
				*src += itsFileName;

				JString complName;
				if (GetDocumentManager()->
						GetComplementFile(fullName, type, &complName,
										  GetProjectDoc(), false) &&
					!(GetProjectTree()->GetProjectRoot())->Includes(complName))
				{
					const ProjectTable::PathType pathType =
						ProjectTable::CalcPathType(itsFileName);
					complName = ProjectTable::ConvertToRelativePath(
									complName, GetProjectDoc()->GetFilePath(), pathType);

					hdr->Append(" ");
					*hdr += complName;
				}
			}
			else if (IncludeInCMakeHeader(type))
			{
				hdr->Append(" ");
				*hdr += itsFileName;
			}
		}
		else
		{
			invalidList->Append(const_cast<FileNodeBase*>(this));
		}
	}

	ProjectNode::BuildCMakeData(src, hdr, invalidList);
}

/******************************************************************************
 IncludedInQMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

bool
FileNodeBase::IncludedInQMakeData()
	const
{
	const TextFileType type = GetPrefsManager()->GetFileType(itsFileName);
	return IncludeInQMakeSource(type) || IncludeInQMakeHeader(type);
}

/******************************************************************************
 BuildQMakeData (virtual)

 ******************************************************************************/

void
FileNodeBase::BuildQMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	if (IncludedInQMakeData())
	{
		JString fullName;
		if (GetFullName(&fullName))
		{
			const TextFileType type = GetPrefsManager()->GetFileType(itsFileName);
			if (IncludeInQMakeSource(type))
			{
				src->Append(" ");
				*src += itsFileName;

				JString complName;
				if (GetDocumentManager()->
						GetComplementFile(fullName, type, &complName,
										  GetProjectDoc(), false) &&
					!(GetProjectTree()->GetProjectRoot())->Includes(complName))
				{
					const ProjectTable::PathType pathType =
						ProjectTable::CalcPathType(itsFileName);
					complName = ProjectTable::ConvertToRelativePath(
									complName, GetProjectDoc()->GetFilePath(), pathType);

					hdr->Append(" ");
					*hdr += complName;
				}
			}
			else if (IncludeInQMakeHeader(type))
			{
				hdr->Append(" ");
				*hdr += itsFileName;
			}
		}
		else
		{
			invalidList->Append(const_cast<FileNodeBase*>(this));
		}
	}

	ProjectNode::BuildQMakeData(src, hdr, invalidList);
}

/******************************************************************************
 Print (virtual)

 ******************************************************************************/

void
FileNodeBase::Print
	(
	JString* text
	)
	const
{
	*text += "  ";
	*text += itsFileName;
	*text += "\n";

	ProjectNode::Print(text);
}

/******************************************************************************
 FileRenamed (virtual)

 ******************************************************************************/

void
FileNodeBase::FileRenamed
	(
	const JString& origFullName,
	const JString& newFullName
	)
{
	const ProjectTable::PathType type = ProjectTable::CalcPathType(itsFileName);
	if (type == ProjectTable::kAbsolutePath && itsFileName == origFullName)
	{
		SetFileName(newFullName);
	}
	else if (type == ProjectTable::kHomeRelative)
	{
		JString s;
		const bool ok = JExpandHomeDirShortcut(itsFileName, &s);
		assert( ok );
		if (s == origFullName)
		{
			s = ProjectTable::ConvertToRelativePath(newFullName, JString::empty, type);
			SetFileName(s);
		}
	}
	else
	{
		assert( type == ProjectTable::kProjectRelative );

		const auto* projTree = dynamic_cast<const ProjectTree*>(GetTree());
		assert( projTree != nullptr );

		const JString& basePath = (projTree->GetProjectDoc())->GetFilePath();
		JString s               = JConvertToRelativePath(origFullName, basePath);
		if (itsFileName == s)
		{
			s = ProjectTable::ConvertToRelativePath(newFullName, basePath, type);
			SetFileName(s);
		}
	}

	ProjectNode::FileRenamed(origFullName, newFullName);
}

/******************************************************************************
 ShowFileLocation (virtual)

 ******************************************************************************/

void
FileNodeBase::ShowFileLocation()
	const
{
	JString fullName;
	if (GetFullName(&fullName))
	{
		(JXGetWebBrowser())->ShowFileLocation(fullName);
	}
	else
	{
		ReportNotFound();
	}
}

/******************************************************************************
 ReportNotFound (protected)

 ******************************************************************************/

void
FileNodeBase::ReportNotFound()
	const
{
	const JUtf8Byte* map[] =
	{
		"name", itsFileName.GetBytes()
	};
	const JString msg = JGetString("FileNotFound::FileNodeBase", map, sizeof(map));
	JGetUserNotification()->ReportError(msg);
}

/******************************************************************************
 New (static)

	Creates the appropriate node, based on the file name.

 ******************************************************************************/

FileNodeBase*
FileNodeBase::New
	(
	ProjectTree*	tree,
	const JString&	fileName
	)
{
	FileNodeBase* node = nullptr;

	const TextFileType type = GetPrefsManager()->GetFileType(fileName);
	if (IsLibrary(type))
	{
		node = jnew LibraryNode(tree, fileName);
	}
	else
	{
		node = jnew FileNode(tree, fileName);
	}
	assert( node != nullptr );

	return node;
}
