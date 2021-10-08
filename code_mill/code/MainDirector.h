/******************************************************************************
 MainDirector.h

	Copyright (C) 2002 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_MainDirector
#define _H_MainDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JPrefObject.h>

#include <jx-af/jcore/JPtrArray-JString.h>

class JXInputField;
class JXPathInput;
class JXTextButton;
class JXStaticText;
class JXToolBar;

class Class;
class FunctionTable;

class MainDirector : public JXWindowDirector, public JPrefObject
{
public:

public:

	MainDirector(JXDirector* supervisor, const JPtrArray<JString>& argList);

	~MainDirector() override;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	Class*			itsClass;
	FunctionTable*	itsTable;

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
