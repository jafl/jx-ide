/******************************************************************************
 EditStylerDialog.h

	Interface for the EditStylerDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditStylerDialog
#define _H_EditStylerDialog

#include <jx-af/jx/JXDialogDirector.h>
#include "StylerBase.h"		// need definition of WordStyle

class JXTextButton;
class JXTextCheckbox;
class StylerTable;

class EditStylerDialog : public JXDialogDirector
{
public:

	EditStylerDialog(const JString& windowTitle, const bool active,
					   const JUtf8Byte** typeNames,
					   const JArray<JFontStyle>& typeStyles,
					   const JArray<StylerBase::WordStyle>& wordList,
					   const TextFileType fileType);

	~EditStylerDialog() override;

	void	GetData(bool* active, JArray<JFontStyle>* typeStyles,
					JStringMap<JFontStyle>* wordStyles) const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	StylerTable*	itsTypeTable;
	StylerTable*	itsWordTable;

// begin JXLayout

	JXTextCheckbox* itsActiveCB;
	JXTextButton*   itsHelpButton;

// end JXLayout

private:

	void	BuildWindow(const JString& windowTitle, const bool active,
						const JUtf8Byte** typeNames,
						const JArray<JFontStyle>& typeStyles,
						const JArray<StylerBase::WordStyle>& wordList,
						const TextFileType fileType);
};

#endif
