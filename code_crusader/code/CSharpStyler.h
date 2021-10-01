/******************************************************************************
 CSharpStyler.h

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_CSharpStyler
#define _H_CSharpStyler

#include "StylerBase.h"
#include "CSharpScanner.h"

class CSharpStyler : public StylerBase, public TextScanner::CSharp::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~CSharpStyler();

protected:

	CSharpStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CSharpStyler*	itsSelf;

private:

	bool	SlurpPPComment(JStyledText::TextRange* totalRange);
};

#endif
