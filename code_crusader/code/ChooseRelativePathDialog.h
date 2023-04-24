/******************************************************************************
 ChooseRelativePathDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_ChooseRelativePathDialog
#define _H_ChooseRelativePathDialog

#include <jx-af/jx/JXChoosePathDialog.h>
#include "ProjectTable.h"		// for PathType

class JXRadioGroup;

class ChooseRelativePathDialog : public JXChoosePathDialog
{
public:

	static ChooseRelativePathDialog*
		Create(const ProjectTable::PathType pathType,
			   const JString& startPath = JString::empty,
			   const JString& fileFilter = JString::empty,
			   const JString& message = JString::empty);

	~ChooseRelativePathDialog() override;

	ProjectTable::PathType	GetPathType() const;

protected:

	ChooseRelativePathDialog(const JString& fileFilter);

private:

// begin JXLayout

	JXRadioGroup* itsPathTypeRG;

// end JXLayout

private:

	void	BuildWindow(const ProjectTable::PathType pathType,
						const JString& startPath,
						const JString& message);
};

#endif
