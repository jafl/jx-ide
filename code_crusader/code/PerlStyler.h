/******************************************************************************
 PerlStyler.h

	Copyright © 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_PerlStyler
#define _H_PerlStyler

#include "StylerBase.h"
#include "PerlScanner.h"

class PerlStyler : public StylerBase, public TextScanner::Perl::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~PerlStyler();

protected:

	PerlStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
	void	PreexpandCheckRange(const JString& text,
								const JRunArray<JFont>& styles,
								const JCharacterRange& modifiedRange,
								const bool deletion,
								JStyledText::TextRange* checkRange) override;

	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

private:

	static PerlStyler*	itsSelf;
};

#endif
