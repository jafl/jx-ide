/******************************************************************************
 ProjectNode.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_ProjectNode
#define _H_ProjectNode

#include <jx-af/jcore/JNamedTreeNode.h>
#include "ProjectNodeType.h"

class JProgressDisplay;
class JXDocument;
class ProjectTree;
class ProjectDocument;
class FileListTable;
class SymbolList;
class CTree;
class DTree;
class GoTree;
class JavaTree;
class PHPTree;

class ProjectNode : public JNamedTreeNode
{
public:

	ProjectNode(ProjectTree* tree, const ProjectNodeType type,
				  const JString& name, const bool isOpenable);

	~ProjectNode();

	ProjectNodeType	GetType() const;
	virtual bool	IncludedInMakefile() const;
	virtual bool	IncludedInCMakeData() const;
	virtual bool	IncludedInQMakeData() const;

	ProjectDocument*	GetProjectDoc() const;

	virtual void	OpenFile() const;
	virtual void	OpenComplementFile() const;
	virtual bool	GetFullName(JString* fullName) const;
	virtual void	ShowFileLocation() const;
	virtual void	ViewPlainDiffs(const bool silent) const;
	virtual void	ViewVCSDiffs(const bool silent) const;

	bool	Includes(const JString& fullName) const;
	bool	FindFile(const JString& fullName,
						 const ProjectNode** node) const;
	bool	FindFile(const JString& fullName, ProjectNode** node);

	virtual void	BuildMakeFiles(JString* text,
								   JPtrArray<JTreeNode>* invalidList,
								   JPtrArray<JString>* libFileList,
								   JPtrArray<JString>* libProjPathList) const;
	virtual void	BuildCMakeData(JString* src, JString* hdr,
								   JPtrArray<JTreeNode>* invalidList) const;
	virtual void	BuildQMakeData(JString* src, JString* hdr,
								   JPtrArray<JTreeNode>* invalidList) const;

	virtual bool	ParseFiles(FileListTable* parser,
								   const JPtrArray<JString>& allSuffixList,
								   SymbolList* symbolList,
								   CTree* cTree, DTree* dTree, GoTree* goTree,
								   JavaTree* javaTree, PHPTree* phpTree,
								   JProgressDisplay& pg) const;
	virtual void	Print(JString* text) const;

	virtual void	FileRenamed(const JString& origFullName, const JString& newFullName);

	static ProjectNode*	StreamIn(std::istream& input, const JFileVersion vers,
									 ProjectNode* parent);
	virtual void			StreamOut(std::ostream& output) const;

	virtual void	CreateFilesForTemplate(std::istream& input,
										   const JFileVersion vers) const;
	virtual void	SaveFilesInTemplate(std::ostream& output) const;

	ProjectTree*			GetProjectTree();
	const ProjectTree*	GetProjectTree() const;

	ProjectNode*			GetProjectParent();
	const ProjectNode*	GetProjectParent() const;
	bool					GetProjectParent(ProjectNode** parent);
	bool					GetProjectParent(const ProjectNode** parent) const;

	ProjectNode*			GetProjectChild(const JIndex index);
	const ProjectNode*	GetProjectChild(const JIndex index) const;

protected:

	ProjectNode(std::istream& input, const JFileVersion vers,
				  ProjectNode* parent, const ProjectNodeType type,
				  const bool isOpenable);

	virtual bool	FindFile1(const JString& fullName,
								  ProjectNode** node);

private:

	const ProjectNodeType	itsType;
};


/******************************************************************************
 GetType

 ******************************************************************************/

inline ProjectNodeType
ProjectNode::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 Includes

 ******************************************************************************/

inline bool
ProjectNode::Includes
	(
	const JString& fullName
	)
	const
{
	const ProjectNode* node;
	return FindFile(fullName, &node);
}

#endif
