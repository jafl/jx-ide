/******************************************************************************
 DTree.h

	Interface for the DTree Class

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_DTree
#define _H_DTree

#include "Tree.h"

class DTreeDirector;
class CClass;
class Class;

namespace TreeScanner::D { class Scanner; }

class DTree : public Tree
{
public:

	DTree(DTreeDirector* director, const JSize marginWidth);
	DTree(std::istream& projInput, const JFileVersion projVers,
			std::istream* setInput, const JFileVersion setVers,
			std::istream* symInput, const JFileVersion symVers,
			DTreeDirector* director, const JSize marginWidth,
			DirList* dirList);

	~DTree() override;

	void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
					  std::ostream* symOutput, const DirList* dirList) const override;

	bool	UpdateFinished(const JArray<JFAID_t>& deadFileList,
							JProgressDisplay& pg) override;

protected:

	void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	TreeScanner::D::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	static Class* StreamInCClass(std::istream& input, const JFileVersion vers,
								   Tree* tree);
};

#endif
