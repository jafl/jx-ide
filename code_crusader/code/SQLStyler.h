/******************************************************************************
 SQLStyler.h

	Copyright © 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_SQLStyler
#define _H_SQLStyler

#include "StylerBase.h"
#include "SQLScanner.h"

class SQLStyler : public StylerBase, public TextScanner::SQL::Scanner
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();

	virtual ~SQLStyler();

protected:

	SQLStyler();

	void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static SQLStyler*	itsSelf;
};

#endif
