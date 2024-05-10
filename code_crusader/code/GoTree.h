/******************************************************************************
 GoTree.h

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_GoTree
#define _H_GoTree

#include "Tree.h"

class GoTreeDirector;
class GoClass;
class Class;

namespace TreeScanner::Go { class Scanner; }

class GoTree : public Tree
{
public:

	GoTree(GoTreeDirector* director, const JSize marginWidth);
	GoTree(std::istream& projInput, const JFileVersion projVers,
			 std::istream* setInput, const JFileVersion setVers,
			 std::istream* symInput, const JFileVersion symVers,
			 GoTreeDirector* director, const JSize marginWidth,
			   DirList* dirList);

	~GoTree() override;

	void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
					  std::ostream* symOutput, const DirList* dirList) const override;

	void	UpdateThreadFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	TreeScanner::Go::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	static Class* StreamInGoClass(std::istream& input, const JFileVersion vers,
									Tree* tree);
};

#endif
