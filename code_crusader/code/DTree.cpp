/******************************************************************************
 DTree.cpp

	This class instantiates a D inheritance tree.

	BASE CLASS = Tree

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#include "DTree.h"
#include "DClass.h"
#include "DTreeDirector.h"
#include "DTreeScanner.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DTree::DTree
	(
	DTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	Tree(StreamInCClass, director, kDLang, kDFT, marginWidth),
	itsClassNameLexer(nullptr)
{
}

#ifndef CODE_CRUSADER_UNIT_TEST

DTree::DTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	DTreeDirector*	director,
	const JSize			marginWidth,
	DirList*			dirList
	)
	:
	Tree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInCClass, director, kDLang, kDFT, marginWidth, dirList),
	itsClassNameLexer(nullptr)
{
}

#endif

/******************************************************************************
 Destructor

 ******************************************************************************/

DTree::~DTree()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
DTree::StreamOut
	(
	std::ostream&		projOutput,
	std::ostream*		setOutput,
	std::ostream*		symOutput,
	const DirList*	dirList
	)
	const
{
	assert( dirList == nullptr );
	Tree::StreamOut(projOutput, setOutput, symOutput, dirList);
}

/******************************************************************************
 StreamInCClass (static private)

	Creates a new DClass from the data in the given stream.

 ******************************************************************************/

Class*
DTree::StreamInCClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	Tree*				tree
	)
{
	auto* newClass = jnew DClass(input, vers, tree);
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This runs in the update fiber.

 ******************************************************************************/

bool
DTree::UpdateFinished
	(
	const JArray<JFAID_t>&	deadFileList,
	JProgressDisplay&		pg
	)
{
	jdelete itsClassNameLexer;
	itsClassNameLexer = nullptr;

	return Tree::UpdateFinished(deadFileList, pg);
}

/******************************************************************************
 ParseFile (virtual protected)

	Parses the given file and creates DClasses.

 ******************************************************************************/

void
DTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
	{
		itsClassNameLexer = jnew TreeScanner::D::Scanner;
	}

	// extract info about classes

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(TreeScanner::D::Scanner::INITIAL);

	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	itsClassNameLexer->CreateClasses(fileName, id, this, &classList);
}
