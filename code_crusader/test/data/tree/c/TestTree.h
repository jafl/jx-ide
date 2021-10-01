/******************************************************************************
 TestTree.h

	Interface for the TestTree Class

	Copyright (C) 1995-99 John Lindal.

 ******************************************************************************/

#ifndef _H_TestTree
#define _H_TestTree

#include "Tree.h"
#include "CtagsUser.h"

class TestTreeScanner;
class TestTreeDirector;
class CPreprocessor;
class CClass;
class Class;

enum FooBar
{
	a, b, c
};

class TestTree : public Tree, public TEST_MACRO
{
public:

	TestTree(TestTreeDirector* director, const JSize marginWidth);
	TestTree(std::istream& projInput, const JFileVersion projVers,
			std::istream* setInput, const JFileVersion setVers,
			std::istream* symInput, const JFileVersion symVers,
			TestTreeDirector* director, const JSize marginWidth,
			DirList* dirList);

	virtual ~TestTree();

	CPreprocessor*	GetCPreprocessor() const;

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const DirList* dirList) const override;

protected:

	virtual bool	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;
	virtual void		ParseFile(const JString& fileName, const JFAID_t id) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CPreprocessor*	itsCPP;
	TestTreeScanner*		itsClassNameLexer;	// nullptr unless parsing

private:

	void	TestTreeX();

	static Class* StreamInCClass(std::istream& input, const JFileVersion vers,
								   Tree* tree);
};


/******************************************************************************
 GetCPreprocessor

 ******************************************************************************/

inline CPreprocessor*
TestTree::GetCPreprocessor()
	const
{
	return itsCPP;
}

#endif
