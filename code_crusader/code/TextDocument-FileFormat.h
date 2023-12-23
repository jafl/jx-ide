// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_TextDocument_FileFormat
#define _H_TextDocument_FileFormat

static const JUtf8Byte* kFileFormatMenuStr =
"* %r %i UNIXTextFormat::TextDocument"
"|* %r %i MacTextFormat::TextDocument"
"|* %r %i DOSTextFormat::TextDocument"
;

enum {
	kUNIXFmtCmd=1,
	kMacFmtCmd,
	kDOSFmtCmd,
};

#ifndef _H_jcc_unix_format
#define _H_jcc_unix_format
#include "jcc_unix_format.xpm"
#endif
#ifndef _H_jcc_mac_format
#define _H_jcc_mac_format
#include "jcc_mac_format.xpm"
#endif
#ifndef _H_jcc_dos_format
#define _H_jcc_dos_format
#include "jcc_dos_format.xpm"
#endif

static void ConfigureFileFormatMenu(JXTextMenu* menu, const int offset = 0) {
	menu->SetItemImage(kUNIXFmtCmd + offset, jcc_unix_format);
	menu->SetItemImage(kMacFmtCmd + offset, jcc_mac_format);
	menu->SetItemImage(kDOSFmtCmd + offset, jcc_dos_format);
};

#endif