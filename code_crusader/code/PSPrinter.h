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

	virtual ~PSPrinter();

	const JString&	GetHeaderName() const;
	void			SetPrintInfo(TextEditor* te, const JString& headerName);

	virtual bool	OpenDocument();
	virtual void	CloseDocument();
	virtual void	CancelDocument();

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual JXPSPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const bool collate, const bool bw);

	virtual bool	EndUserPrintSetup(const JBroadcaster::Message& message,
									  bool* changed);

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
