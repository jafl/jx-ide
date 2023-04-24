/******************************************************************************
 EditMacroDialog.h

	Interface for the EditMacroDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditMacroDialog
#define _H_EditMacroDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include <jx-af/jcore/JPrefObject.h>
#include "PrefsManager.h"		// for MacroSetInfo

class JXTextButton;
class JXVertPartition;
class MacroSetTable;
class CharActionTable;
class MacroTable;

class EditMacroDialog : public JXModalDialogDirector, public JPrefObject
{
public:

	EditMacroDialog(JArray<PrefsManager::MacroSetInfo>* macroList,
					const JIndex initialSelection,
					const JIndex firstUnusedID);

	~EditMacroDialog() override;

	bool	ContentsValid() const;
	bool	GetCurrentMacroSetName(JString* name) const;

	JArray<PrefsManager::MacroSetInfo>*
		GetMacroList(JIndex* firstNewID, JIndex* lastNewID) const;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	MacroSetTable*		itsMacroSetTable;
	CharActionTable*	itsActionTable;
	MacroTable*			itsMacroTable;

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
};

#endif
