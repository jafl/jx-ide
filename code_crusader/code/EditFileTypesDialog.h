/******************************************************************************
 EditFileTypesDialog.h

	Interface for the EditFileTypesDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditFileTypesDialog
#define _H_EditFileTypesDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "PrefsManager.h"		// for FileTypeInfo

class JXTextButton;
class JXTextCheckbox;
class FileTypeTable;

class EditFileTypesDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	EditFileTypesDialog(const JArray<PrefsManager::FileTypeInfo>& fileTypeList,
						const JArray<PrefsManager::MacroSetInfo>& macroList,
						const JArray<PrefsManager::CRMRuleListInfo>& crmList,
						const bool execOutputWordWrap,
						const bool unknownTypeWordWrap);

	~EditFileTypesDialog() override;

	void	GetFileTypeInfo(JArray<PrefsManager::FileTypeInfo>* fileTypeList,
							bool* execOutputWordWrap,
							bool* unknownTypeWordWrap) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextCheckbox* itsExecOutputWrapCB;
	JXTextCheckbox* itsUnknownTypeWrapCB;
	JXTextButton*   itsHelpButton;
	FileTypeTable*  itsTable;

// end JXLayout

private:

	void	BuildWindow(const JArray<PrefsManager::FileTypeInfo>& fileTypeList,
						const JArray<PrefsManager::MacroSetInfo>& macroList,
						const JArray<PrefsManager::CRMRuleListInfo>& crmList,
						const bool execOutputWordWrap,
						const bool unknownTypeWordWrap);
};

#endif
