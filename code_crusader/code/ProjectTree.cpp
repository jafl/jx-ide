/******************************************************************************
 ProjectTree.cpp

	BASE CLASS = JTree

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "ProjectTree.h"
#include "ProjectNode.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ProjectTree::ProjectTree
	(
	ProjectDocument* doc
	)
	:
	JTree(jnew ProjectNode(nullptr, kRootNT, "root", true))
{
	itsDoc = doc;
}

ProjectTree::ProjectTree
	(
	std::istream&			input,
	const JFileVersion	vers,
	ProjectDocument*	doc
	)
	:
	JTree(ProjectNode::StreamIn(input, vers, nullptr))
{
	itsDoc = doc;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ProjectTree::~ProjectTree()
{
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
ProjectTree::StreamOut
	(
	std::ostream& output
	)
	const
{
	GetProjectRoot()->StreamOut(output);
}

/******************************************************************************
 GetProjectRoot

 ******************************************************************************/

ProjectNode*
ProjectTree::GetProjectRoot()
	const
{
	auto* rootNode = dynamic_cast<ProjectNode*>(const_cast<JTreeNode*>(GetRoot()));
	assert( rootNode != nullptr );
	return rootNode;
}

/******************************************************************************
 BuildMakeFiles

	Returns true if all the source files exist.

 ******************************************************************************/

bool
ProjectTree::BuildMakeFiles
	(
	JString*				text,
	JPtrArray<JTreeNode>*	invalidList,
	JPtrArray<JString>*		libFileList,
	JPtrArray<JString>*		libProjPathList
	)
	const
{
	text->Clear();
	invalidList->RemoveAll();
	GetProjectRoot()->BuildMakeFiles(text, invalidList, libFileList, libProjPathList);
	return invalidList->IsEmpty();
}

/******************************************************************************
 BuildCMakeData

	Returns true if all the source files exist.

 ******************************************************************************/

bool
ProjectTree::BuildCMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	src->Clear();
	hdr->Clear();
	invalidList->RemoveAll();
	GetProjectRoot()->BuildCMakeData(src, hdr, invalidList);
	return invalidList->IsEmpty();
}

/******************************************************************************
 BuildQMakeData

	Returns true if all the source files exist.

 ******************************************************************************/

bool
ProjectTree::BuildQMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	src->Clear();
	hdr->Clear();
	invalidList->RemoveAll();
	GetProjectRoot()->BuildQMakeData(src, hdr, invalidList);
	return invalidList->IsEmpty();
}

/******************************************************************************
 ParseFiles

 ******************************************************************************/

void
ProjectTree::ParseFiles
	(
	FileListTable*				parser,
	const JPtrArray<JString>&	allSuffixList,
	SymbolList*					symbolList,
	const JPtrArray<Tree>&		treeList,
	JProgressDisplay&			pg
	)
	const
{
	GetProjectRoot()->ParseFiles(parser, allSuffixList, symbolList, treeList, pg);
}

/******************************************************************************
 Print

 ******************************************************************************/

void
ProjectTree::Print
	(
	JString* text
	)
	const
{
	GetProjectRoot()->Print(text);
}

/******************************************************************************
 CreateFilesForTemplate

 ******************************************************************************/

void
ProjectTree::CreateFilesForTemplate
	(
	std::istream&			input,
	const JFileVersion	vers
	)
	const
{
	GetProjectRoot()->CreateFilesForTemplate(input, vers);
}

/******************************************************************************
 SaveFilesInTemplate

 ******************************************************************************/

void
ProjectTree::SaveFilesInTemplate
	(
	std::ostream& output
	)
	const
{
	GetProjectRoot()->SaveFilesInTemplate(output);
}
