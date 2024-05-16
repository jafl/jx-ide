/******************************************************************************
 CTree.cpp

	This class instantiates a C++ inheritance tree.

	BASE CLASS = Tree

	Copyright © 1995-99 John Lindal.

 ******************************************************************************/

#include "CTree.h"
#include "CClass.h"
#include "CTreeDirector.h"
#include "CTreeScanner.h"
#include "CPreprocessor.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <fstream>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kCtagsArgs =
	"--format=2 --excmd=number --sort=no "
	"--c-kinds=fp --fields-C++=+{properties} --extras=+q";

/******************************************************************************
 Constructor

 ******************************************************************************/

CTree::CTree
	(
	CTreeDirector*	director,
	const JSize		marginWidth
	)
	:
	Tree(StreamInCClass, director, kCLang, kCHeaderFT, marginWidth),
	CtagsUser(kCtagsArgs),
	itsClassNameLexer(nullptr)
{
	CTreeX();
}

#ifndef CODE_CRUSADER_UNIT_TEST

CTree::CTree
	(
	std::istream&		projInput,
	const JFileVersion	projVers,
	std::istream*		setInput,
	const JFileVersion	setVers,
	std::istream*		symInput,
	const JFileVersion	symVers,
	CTreeDirector*		director,
	const JSize			marginWidth,
	DirList*			dirList
	)
	:
	Tree(projInput, projVers, setInput, setVers, symInput, symVers,
		 StreamInCClass, director, kCLang, kCHeaderFT, marginWidth, dirList),
	CtagsUser(kCtagsArgs),
	itsClassNameLexer(nullptr)
{
	CTreeX();

	if (projVers >= 28)
	{
		itsCPP->ReadSetup(projInput, projVers);
	}

	if (projVers < 99 && !IsEmpty())
	{
		NextUpdateMustReparseAll();
	}
}

#endif

// private

void
CTree::CTreeX()
{
	itsCPP = jnew CPreprocessor;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CTree::~CTree()
{
	jdelete itsCPP;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CTree::StreamOut
	(
	std::ostream&	projOutput,
	std::ostream*	setOutput,
	std::ostream*	symOutput,
	const DirList*	dirList
	)
	const
{
	Tree::StreamOut(projOutput, setOutput, symOutput, dirList);

	itsCPP->WriteSetup(projOutput);
}

/******************************************************************************
 StreamInCClass (static private)

	Creates a new CClass from the data in the given stream.

 ******************************************************************************/

Class*
CTree::StreamInCClass
	(
	std::istream&		input,
	const JFileVersion	vers,
	Tree*				tree
	)
{
	auto* newClass = jnew CClass(input, vers, tree);
	return newClass;
}

/******************************************************************************
 UpdateThreadFinished (virtual)

	*** This runs in the update thread.

 ******************************************************************************/

void
CTree::UpdateThreadFinished
	(
	const JArray<JFAID_t>& deadFileList
	)
{
	jdelete itsClassNameLexer;
	itsClassNameLexer = nullptr;

	DeleteProcess();

	Tree::UpdateThreadFinished(deadFileList);
}

/******************************************************************************
 ParseFile (virtual protected)

	Parses the given file and creates CClasses.

	*** This runs in the update thread.

 ******************************************************************************/

void
CTree::ParseFile
	(
	const JString&	origFileName,
	const JFAID_t	id
	)
{
	if (itsClassNameLexer == nullptr)
	{
		itsClassNameLexer = jnew TreeScanner::C::Scanner;
	}

	// Read in the entire file and apply preprocessor.

	JString fileName = origFileName;

	JString buffer;
	JReadFile(fileName, &buffer);
	if (itsCPP->Preprocess(&buffer))
	{
		JString newFileName;
		if (!JCreateTempFile(&newFileName))
		{
			return;
		}

		fileName = newFileName;

		std::ofstream output(fileName.GetBytes());
		buffer.Print(output);
	}

	// extract info about classes

	JPtrArray<Class> classList(JPtrArrayT::kForgetAll);
	JString data;
	Language lang;

	std::ifstream input(fileName.GetBytes());
	itsClassNameLexer->in(&input);
	itsClassNameLexer->start(TreeScanner::C::Scanner::INITIAL);

	if (itsClassNameLexer->CreateClasses(id, this, &classList) &&
		ProcessFile(fileName, kCHeaderFT, &data, &lang))
	{
		// check for pure virtual via ctags

		icharbufstream input(data.GetBytes(), data.GetByteCount());

		JString name;
		JStringPtrMap<JString> flags(JPtrArrayT::kDeleteAll);
		while (true)
		{
			input >> std::ws;
			while (input.peek() == '!')
			{
				JIgnoreLine(input);
				input >> std::ws;
			}

			name = JReadUntil(input, '\t');			// function name
			if (input.eof() || input.fail())
			{
				break;
			}

			ReadExtensionFlags(input, &flags);		// skips file name and line number

			JString* props;
			if (name.Contains("::") && flags.GetItem("properties", &props) &&
				props->Contains("pure") && props->Contains("virtual"))
			{
				JStringIterator iter(&name, JStringIterator::kStartAtEnd);
				if (iter.Prev("::"))
				{
					iter.RemoveAllNext();
					iter.Invalidate();

					for (auto* c : classList)
					{
						if (name == c->GetFullName())
						{
							c->SetAbstract();
							break;
						}
					}
				}
			}
		}
	}

	if (fileName != origFileName)
	{
		JRemoveFile(fileName);
	}
}

/******************************************************************************
 Receive (virtual protected)

	Required because of multiple inheritance.

 ******************************************************************************/

void
CTree::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	Tree::Receive(sender, message);
	CtagsUser::Receive(sender, message);
}
