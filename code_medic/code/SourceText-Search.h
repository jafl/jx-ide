// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_SourceText_Search
#define _H_SourceText_Search

static const JUtf8Byte* kSearchMenuStr =
"* %i BalanceGroupingSymbol::SourceText %l"
"|* %i GoToLine::SourceText"
"|* %i GoToCurrentLine::SourceText"
;

#include "SourceText-Search-enum.h"

#ifndef _H_jcc_balance_braces
#define _H_jcc_balance_braces
#include "jcc_balance_braces.xpm"
#endif

static void ConfigureSearchMenu(JXTextMenu* menu, const int offset = 0) {
	menu->SetItemImage(kBalanceCmd + offset, jcc_balance_braces);
};

#endif
