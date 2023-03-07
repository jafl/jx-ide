/******************************************************************************
 ChooseRelativeFileDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_ChooseRelativeFileDialog
#define _H_ChooseRelativeFileDialog

#include <jx-af/jx/JXChooseFileDialog.h>
#include "ProjectTable.h"		// need definition of PathType

class JXRadioGroup;

class ChooseRelativeFileDialog : public JXChooseFileDialog
{
public:

	static ChooseRelativeFileDialog*
		Create(const ProjectTable::PathType pathType,
			   const SelectType selectType,
			   const JString& selectName = JString::empty,
			   const JString& fileFilter = JString::empty,
			   const JString& message = JString::empty);

	~ChooseRelativeFileDialog() override;

	ProjectTable::PathType	GetPathType() const;

protected:

	ChooseRelativeFileDialog(const JString& fileFilter);

private:

// begin JXLayout

	JXRadioGroup* itsPathTypeRG;

// end JXLayout

private:

	void	BuildWindow(const ProjectTable::PathType pathType,
						const SelectType selectType,
						const JString& selectName,
						const JString& message);
};

#endif
