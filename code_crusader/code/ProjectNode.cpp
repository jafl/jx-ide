/******************************************************************************
 ProjectNode.cpp

	Base class for all items in a project window.

	BASE CLASS = JNamedTreeNode

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#include "ProjectNode.h"
#include "ProjectTree.h"
#include "GroupNode.h"
#include "FileNode.h"
#include "LibraryNode.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ProjectNode::ProjectNode
	(
	ProjectTree*			tree,
	const ProjectNodeType	type,
	const JString&			name,
	const bool			isOpenable
	)
	:
	JNamedTreeNode(tree, name, isOpenable),
	itsType(type)
{
}

// protected

ProjectNode::ProjectNode
	(
	std::istream&			input,
	const JFileVersion		vers,
	ProjectNode*			parent,
	const ProjectNodeType	type,
	const bool			isOpenable
	)
	:
	JNamedTreeNode(nullptr, JString::empty, isOpenable),
	itsType(type)
{
	if (parent != nullptr)
	{
		parent->Append(this);
	}

	JString name;
	input >> name;
	SetName(name);

	if (vers <= 71)
	{
		JSize childCount;
		input >> childCount;

		for (JIndex i=1; i<=childCount; i++)
		{
			StreamIn(input, vers, this);		// connects itself
		}
	}
	else
	{
		while (true)
		{
			bool keepGoing;
			input >> JBoolFromString(keepGoing);
			if (!keepGoing)
			{
				break;
			}

			StreamIn(input, vers, this);
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ProjectNode::~ProjectNode()
{
}

/******************************************************************************
 GetProjectDoc

 ******************************************************************************/

ProjectDocument*
ProjectNode::GetProjectDoc()
	const
{
	const auto* tree = dynamic_cast<const ProjectTree*>(GetTree());
	assert( tree != nullptr );
	return tree->GetProjectDoc();
}

/******************************************************************************
 StreamIn (static)

 ******************************************************************************/

ProjectNode*
ProjectNode::StreamIn
	(
	std::istream&			input,
	const JFileVersion	vers,
	ProjectNode*		parent
	)
{
	ProjectNodeType type;
	input >> type;

	ProjectNode* node = nullptr;
	if (type == kRootNT)
	{
		assert( parent == nullptr );
		node = jnew ProjectNode(input, vers, parent, kRootNT, true);
	}
	else if (type == kGroupNT)
	{
		node = jnew GroupNode(input, vers, parent);
	}
	else if (type == kFileNT)
	{
		node = jnew FileNode(input, vers, parent);
	}
	else if (type == kLibraryNT)
	{
		node = jnew LibraryNode(input, vers, parent);
	}
	assert( node != nullptr );

	return node;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
ProjectNode::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << itsType;
	output << ' ' << GetName() << '\n';

	const JSize childCount = GetChildCount();
	for (JIndex i=1; i<=childCount; i++)
	{
		const auto* child = dynamic_cast<const ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		output << JBoolToString(true) << '\n';
		child->StreamOut(output);
	}

	output << JBoolToString(false) << '\n';
}

/******************************************************************************
 OpenFile (virtual)

 ******************************************************************************/

void
ProjectNode::OpenFile()
	const
{
}

/******************************************************************************
 OpenComplementFile (virtual)

 ******************************************************************************/

void
ProjectNode::OpenComplementFile()
	const
{
}

/******************************************************************************
 GetFullName (virtual)

	Returns true if this is a file that can be found.

 ******************************************************************************/

bool
ProjectNode::GetFullName
	(
	JString* fullName
	)
	const
{
	fullName->Clear();
	return false;
}

/******************************************************************************
 ShowFileLocation (virtual)

	Don't automatically propagate, to avoid execing twice when group and
	contained file are both selected.

 ******************************************************************************/

void
ProjectNode::ShowFileLocation()
	const
{
}

/******************************************************************************
 ViewPlainDiffs (virtual)

	Don't automatically propagate, to avoid diffing twice when group and
	contained file are both selected.

 ******************************************************************************/

void
ProjectNode::ViewPlainDiffs
	(
	const bool silent
	)
	const
{
}

/******************************************************************************
 ViewVCSDiffs (virtual)

	Don't automatically propagate, to avoid diffing twice when group and
	contained file are both selected.

 ******************************************************************************/

void
ProjectNode::ViewVCSDiffs
	(
	const bool silent
	)
	const
{
}

/******************************************************************************
 FindFile

 ******************************************************************************/

bool
ProjectNode::FindFile
	(
	const JString&		fullName,
	const ProjectNode**	node
	)
	const
{
	return const_cast<ProjectNode*>(this)->
		FindFile(fullName, const_cast<ProjectNode**>(node));
}

bool
ProjectNode::FindFile
	(
	const JString&	fullName,
	ProjectNode**	node
	)
{
	if (JFileExists(fullName))
	{
		return CalledByFindFile(fullName, node);
	}
	else
	{
		*node = nullptr;
		return false;
	}
}

/******************************************************************************
 CalledByFindFile (virtual protected)

	Derived classes must override to check if they are the specified file.

 ******************************************************************************/

bool
ProjectNode::CalledByFindFile
	(
	const JString&	fullName,
	ProjectNode**	node
	)
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto* child = dynamic_cast<ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		if (child->CalledByFindFile(fullName, node))
		{
			return true;
		}
	}

	*node = nullptr;
	return false;
}

/******************************************************************************
 BuildMakeFiles (virtual)

	If all source files existed, text contains the text for Make.files.
	Otherwise, invalidList is not empty.

	Derived classes must override to add themselves to the list if they are
	a nonexistent source file.

 ******************************************************************************/

