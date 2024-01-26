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

private:

	Class*	itsClass;

// begin JXLayout

	JXTextButton*  itsChooseButton;
	JXStaticText*  itsBaseClassTxt;
	FunctionTable* itsTable;
	JXTextButton*  itsStringsButton;
	JXTextButton*  itsCancelButton;
	JXTextButton*  itsHelpButton;
	JXTextButton*  itsGenerateButton;
	JXInputField*  itsClassInput;
	JXPathInput*   itsDirInput;
	JXInputField*  itsAuthorInput;
	JXInputField*  itsCopyrightInput;

// end JXLayout

private:

	void 	BuildWindow(const JString& outputPath);
	bool 	Write();
};

#endif
