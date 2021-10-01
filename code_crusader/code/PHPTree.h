/******************************************************************************
 PHPTree.h

	Copyright © 2014 John Lindal.

 ******************************************************************************/

#ifndef _H_PHPTree
#define _H_PHPTree

#include "Tree.h"

class PHPTreeDirector;
class PHPClass;
class Class;

namespace TreeScanner::PHP { class Scanner; }

class PHPTree : public Tree
{
public:

	PHPTree(PHPTreeDirector* director, const JSize marginWidth);
	PHPTree(std::istream& projInput, const JFileVersion projVers,
			  std::istream* setInput, const JFileVersion setVers,
			  std::istream* symInput, const JFileVersion symVers,
			  PHPTreeDirector* director, const JSize marginWidth,
			  DirList* dirList);

	virtual ~PHPTree();

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const DirList* dirList) const override;

	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	TreeScanner::PHP::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	static Class* StreamInPHPClass(std::istream& input, const JFileVersion vers,
									 Tree* tree);
};

#endif
