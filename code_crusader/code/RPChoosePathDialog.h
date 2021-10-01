/******************************************************************************
 RPChoosePathDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_RPChoosePathDialog
#define _H_RPChoosePathDialog

#include <jx-af/jx/JXChoosePathDialog.h>
#include "RelPathCSF.h"		// need definition of PathType

class JXRadioGroup;

class RPChoosePathDialog : public JXChoosePathDialog
{
public:

	static RPChoosePathDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter,
			   const bool selectOnlyWritable,
			   const RelPathCSF::PathType pathType,
			   const JString& message = JString::empty);

	virtual ~RPChoosePathDialog();

	RelPathCSF::PathType	GetPathType() const;

protected:

	RPChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter,
						 const bool selectOnlyWritable);

private:

// begin JXLayout

	JXRadioGroup* itsPathTypeRG;

// end JXLayout

private:

	void	BuildWindow(const RelPathCSF::PathType pathType,
						const JString& message);

	// not allowed

	RPChoosePathDialog(const RPChoosePathDialog& source);
	const RPChoosePathDialog& operator=(const RPChoosePathDialog& source);
};

#endif
