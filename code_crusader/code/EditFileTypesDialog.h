/******************************************************************************
 EditFileTypesDialog.h

	Interface for the EditFileTypesDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditFileTypesDialog
#define _H_EditFileTypesDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "PrefsManager.h"		// need definition of FileTypeInfo

class JXTextButton;
class JXTextCheckbox;
class FileTypeTable;

class EditFileTypesDialog : public JXDialogDirector, public JPrefObject
{
public:

	EditFileTypesDialog(JXDirector* supervisor,
						  const JArray<PrefsManager::FileTypeInfo>& fileTypeList,
						  const JArray<PrefsManager::MacroSetInfo>& macroList,
						  const JArray<PrefsManager::CRMRuleListInfo>& crmList,
						  const bool execOutputWordWrap,
						  const bool unknownTypeWordWrap);

	virtual ~EditFileTypesDialog();

	void	GetFileTypeInfo(JArray<PrefsManager::FileTypeInfo>* fileTypeList,
							bool* execOutputWordWrap,
							bool* unknownTypeWordWrap) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	FileTypeTable*	itsTable;

// begin JXLayout

	JXTextCheckbox* itsExecOutputWrapCB;
	JXTextCheckbox* itsUnknownTypeWrapCB;
	JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JArray<PrefsManager::FileTypeInfo>& fileTypeList,
						const JArray<PrefsManager::MacroSetInfo>& macroList,
						const JArray<PrefsManager::CRMRuleListInfo>& crmList,
						const bool execOutputWordWrap,
						const bool unknownTypeWordWrap);

	// not allowed

	EditFileTypesDialog(const EditFileTypesDialog& source);
	const EditFileTypesDialog& operator=(const EditFileTypesDialog& source);
};

#endif
