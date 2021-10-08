/******************************************************************************
 FileNodeBase.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_FileNodeBase
#define _H_FileNodeBase

#include "ProjectNode.h"

class FileNodeBase : public ProjectNode
{
public:

	FileNodeBase(ProjectTree* tree, const ProjectNodeType type,
				   const JString& fileName);
	FileNodeBase(std::istream& input, const JFileVersion vers,
				   ProjectNode* parent, const ProjectNodeType type);

	~FileNodeBase();

	const JString&	GetFileName() const;
	void			SetFileName(const JString& fileName);

	bool	GetFullName(JString* fullName) const override;

	void	StreamOut(std::ostream& output) const override;
	bool	IncludedInMakefile() const override;
	void	BuildMakeFiles(JString* text,
						   JPtrArray<JTreeNode>* invalidList,
						   JPtrArray<JString>* libFileList,
						   JPtrArray<JString>* libProjPathList) const override;
	bool	IncludedInCMakeData() const override;
	void	BuildCMakeData(JString* src, JString* hdr,
						   JPtrArray<JTreeNode>* invalidList) const override;
	bool	IncludedInQMakeData() const override;
	void	BuildQMakeData(JString* src, JString* hdr,
						   JPtrArray<JTreeNode>* invalidList) const override;
	void	Print(JString* text) const override;
	void	FileRenamed(const JString& origFullName, const JString& newFullName) override;
	void	ShowFileLocation() const override;

	static FileNodeBase*	New(ProjectTree* tree, const JString& fileName);

protected:

	bool	GetFullName(const JString& fileName, JString* fullName) const;
	void	ReportNotFound() const;

	bool	FindFile1(const JString& fullName, ProjectNode** node) override;

private:

	JString	itsFileName;	// can be relative
};


/******************************************************************************
 GetFileName

	This can be a relative path.

 ******************************************************************************/

inline const JString&
FileNodeBase::GetFileName()
	const
{
	return itsFileName;
}

#endif
