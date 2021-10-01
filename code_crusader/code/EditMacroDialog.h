/******************************************************************************
 EditMacroDialog.h

	Interface for the EditMacroDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditMacroDialog
#define _H_EditMacroDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "PrefsManager.h"		// need definition of MacroSetInfo

class JXTextButton;
class JXVertPartition;
class MacroSetTable;
class CharActionTable;
class MacroTable;

class EditMacroDialog : public JXDialogDirector, public JPrefObject
{
public:

	EditMacroDialog(JArray<PrefsManager::MacroSetInfo>* macroList,
					  const JIndex initialSelection,
					  const JIndex firstUnusedID);

	virtual ~EditMacroDialog();

	bool	ContentsValid() const;
	bool	GetCurrentMacroSetName(JString* name) const;

	JArray<PrefsManager::MacroSetInfo>*
		GetMacroList(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual bool	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	MacroSetTable*	itsMacroSetTable;
	CharActionTable*	itsActionTable;
	MacroTable*		itsMacroTable;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin actionLayout


// end actionLayout

// begin macroLayout


// end macroLayout

// begin macroSetLayout


// end macroSetLayout

private:

	void	BuildWindow(JArray<PrefsManager::MacroSetInfo>* macroList,
						const JIndex initialSelection,
						const JIndex firstUnusedID);

	// not allowed

	EditMacroDialog(const EditMacroDialog& source);
	const EditMacroDialog& operator=(const EditMacroDialog& source);
};

#endif
