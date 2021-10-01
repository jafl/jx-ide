/******************************************************************************
 RelPathCSF.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_RelPathCSF
#define _H_RelPathCSF

#include <jx-af/jx/JXChooseSaveFile.h>

class ProjectDocument;
class RPChooseFileDialog;
class RPChoosePathDialog;

class RelPathCSF : public JXChooseSaveFile
{
public:

	// Do not change these values once they are assigned because
	// they are use by ProjectTable in the prefs file.

	// If you add values, be sure to keep ProjectTable::DropFileAction up to date.

	enum PathType
	{
		kAbsolutePath = 1,	// avoid conflict with X atom None when calling ChooseDropAction()
		kProjectRelative,
		kHomeRelative
	};

public:

	RelPathCSF(ProjectDocument* doc);

	virtual ~RelPathCSF();

	PathType	GetPathType() const;
	void		SetPathType(const PathType type);

	const JString&	GetProjectPath() const;

	JString			ConvertToRelativePath(const JString& fullPath);
	static JString	ConvertToRelativePath(const JString& fullPath,
										  const JString& projPath,
										  const PathType pathType);
	static PathType	CalcPathType(const JString& path);

	bool ChooseRelFile(const JString& prompt,
						   const JString& instructions,
						   const JString& origName,
						   JString* name);					// relative

	bool ChooseRelRPath(const JString& prompt,
							const JString& instructions,
							const JString& origPath,
							JString* newPath);				// relative
	bool ChooseRelRWPath(const JString& prompt,
							 const JString& instructions,
							 const JString& origPath,
							 JString* newPath);				// relative

protected:

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool allowSelectMultiple,
						   const JString& origName, const JString& message) override;

	virtual JXChoosePathDialog*
	CreateChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const bool selectOnlyWritable, const JString& message) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	RPChooseFileDialog*	itsFileDialog;
	RPChoosePathDialog*	itsPathDialog;
	PathType				itsPathType;
	ProjectDocument*		itsDoc;

private:

	JString	PrepareForChoose(const JString& origName);
	void	CalcPathType(const JString& path, PathType* type) const;
};


/******************************************************************************
 Path type

 ******************************************************************************/

inline RelPathCSF::PathType
RelPathCSF::GetPathType()
	const
{
	return itsPathType;
}

inline void
RelPathCSF::SetPathType
	(
	const PathType type
	)
{
	itsPathType = type;
}

#endif
