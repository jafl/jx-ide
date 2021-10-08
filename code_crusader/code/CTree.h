/******************************************************************************
 CTree.h

	Interface for the CTree Class

	Copyright © 1995-99 John Lindal.

 ******************************************************************************/

#ifndef _H_CTree
#define _H_CTree

#include "Tree.h"
#include "CtagsUser.h"

class CTreeDirector;
class CPreprocessor;
class CClass;
class Class;

namespace TreeScanner::C { class Scanner; }

class CTree : public Tree, public CtagsUser
{
public:

	CTree(CTreeDirector* director, const JSize marginWidth);
	CTree(std::istream& projInput, const JFileVersion projVers,
			std::istream* setInput, const JFileVersion setVers,
			std::istream* symInput, const JFileVersion symVers,
			CTreeDirector* director, const JSize marginWidth,
			DirList* dirList);

	~CTree() override;

	CPreprocessor*	GetCPreprocessor() const;

	void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
					  std::ostream* symOutput, const DirList* dirList) const override;

	bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	void	ParseFile(const JString& fileName, const JFAID_t id) override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CPreprocessor*				itsCPP;
	TreeScanner::C::Scanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	void	CTreeX();

	static Class* StreamInCClass(std::istream& input, const JFileVersion vers,
								   Tree* tree);
};


/******************************************************************************
 GetCPreprocessor

 ******************************************************************************/

inline CPreprocessor*
CTree::GetCPreprocessor()
	const
{
	return itsCPP;
}

#endif
