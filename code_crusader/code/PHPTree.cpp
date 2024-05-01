/******************************************************************************
 PHPTree.cpp

	This class instantiates a PHP inheritance tree.

	BASE CLASS = Tree

	Copyright © 2014-2021 John Lindal.

 ******************************************************************************/

#include "PHPTree.h"
#include "PHPClass.h"
#include "PHPTreeDirector.h"
#include "PHPTreeScanner.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <fstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PHPTree::PHPTree
	(
	PHPTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	Tree(StreamInPHPClass, director, kPHPLang, kPHPFT, marginWidth),
	itsClassNameLexer(nullptr)
{
}

#ifndef CODE_CRUSADER_UNIT_TEST

PHPTree::PHPTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	PHPTreeDirector*	director,
	const JSize			marginWidth,
	DirList*			dirList
	)
	:
	Tree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInPHPClass, director, kPHPLang, kPHPFT, marginWidth, dirList),
	itsClassNameLexer(nullptr)
{
	if (projVers < 81 && !IsEmpty())
	{
		NextUpdateMustReparseAll();
	}
}

#endif

/******************************************************************************
 Destructor

 ******************************************************************************/

PHPTree::~PHPTree()
{
	jdelete itsClassNameLexer;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
PHPTree::StreamOut
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
 StreamInPHPClass (static private)

	Creates a new PHPClass from the data in the given stream.

 ******************************************************************************/

Class*
PHPTree::StreamInPHPClass
	(
	std::istream&			input,
	const JFileVersion	vers,
	Tree*				tree
	)
{
	auto* newClass = jnew PHPClass(input, vers, tree);
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This runs in the update fiber.

 ******************************************************************************/

bool
PHPTree::UpdateFinished
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

	Parses the given file and creates one PHPClass.

 ******************************************************************************/

void
PHPTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
	{
		itsClassNameLexer = jnew TreeScanner::PHP::Scanner;
	}

	// extract info about class

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(TreeScanner::PHP::Scanner::INITIAL);

	Class* newClass;
	itsClassNameLexer->CreateClass(id, this, &newClass);
}