void
ProjectNode::BuildMakeFiles
	(
	JString*				text,
	JPtrArray<JTreeNode>*	invalidList,
	JPtrArray<JString>*		libFileList,
	JPtrArray<JString>*		libProjPathList
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		const auto* child = dynamic_cast<const ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->BuildMakeFiles(text, invalidList, libFileList, libProjPathList);
	}
}

/******************************************************************************
 BuildCMakeData (virtual)

	If all source files existed, strings contain the text for CMakeLists.txt
	file.  Otherwise, invalidList is not empty.

	Derived classes must override to add themselves to the list if they are
	a nonexistent source file.

 ******************************************************************************/

void
ProjectNode::BuildCMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		const auto* child = dynamic_cast<const ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->BuildCMakeData(src, hdr, invalidList);
	}
}

/******************************************************************************
 BuildQMakeData (virtual)

	If all source files existed, strings contain the text for .pro file.
	Otherwise, invalidList is not empty.

	Derived classes must override to add themselves to the list if they are
	a nonexistent source file.

 ******************************************************************************/

void
ProjectNode::BuildQMakeData
	(
	JString*				src,
	JString*				hdr,
	JPtrArray<JTreeNode>*	invalidList
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		const auto* child = dynamic_cast<const ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->BuildQMakeData(src, hdr, invalidList);
	}
}

/******************************************************************************
 ParseFiles (virtual)

	If the derived class represents a file, it should pass it to the parser.
	Returns false if process was cancelled by user.

 ******************************************************************************/

bool
ProjectNode::ParseFiles
	(
	FileListTable*			parser,
	const JPtrArray<JString>&	allSuffixList,
	SymbolList*				symbolList,
	CTree*					cTree,
	DTree*					dTree,
	GoTree*					goTree,
	JavaTree*					javaTree,
	PHPTree*					phpTree,
	JProgressDisplay&			pg
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		const auto* child = dynamic_cast<const ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		if (!child->ParseFiles(parser, allSuffixList, symbolList,
							   cTree, dTree, goTree, javaTree, phpTree, pg))
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 Print (virtual)

	Derived classes must override to add themselves to the text.

 ******************************************************************************/

void
ProjectNode::Print
	(
	JString* text
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		const auto* child = dynamic_cast<const ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->Print(text);
	}
}

/******************************************************************************
 FileRenamed (virtual)

 ******************************************************************************/

void
ProjectNode::FileRenamed
	(
	const JString& origFullName,
	const JString& newFullName
	)
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		auto* child = dynamic_cast<ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->FileRenamed(origFullName, newFullName);
	}
}

/******************************************************************************
 IncludedInMakefile (virtual)

	Derived classes must override if they affect Make.files.

 ******************************************************************************/

bool
ProjectNode::IncludedInMakefile()
	const
{
	return false;
}

/******************************************************************************
 IncludedInCMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

bool
ProjectNode::IncludedInCMakeData()
	const
{
	return false;
}

/******************************************************************************
 IncludedInQMakeData (virtual)

	Derived classes must override if they affect .pro file.

 ******************************************************************************/

bool
ProjectNode::IncludedInQMakeData()
	const
{
	return false;
}

/******************************************************************************
 CreateFilesForTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

void
ProjectNode::CreateFilesForTemplate
	(
	std::istream&			input,
	const JFileVersion	vers
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		const auto* child = dynamic_cast<const ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->CreateFilesForTemplate(input, vers);
	}
}

/******************************************************************************
 SaveFilesInTemplate (virtual)

	Derived classes must override to create their files.

 ******************************************************************************/

void
ProjectNode::SaveFilesInTemplate
	(
	std::ostream& output
	)
	const
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
	{
		const auto* child = dynamic_cast<const ProjectNode*>(GetChild(i));
		assert( child != nullptr );

		child->SaveFilesInTemplate(output);
	}
}

/******************************************************************************
 GetProjectTree

 ******************************************************************************/

ProjectTree*
ProjectNode::GetProjectTree()
{
	auto* tree = dynamic_cast<ProjectTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

const ProjectTree*
ProjectNode::GetProjectTree()
	const
{
	const auto* tree = dynamic_cast<const ProjectTree*>(GetTree());
	assert (tree != nullptr);
	return tree;
}

/******************************************************************************
 GetProjectParent

 ******************************************************************************/

ProjectNode*
ProjectNode::GetProjectParent()
{
	JTreeNode* p     = GetParent();
	auto* n = dynamic_cast<ProjectNode*>(p);
	assert( n != nullptr );
	return n;
}

const ProjectNode*
ProjectNode::GetProjectParent()
	const
{
	const JTreeNode* p     = GetParent();
	const auto* n = dynamic_cast<const ProjectNode*>(p);
	assert( n != nullptr );
	return n;
}

bool
ProjectNode::GetProjectParent
	(
	ProjectNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
	{
		*parent = dynamic_cast<ProjectNode*>(p);
		assert( *parent != nullptr );
		return true;
	}
	else
	{
		*parent = nullptr;
		return false;
	}
}

bool
ProjectNode::GetProjectParent
	(
	const ProjectNode** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
	{
		*parent = dynamic_cast<const ProjectNode*>(p);
		assert( *parent != nullptr );
		return true;
	}
	else
	{
		*parent = nullptr;
		return false;
	}
}

/******************************************************************************
 GetProjectChild

 ******************************************************************************/

ProjectNode*
ProjectNode::GetProjectChild
	(
	const JIndex index
	)
{
	auto* node = dynamic_cast<ProjectNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

const ProjectNode*
ProjectNode::GetProjectChild
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const ProjectNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}
