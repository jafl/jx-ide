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

class PSPrinter : public JXPSPrinter, public JPrefObject
{
public:

	PSPrinter(JXDisplay* display);

	~PSPrinter() override;

	const JString&	GetHeaderName() const;
	void			SetPrintInfo(TextEditor* te, const JString& headerName);

	JSize	GetFontSize() const;
	void	SetFontSize(const JSize size);

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

private:

	JSize		itsFontSize;
	TextEditor*	itsTE;			// nullptr unless printing; not owned
	JString		itsHeaderName;	// only used during printing
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
	TextEditor*		te,
	const JString&	headerName
	)
{
	itsTE         = te;
	itsHeaderName = headerName;
}

/******************************************************************************
 Font size

 ******************************************************************************/

inline JSize
PSPrinter::GetFontSize()
	const
{
	return itsFontSize;
}

inline void
PSPrinter::SetFontSize
	(
	const JSize size
	)
{
	itsFontSize = size;
}

#endif
