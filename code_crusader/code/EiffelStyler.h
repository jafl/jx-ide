/******************************************************************************
 EiffelStyler.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_EiffelStyler
#define _H_EiffelStyler

#include "StylerBase.h"
#include "EiffelScanner.h"

class EiffelStyler : public StylerBase, public TextScanner::Eiffel::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~EiffelStyler();

protected:

	EiffelStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

private:

	static EiffelStyler*	itsSelf;
};

#endif
