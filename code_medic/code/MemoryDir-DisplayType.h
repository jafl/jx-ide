// This file was automatically generated by jx_menu_editor.
// Do not edit it directly!
// Any changes you make will be silently overwritten.

#ifndef _H_MemoryDir_DisplayType
#define _H_MemoryDir_DisplayType

static const JUtf8Byte* kDisplayTypeMenuStr =
"* %i __HexBytes::MemoryDir"
"|* %i __HexShort2Bytes::MemoryDir"
"|* %i __HexWord4Bytes::MemoryDir"
"|* %i __HexLong8Bytes::MemoryDir %l"
"|* %i __CharactersISO88591::MemoryDir %l"
"|* %i __AssemblyCode::MemoryDir"
;

enum {
	____HexBytes=1,
	____HexShort2Bytes,
	____HexWord4Bytes,
	____HexLong8Bytes,
	____CharactersISO88591,
	____AssemblyCode,
};


static void ConfigureDisplayTypeMenu(JXTextMenu* menu, const int offset = 0) {
};

#endif