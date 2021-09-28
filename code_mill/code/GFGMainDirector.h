/******************************************************************************
 GFGMainDirector.h

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GFGMainDirector
#define _H_GFGMainDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

#include <jx-af/jcore/JPtrArray-JString.h>

class JXInputField;
class JXPathInput;
class JXTextButton;
class JXStaticText;
class JXToolBar;

class GFGClass;
class GFGFunctionTable;

class GFGMainDirector : public JXWindowDirector, public JPrefObject
{
public:

public:

	GFGMainDirector(JXDirector* supervisor, const JPtrArray<JString>& argList);

	virtual	~GFGMainDirector();

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	GFGClass*			itsClass;
	GFGFunctionTable*	itsTable;

// begin JXLayout

	JXInputField* itsClassInput;
	JXTextButton* itsChooseButton;
	JXTextButton* itsGenerateButton;
	JXTextButton* itsHelpButton;
	JXTextButton* itsCancelButton;
	JXPathInput*  itsDirInput;
	JXStaticText* itsBaseClassTxt;
	JXInputField* itsAuthorInput;
	JXTextButton* itsStringsButton;
	JXInputField* itsCopyrightInput;

// end JXLayout

private:

	void 	BuildWindow(const JString& outputPath);
	bool 	Write();
};

#endif
