/******************************************************************************
 MakeStyler.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_MakeStyler
#define _H_MakeStyler

#include "StylerBase.h"
#include "MakeScanner.h"

class MakeStyler : public StylerBase, public TextScanner::Make::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~MakeStyler() override;

protected:

	MakeStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

private:

	static MakeStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
	void	StyleEmbeddedVariables(const Token& token);
};

#endif
