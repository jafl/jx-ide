/******************************************************************************
 RPChooseFileDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_RPChooseFileDialog
#define _H_RPChooseFileDialog

#include <jx-af/jx/JXChooseFileDialog.h>
#include "RelPathCSF.h"		// need definition of PathType

class JXRadioGroup;

class RPChooseFileDialog : public JXChooseFileDialog
{
public:

	static RPChooseFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter,
			   const bool allowSelectMultiple,
			   const RelPathCSF::PathType pathType,
			   const JString& origName, const JString& message);

	~RPChooseFileDialog() override;

	RelPathCSF::PathType	GetPathType() const;

protected:

	RPChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter,
						 const bool allowSelectMultiple);

private:

// begin JXLayout

	JXRadioGroup* itsPathTypeRG;

// end JXLayout

private:

	void	BuildWindow(const RelPathCSF::PathType pathType,
						const JString& origName,
						const JString& message);
};

#endif
