/******************************************************************************
 FunctionMenu.h

	Interface for the FunctionMenu class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_FunctionMenu
#define _H_FunctionMenu

#include <jx-af/jx/JXTextMenu.h>
#include "CtagsUser.h"
#include <jx-af/jcore/JString.h>

class JXFileDocument;
class JXTEBase;

class FunctionMenu : public JXTextMenu
{
public:

	FunctionMenu(JXFileDocument* doc, const TextFileType type,
				   JXTEBase* te, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	FunctionMenu(JXFileDocument* doc, const TextFileType type, JXTEBase* te,
				   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	~FunctionMenu() override;

	void	TextChanged(const TextFileType type, const JString& fileName);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXFileDocument*		itsDoc;				// not owned; can be nullptr (Medic)
	TextFileType		itsFileType;
	JXTEBase*			itsTE;				// not owned
	JArray<JIndex>*		itsLineIndexList;
	JArray<Language>*	itsLineLangList;
	JIndex				itsCaretItemIndex;
	bool				itsNeedsUpdate;		// true if must rebuild menu
	bool				itsSortFlag;		// true if menu is currently sorted
	bool				itsIncludeNSFlag;	// true if menu is displaying namespace
	bool				itsPackFlag;		// true if menu is currently packed

	#ifdef CODE_MEDIC
	JString itsFileName;
	#endif

private:

	void	FunctionMenuX(JXFileDocument* doc, const TextFileType type,
							JXTEBase* te);

	void	SetEmptyMenuItems();
	void	UpdateMenu();
	void	HandleSelection(const JIndex index);
};

#endif
