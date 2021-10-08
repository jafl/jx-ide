/******************************************************************************
 JavaTree.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_JavaTree
#define _H_JavaTree

#include "Tree.h"

class JavaTreeDirector;
class JavaClass;
class Class;

namespace TreeScanner::Java { class Scanner; }

class JavaTree : public Tree
{
public:

	JavaTree(JavaTreeDirector* director, const JSize marginWidth);
	JavaTree(std::istream& projInput, const JFileVersion projVers,
			   std::istream* setInput, const JFileVersion setVers,
			   std::istream* symInput, const JFileVersion symVers,
			   JavaTreeDirector* director, const JSize marginWidth,
			   DirList* dirList);

	~JavaTree();

	void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const DirList* dirList) const override;

	bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	TreeScanner::Java::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	static Class* StreamInJavaClass(std::istream& input, const JFileVersion vers,
									  Tree* tree);
};

#endif
