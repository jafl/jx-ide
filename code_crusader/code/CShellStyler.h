/******************************************************************************
 CShellStyler.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CShellStyler
#define _H_CShellStyler

#include "StylerBase.h"
#include "CShellScanner.h"

class CShellStyler : public StylerBase, public TextScanner::CShell::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~CShellStyler();

protected:

	CShellStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CShellStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);
};

#endif