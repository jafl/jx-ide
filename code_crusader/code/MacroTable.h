/******************************************************************************
 MacroTable.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_MacroTable
#define _H_MacroTable

#include "KeyScriptTableBase.h"

class MacroManager;
class ListCSF;

class MacroTable : public KeyScriptTableBase
{
public:

	MacroTable(EditMacroDialog* dialog,
				 JXTextButton* addRowButton, JXTextButton* removeRowButton,
				 JXTextButton* loadButton, JXTextButton* saveButton,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~MacroTable();

	virtual void	Activate() override;
	virtual void	Deactivate() override;

	void	GetData(MacroManager* mgr) const;
	void	SetData(const MacroManager& mgr);
	void	ClearData();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	ListCSF*		itsCSF;
	JXTextButton*	itsLoadButton;
	JXTextButton*	itsSaveButton;

private:

	void	LoadMacros();
	void	SaveMacros() const;

	void	ReadData(const JString& fileName, const bool replace);
	void	WriteData(const JString& fileName) const;

	// not allowed

	MacroTable(const MacroTable& source);
	const MacroTable& operator=(const MacroTable& source);
};

#endif
