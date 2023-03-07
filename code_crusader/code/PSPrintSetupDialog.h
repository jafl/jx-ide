/******************************************************************************
 PSPrintSetupDialog.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_PSPrintSetupDialog
#define _H_PSPrintSetupDialog

#include <jx-af/jx/JXPSPrintSetupDialog.h>

class JXFontSizeMenu;

class PSPrintSetupDialog : public JXPSPrintSetupDialog
{
public:

	static PSPrintSetupDialog*
		Create(const JXPSPrinter::Destination dest,
			   const JString& printCmd, const JString& fileName,
			   const bool collate, const bool bw,
			   const JSize fontSize,
			   const bool printHeader);

	~PSPrintSetupDialog() override;

	bool	SetParameters(JXPSPrinter* p) const override;

protected:

	PSPrintSetupDialog();

private:

// begin JXLayout

	JXTextCheckbox* itsPrintHeaderCB;
	JXFontSizeMenu* itsFontSizeMenu;

// end JXLayout

private:

	void	BuildWindow(const JXPSPrinter::Destination dest,
						const JString& printCmd, const JString& fileName,
						const bool collate, const bool bw,
						const JSize fontSize,
						const bool printHeader);
};

#endif
