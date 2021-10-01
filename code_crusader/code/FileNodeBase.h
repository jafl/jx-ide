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

	virtual ~FileNodeBase();

	const JString&	GetFileName() const;
	void			SetFileName(const JString& fileName);

	virtual bool	GetFullName(JString* fullName) const override;

	virtual void		StreamOut(std::ostream& output) const override;
	virtual bool	IncludedInMakefile() const override;
	virtual void		BuildMakeFiles(JString* text,
									   JPtrArray<JTreeNode>* invalidList,
									   JPtrArray<JString>* libFileList,
									   JPtrArray<JString>* libProjPathList) const override;
	virtual bool	IncludedInCMakeData() const override;
	virtual void		BuildCMakeData(JString* src, JString* hdr,
									   JPtrArray<JTreeNode>* invalidList) const override;
	virtual bool	IncludedInQMakeData() const override;
	virtual void		BuildQMakeData(JString* src, JString* hdr,
									   JPtrArray<JTreeNode>* invalidList) const override;
	virtual void		Print(JString* text) const override;
	virtual void		FileRenamed(const JString& origFullName, const JString& newFullName) override;
	virtual void		ShowFileLocation() const override;

	static FileNodeBase*	New(ProjectTree* tree, const JString& fileName);

protected:

	bool	GetFullName(const JString& fileName, JString* fullName) const;
	void		ReportNotFound() const;

	virtual bool	FindFile1(const JString& fullName,
								  ProjectNode** node) override;

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
