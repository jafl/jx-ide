/******************************************************************************
 SymbolSRDirector.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_SymbolSRDirector
#define _H_SymbolSRDirector

#include <jx-af/jx/JXWindowDirector.h>

class JError;
class JXTextButton;
class JXTextMenu;
class SymbolList;
class SymbolTable;
class ProjectDocument;
class SymbolDirector;
class CommandMenu;

class SymbolSRDirector : public JXWindowDirector
{
public:

	SymbolSRDirector(SymbolDirector* mainSym, ProjectDocument* projDoc,
					   SymbolList* symbolList,
					   const JString& filterStr, const bool isRegex,
					   JError* err);
	SymbolSRDirector(SymbolDirector* mainSym, ProjectDocument* projDoc,
					   SymbolList* symbolList,
					   const JArray<JIndex>& displayList, const JString& searchStr);

	virtual ~SymbolSRDirector();

	SymbolList*	GetSymbolList() const;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;
	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	SymbolDirector*		itsMainSymDir;		// owns us
	ProjectDocument*	itsProjDoc;			// not owned
	SymbolList*			itsSymbolList;		// not owned
	SymbolTable*		itsSymbolTable;
	JXTextMenu*			itsActionsMenu;
	CommandMenu*		itsCmdMenu;

// begin JXLayout


// end JXLayout

private:

	void	SymbolSRDirectorX(SymbolDirector* owner,
								ProjectDocument* projDoc,
								SymbolList* symbolList,
								const bool focus);
	void	BuildWindow(SymbolList* symbolList, const bool focus);
	void	FitWindowToContent();

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);
};


/******************************************************************************
 GetSymbolList

 ******************************************************************************/

inline SymbolList*
SymbolSRDirector::GetSymbolList()
	const
{
	return itsSymbolList;
}

#endif
