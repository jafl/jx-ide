/******************************************************************************
 PSPrinter.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_PSPrinter
#define _H_PSPrinter

#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JString.h>

class TextEditor;
class PSPrintSetupDialog;

class PSPrinter : public JXPSPrinter, public JPrefObject
{
public:

	PSPrinter(JXDisplay* display);

	~PSPrinter();

	const JString&	GetHeaderName() const;
	void			SetPrintInfo(TextEditor* te, const JString& headerName);

	bool	OpenDocument() override;
	void	CloseDocument() override;
	void	CancelDocument() override;

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	JXPSPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const bool collate, const bool bw) override;

	bool	EndUserPrintSetup(const JBroadcaster::Message& message,
							  bool* changed) override;

private:

	JSize		itsFontSize;
	TextEditor*	itsTE;			// nullptr unless printing; not owned
	JString		itsHeaderName;	// only used during printing

	PSPrintSetupDialog*	itsCBPrintSetupDialog;
};


/******************************************************************************
 GetHeaderName

 ******************************************************************************/

inline const JString&
PSPrinter::GetHeaderName()
	const
{
	return itsHeaderName;
}

/******************************************************************************
 SetPrintInfo

 ******************************************************************************/

inline void
PSPrinter::SetPrintInfo
	(
	TextEditor*	te,
	const JString&	headerName
	)
{
	itsTE         = te;
	itsHeaderName = headerName;
}

#endif
