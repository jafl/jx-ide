/******************************************************************************
 EditPrefsDialog.h

	Copyright (C) 2001-10 by John Lindal.

 ******************************************************************************/

#ifndef _H_EditPrefsDialog
#define _H_EditPrefsDialog

#include <jx-af/jx/JXDialogDirector.h>
#include <jx-af/jcore/JPtrArray-JString.h>

class JXInputField;
class JXTextButton;
class DebuggerProgramInput;

class EditPrefsDialog : public JXDialogDirector
{
public:

	EditPrefsDialog(JXDirector* supervisor,
					  const JString& gdbCmd,
					  const JString& jdbCmd,
					  const JString& editFileCmd,
					  const JString& editFileLineCmd,
					  const JPtrArray<JString>& cSourceSuffixes,
					  const JPtrArray<JString>& cHeaderSuffixes,
					  const JPtrArray<JString>& javaSuffixes,
					  const JPtrArray<JString>& phpSuffixes,
					  const JPtrArray<JString>& fortranSuffixes,
					  const JPtrArray<JString>& dSuffixes,
					  const JPtrArray<JString>& goSuffixes);

	virtual ~EditPrefsDialog();

	void	GetPrefs(JString* gdbCmd, JString* jdbCmd,
					 JString* editFileCmd, JString* editFileLineCmd,
					 JPtrArray<JString>* cSourceSuffixes,
					 JPtrArray<JString>* cHeaderSuffixes,
					 JPtrArray<JString>* javaSuffixes,
					 JPtrArray<JString>* phpSuffixes,
					 JPtrArray<JString>* fortranSuffixes,
					 JPtrArray<JString>* dSuffixes,
					 JPtrArray<JString>* goSuffixes) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	DebuggerProgramInput* itsGDBCmd;
	DebuggerProgramInput* itsJDBCmd;
	JXInputField*         itsEditFileCmdInput;
	JXInputField*         itsEditFileLineCmdInput;
	JXInputField*         itsCSourceSuffixInput;
	JXInputField*         itsCHeaderSuffixInput;
	JXInputField*         itsJavaSuffixInput;
	JXTextButton*         itsChooseGDBButton;
	JXTextButton*         itsChooseJDBButton;
	JXInputField*         itsPHPSuffixInput;
	JXInputField*         itsFortranSuffixInput;
	JXInputField*         itsDSuffixInput;
	JXInputField*         itsGoSuffixInput;

// end JXLayout

private:

	void	BuildWindow(const JString& gdbCmd,
						const JString& jdbCmd,
						const JString& editFileCmd,
						const JString& editFileLineCmd,
						const JPtrArray<JString>& cSourceSuffixes,
						const JPtrArray<JString>& cHeaderSuffixes,
						const JPtrArray<JString>& javaSuffixes,
						const JPtrArray<JString>& phpSuffixes,
						const JPtrArray<JString>& fortranSuffixes,
						const JPtrArray<JString>& dSuffixes,
						const JPtrArray<JString>& goSuffixes);

	void	GetSuffixList(JXInputField* inputField, JPtrArray<JString>* list) const;
	void	SetSuffixList(JXInputField* inputField, const JPtrArray<JString>& list);

	void	ChooseDebugger(const JString& name, JXInputField* input);
};

#endif
