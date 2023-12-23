// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_FileListDir_Actions
#define _H_FileListDir_Actions

static const JUtf8Byte* kActionsMenuStr =
"* %b %i __UseWildcardFilter::FileListDir"
"|* %b %i __UseRegexFilter::FileListDir"
;

enum {
	kShowFilterCmd=1,
	kShowRegexCmd,
};

#ifndef _H_jx_af_image_jx_jx_filter_wildcard
#define _H_jx_af_image_jx_jx_filter_wildcard
#include <jx-af/image/jx/jx_filter_wildcard.xpm>
#endif
#ifndef _H_jx_af_image_jx_jx_filter_regex
#define _H_jx_af_image_jx_jx_filter_regex
#include <jx-af/image/jx/jx_filter_regex.xpm>
#endif

static void ConfigureActionsMenu(JXTextMenu* menu, const int offset = 0) {
	if (offset == 0 && JXMenu::GetDisplayStyle() == JXMenu::kWindowsStyle) {
		menu->SetShortcuts(JString("#a", JString::kNoCopy));
	}
	menu->SetItemImage(kShowFilterCmd + offset, jx_filter_wildcard);
	menu->SetItemImage(kShowRegexCmd + offset, jx_filter_regex);
};

#endif