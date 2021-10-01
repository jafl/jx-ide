/******************************************************************************
 TCLStyler.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_TCLStyler
#define _H_TCLStyler

#include "StylerBase.h"
#include "TCLScanner.h"

class TCLStyler : public StylerBase, public TextScanner::TCL::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~TCLStyler();

protected:

	TCLStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static TCLStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);
};

#endif
