/******************************************************************************
 ProjectTree.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_ProjectTree
#define _H_ProjectTree

#include <jx-af/jcore/JTree.h>
#include <jx-af/jcore/JPtrArray.h>

class JString;
class JProgressDisplay;
class ProjectDocument;
class ProjectNode;
class FileListTable;
class SymbolList;
class CTree;
class DTree;
class GoTree;
class JavaTree;
class PHPTree;

class ProjectTree : public JTree
{
public:

	ProjectTree(ProjectDocument* doc);
	ProjectTree(std::istream& input, const JFileVersion vers,
				  ProjectDocument* doc);

	~ProjectTree();

	ProjectDocument*	GetProjectDoc() const;
	ProjectNode*		GetProjectRoot() const;

	bool	BuildMakeFiles(JString* text,
						   JPtrArray<JTreeNode>* invalidList,
						   JPtrArray<JString>* libFileList,
						   JPtrArray<JString>* libProjPathList) const;
	bool	BuildCMakeData(JString* src, JString* hdr,
						   JPtrArray<JTreeNode>* invalidList) const;
	bool	BuildQMakeData(JString* src, JString* hdr,
						   JPtrArray<JTreeNode>* invalidList) const;
	void	ParseFiles(FileListTable* parser,
					   const JPtrArray<JString>& allSuffixList,
					   SymbolList* symbolList,
					   CTree* cTree, DTree* dTree, GoTree* goTree,
					   JavaTree* javaTree, PHPTree* phpTree,
					   JProgressDisplay& pg) const;
	void	Print(JString* text) const;

	void	StreamOut(std::ostream& output) const;

	void	CreateFilesForTemplate(std::istream& input, const JFileVersion vers) const;
	void	SaveFilesInTemplate(std::ostream& output) const;

private:

	ProjectDocument*	itsDoc;
};


/******************************************************************************
 GetProjectDoc

 ******************************************************************************/

inline ProjectDocument*
ProjectTree::GetProjectDoc()
	const
{
	return itsDoc;
}

#endif
