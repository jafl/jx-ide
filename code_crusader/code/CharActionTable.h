/******************************************************************************
 CharActionTable.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CharActionTable
#define _H_CharActionTable

#include "KeyScriptTableBase.h"

class CharActionManager;
class ListCSF;

class CharActionTable : public KeyScriptTableBase
{
public:

	CharActionTable(EditMacroDialog* dialog,
					  JXTextButton* addRowButton, JXTextButton* removeRowButton,
					  JXTextButton* loadButton, JXTextButton* saveButton,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	~CharActionTable() override;

	void	Activate() override;
	void	Deactivate() override;

	void	GetData(CharActionManager* mgr) const;
	void	SetData(const CharActionManager& mgr);
	void	ClearData();

protected:

	JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ListCSF*		itsCSF;
	JXTextButton*	itsLoadButton;
	JXTextButton*	itsSaveButton;

private:

	void	LoadMacros();
	void	SaveMacros() const;

	void	ReadData(const JString& fileName, const bool replace);
	void	WriteData(const JString& fileName) const;
};

#endif
