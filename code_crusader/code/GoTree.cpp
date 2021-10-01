/******************************************************************************
 GoTree.cpp

	This class instantiates a Go inheritance tree.

	BASE CLASS = Tree

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#include "GoTree.h"
#include "GoClass.h"
#include "GoTreeDirector.h"
#include "GoTreeScanner.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <fstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GoTree::GoTree
	(
	GoTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	Tree(StreamInGoClass, director, kGoFT, marginWidth),
	itsClassNameLexer(nullptr)
{
}

#ifndef CODE_CRUSADER_UNIT_TEST

GoTree::GoTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	GoTreeDirector*	director,
	const JSize			marginWidth,
	DirList*			dirList
	)
	:
	Tree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInGoClass, director, kGoFT, marginWidth, dirList),
	itsClassNameLexer(nullptr)
{
}

#endif

/******************************************************************************
 Destructor

 ******************************************************************************/

GoTree::~GoTree()
{
	jdelete itsClassNameLexer;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
GoTree::StreamOut
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
 StreamInGoClass (static private)

	Creates a jnew GoClass from the data in the given stream.

 ******************************************************************************/

Class*
GoTree::StreamInGoClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	Tree*				tree
	)
{
	auto* newClass = jnew GoClass(input, vers, tree);
	assert( newClass != nullptr );
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This often runs in the update thread.

 ******************************************************************************/

bool
GoTree::UpdateFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	jdelete itsClassNameLexer;
	itsClassNameLexer = nullptr;

	return Tree::UpdateFinished(deadFileList);
}

/******************************************************************************
 ParseFile (virtual protected)

	Parses the given file and creates GoClasses.

 ******************************************************************************/

void
GoTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
	{
		itsClassNameLexer = jnew TreeScanner::Go::Scanner;
		assert( itsClassNameLexer != nullptr );
	}

	// extract info about classes

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(TreeScanner::Go::Scanner::INITIAL);

	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	itsClassNameLexer->CreateClasses(id, this, &classList);
}
