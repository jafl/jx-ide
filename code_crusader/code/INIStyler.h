/******************************************************************************
 INIStyler.h

	Copyright © 2013 by John Lindal.

 ******************************************************************************/

#ifndef _H_INIStyler
#define _H_INIStyler

#include "StylerBase.h"
#include "INIScanner.h"

class INIStyler : public StylerBase, public TextScanner::INI::Scanner
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();

	~INIStyler() override;

protected:

	INIStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
				 std::istream& input, const TokenExtra& initData) override;
	void	UpgradeTypeList(const JFileVersion vers,
							JArray<JFontStyle>* typeStyles) override;

private:

	static INIStyler*	itsSelf;

private:

	void	ExtendCheckRangeForString(const JStyledText::TextRange& tokenRange);
};

#endif
