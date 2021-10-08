/******************************************************************************
 BourneShellStyler.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_BourneShellStyler
#define _H_BourneShellStyler

#include "StylerBase.h"
#include "BourneShellScanner.h"

class BourneShellStyler : public StylerBase, public TextScanner::BourneShell::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~BourneShellStyler() override;

protected:

	BourneShellStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

private:

	static BourneShellStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);
};

#endif
