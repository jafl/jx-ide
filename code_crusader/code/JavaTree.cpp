/******************************************************************************
 JavaTree.cpp

	This class instantiates a Java inheritance tree.

	BASE CLASS = Tree

	Copyright © 1995-2021 John Lindal.

 ******************************************************************************/

#include "JavaTree.h"
#include "JavaClass.h"
#include "JavaTreeDirector.h"
#include "JavaTreeScanner.h"
#include "ProjectDocument.h"
#include "globals.h"
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <fstream>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JavaTree::JavaTree
	(
	JavaTreeDirector*	director,
	const JSize			marginWidth
	)
	:
	Tree(StreamInJavaClass, director, kJavaLang, kJavaSourceFT, marginWidth),
	itsClassNameLexer(nullptr)
{
}

#ifndef CODE_CRUSADER_UNIT_TEST

JavaTree::JavaTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	JavaTreeDirector*	director,
	const JSize			marginWidth,
	DirList*			dirList
	)
	:
	Tree(projInput, projVers, setInput, setVers, symInput, symVers,
		   StreamInJavaClass, director, kJavaLang, kJavaSourceFT, marginWidth, dirList),
	itsClassNameLexer(nullptr)
{
	if (projVers < 88 && !IsEmpty())	// new parser
	{
		NextUpdateMustReparseAll();
	}
}

#endif

/******************************************************************************
 Destructor

 ******************************************************************************/

JavaTree::~JavaTree()
{
	jdelete itsClassNameLexer;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
JavaTree::StreamOut
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
 StreamInJavaClass (static private)

	Creates a jnew JavaClass from the data in the given stream.

 ******************************************************************************/

Class*
JavaTree::StreamInJavaClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	Tree*				tree
	)
{
	auto* newClass = jnew JavaClass(input, vers, tree);
	return newClass;
}

/******************************************************************************
 UpdateFinished (virtual)

	*** This runs in the update fiber.

 ******************************************************************************/

bool
JavaTree::UpdateFinished
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

	Parses the given file and creates JavaClasses.

	We do not create java.lang.Object, because that destroys the ability to
	minimize MI links.

 ******************************************************************************/

void
JavaTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
	{
		itsClassNameLexer = jnew TreeScanner::Java::Scanner;
		assert( itsClassNameLexer != nullptr );
	}

	// extract info about classes

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(TreeScanner::Java::Scanner::INITIAL);

	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	itsClassNameLexer->CreateClasses(id, this, &classList);
}
