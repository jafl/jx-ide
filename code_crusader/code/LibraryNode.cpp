/******************************************************************************
 LibraryNode.cpp

	BASE CLASS = FileNodeBase

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "LibraryNode.h"
#include "SubprojectConfigDialog.h"
#include "ProjectDocument.h"
#include "BuildManager.h"
#include "globals.h"
#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LibraryNode::LibraryNode
	(
	ProjectTree*	tree,
	const JString&	fileName
	)
	:
	FileNodeBase(tree, kLibraryNT, fileName)
{
	LibraryNodeX();
}

LibraryNode::LibraryNode
	(
	std::istream&			input,
	const JFileVersion	vers,
	ProjectNode*		parent
	)
	:
	FileNodeBase(input, vers, parent, kLibraryNT)
{
	LibraryNodeX();
	if (vers >= 39)
	{
		input >> JBoolFromString(itsIncludeInDepListFlag);
	}
	input >> itsProjFileName;
	if (vers >= 38)
	{
		input >> JBoolFromString(itsShouldBuildFlag);
	}
}

// private

void
LibraryNode::LibraryNodeX()
{
	itsIncludeInDepListFlag = true;
	itsShouldBuildFlag      = true;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LibraryNode::~LibraryNode()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
LibraryNode::StreamOut
	(
	std::ostream& output
	)
	const
{
	FileNodeBase::StreamOut(output);
	output << ' ' << JBoolToString(itsIncludeInDepListFlag);
	output << ' ' << itsProjFileName;
	output << ' ' << JBoolToString(itsShouldBuildFlag) << '\n';
}

/******************************************************************************
 OpenFile (virtual)

 ******************************************************************************/

void
LibraryNode::OpenFile()
	const
{
	auto* me = const_cast<LibraryNode*>(this);
	if (!me->OpenProject())
	{
		me->EditSubprojectConfig();
	}
}

/******************************************************************************
 OpenComplementFile (virtual)

 ******************************************************************************/

void
LibraryNode::OpenComplementFile()
	const
{
}

/******************************************************************************
 IncludedInMakefile (virtual)

 ******************************************************************************/

bool
LibraryNode::IncludedInMakefile()
	const
{
	return itsIncludeInDepListFlag &&
				(GetProjectDoc()->GetBuildManager())->GetMakefileMethod() ==
					BuildManager::kMakemake;
}

/******************************************************************************
 BuildMakeFiles (virtual)

 ******************************************************************************/

void
LibraryNode::BuildMakeFiles
	(
	JString*				text,
	JPtrArray<JTreeNode>*	invalidList,
	JPtrArray<JString>*		libFileList,
	JPtrArray<JString>*		libProjPathList
	)
	const
{
	JString projFullName;
	const bool projExists = GetFullName(itsProjFileName, &projFullName);

	if (itsShouldBuildFlag && projExists &&
		(GetProjectDoc()->GetBuildManager())->GetMakefileMethod() ==
			BuildManager::kMakemake)
	{
		libFileList->Append(GetFileName());

		JString path, name;
		JSplitPathAndName(itsProjFileName, &path, &name);
		libProjPathList->Append(path);

		ProjectDocument* doc;
		if (GetDocumentManager()->ProjectDocumentIsOpen(projFullName, &doc))
		{
			(doc->GetBuildManager())->UpdateMakeFiles();
		}
	}

	// don't complain about non-existent library if we have project to build it

	FileNodeBase::BuildMakeFiles(text,
								   (itsShouldBuildFlag && projExists) ?
										nullptr : invalidList,
								   libFileList, libProjPathList);
}

/******************************************************************************
 EditSubprojectConfig

 ******************************************************************************/

void
LibraryNode::EditSubprojectConfig()
{
	ProjectDocument* doc = GetProjectDoc();

	auto* dlog =
		jnew SubprojectConfigDialog(doc, itsIncludeInDepListFlag,
									itsProjFileName, itsShouldBuildFlag);
	if (dlog->DoDialog())
	{
		bool include, build;
		dlog->GetConfig(&include, &itsProjFileName, &build);

		const bool changed =
			include != itsIncludeInDepListFlag ||
			build   != itsShouldBuildFlag;

		JTree* tree;
		if (changed && GetTree(&tree))
		{
			itsIncludeInDepListFlag = true;		// force rebuild
			tree->BroadcastChange(this);
		}
		itsIncludeInDepListFlag = include;
		itsShouldBuildFlag      = build;
	}
}

/******************************************************************************
 OpenProject

 ******************************************************************************/

bool
LibraryNode::OpenProject
	(
	const bool silent
	)
{
	JString fullName;
	if (GetFullName(itsProjFileName, &fullName))
	{
		ProjectDocument* doc;
		ProjectDocument::Create(fullName, silent, &doc);
		return doc != nullptr;
	}
	else
	{
		return false;
	}
}
