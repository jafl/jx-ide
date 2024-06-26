/******************************************************************************
 FortranCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "FortranCompleter.h"
#include <jx-af/jcore/jAssert.h>

FortranCompleter* FortranCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"abs", "accept", "access", "achar", "acos", "acosd", "action", "adjustl",
	"adjustr", "aimag", "aimax0", "aimin0", "aint", "ajmax0", "ajmin0", "all",
	"allocatable", "allocate", "allocated", "alog", "alog10", "amax0", "amax1",
	"amin0", "amin1", "amod", "and", "and.", "anint", "any", "asin", "asind",
	"assign", "assignment", "associated", "atan", "atan2", "atan2d", "atand",
	"automatic", "backspace", "bit_size", "bitest", "bjtest", "blank", "block",
	"blockdata", "blocksize", "btest", "buffer", "byte", "cabs", "call",
	"carg", "carriagecontrol", "case", "ccos", "cdabs", "cdcos", "cdexp",
	"cdlog", "cdsin", "cdsqrt", "ceiling", "cexp", "char", "character", "cloc",
	"clog", "close", "cmplx", "cncall", "common", "comp", "compl", "complex",
	"conjg", "contains", "continue", "cos", "cosd", "cosh", "cot", "cotan",
	"count", "cpu_time", "creator", "cshift", "csin", "csmg", "csqrt", "cvmgm",
	"cvmgn", "cvmgp", "cvmgt", "cvmgz", "cxabs", "cxcos", "cxexp", "cxlog",
	"cxsin", "cxsqrt", "cycle", "dabs", "dacos", "dacosd", "dasin", "dasind",
	"data", "datan", "datan2", "datan2d", "datand", "date", "date_and_time",
	"dble", "dcmplx", "dconjg", "dcos", "dcosd", "dcosh", "dcot", "dcotan",
	"ddim", "deallocate", "decode", "default", "define", "defined", "delete",
	"descr", "dexp", "dfloat", "dfloti", "dflotj", "digits", "dim", "dimag",
	"dimension", "dint", "direct", "disp", "dispose", "dll_export",
	"dll_import", "dlog", "dlog10", "dmax1", "dmin1", "dmod", "dnint", "do",
	"doall", "doparallel", "dot_product", "double complex", "double precision",
	"doublecomplex", "doubleprecision", "dprod", "dreal", "dshiftl", "dshiftr",
	"dsign", "dsin", "dsind", "dsinh", "dsqrt", "dtan", "dtand", "dtanh",
	"dtod", "dtoi", "else", "elseif", "elsewhere", "encode", "end", "end if",
	"endblockdata", "enddo", "endfile", "endfunction", "endif", "endinterface",
	"endmap", "endmodule", "endprogram", "endselect", "endstructure",
	"endsubroutine", "endunion", "endwhere", "endwhile", "entry", "eof",
	"eoshift", "epsilon", "eq.", "equivalence", "eqv", "eqv.", "err", "exist",
	"exit", "exp", "exponent", "external", "false.", "ferror", "file",
	"filetype", "float", "floati", "floatj", "floor", "fmt", "form", "format",
	"formatted", "fraction", "ftof", "ftoi", "ftrace", "function", "ge.",
	"global", "globaldefine", "go to", "goto", "gt.", "guard", "hfix", "huge",
	"i2abs", "i2dim", "i2max0", "i2min0", "i2mod", "i2nint", "i2sign", "iabs",
	"iachar", "iand", "ibchng", "ibclr", "ibits", "ibset", "ichar", "idim",
	"idint", "idnint", "ieor", "if", "ifix", "iiabs", "iiand", "iibclr",
	"iibits", "iibset", "iidim", "iidint", "iidnnt", "iieor", "iifix", "iint",
	"iior", "iishft", "iishftc", "iisign", "iixint", "iixnnt", "imag", "imax0",
	"imax1", "imin0", "imin1", "imod", "implicit", "implicit none",
	"implicitnone", "in", "include", "index", "inint", "inline", "inot",
	"inout", "inquire", "int", "int2", "int4", "integer", "intent",
	"interface", "intrinsic", "ior", "iostat", "isha", "ishc", "ishft",
	"ishftc", "ishl", "isign", "ixint", "ixnint", "izext", "izext2", "jdate",
	"jiabs", "jiand", "jibclr", "jibits", "jibset", "jidim", "jidint",
	"jidnnt", "jieor", "jifix", "jint", "jior", "jishft", "jishftc", "jisign",
	"jixint", "jixnnt", "jmax0", "jmax1", "jmin0", "jmin1", "jmod", "jnint",
	"jnot", "jzext", "jzext2", "jzext4", "keep", "kind", "lbound", "le.",
	"leadz", "len", "len_trim", "length", "lge", "lgt", "lint", "lle", "llt",
	"loc", "log", "log10", "logical", "long", "lshift", "lt.", "map", "mask",
	"matmul", "max", "max0", "max1", "maxcpus", "maxexponent", "maxloc",
	"maxrec", "maxval", "merge", "min", "min0", "min1", "minexponent",
	"minloc", "minval", "mod", "module", "modulo", "mvbits", "name", "named",
	"namelist", "ne.", "nearest", "neqv", "neqv.", "nextrec", "nint", "nml",
	"nospanblocks", "not", "not.", "nullify", "number", "numcpus", "only",
	"open", "opened", "operator", "option", "optional", "or", "or.",
	"organization", "out", "pack", "parallel", "parameter", "pascal", "pause",
	"permutation", "pointer", "popcnt", "poppar", "position", "precision",
	"present", "print", "private", "procedure", "product", "program", "public",
	"radix", "random_number", "random_seed", "ranf", "range", "ranget",
	"ranset", "read", "readonly", "real", "rec", "recl", "record",
	"recordsize", "recursive", "ref", "repeat", "reshape", "result", "return",
	"rewind", "rrspacing", "rshift", "save", "scale", "scan", "secnds",
	"select", "selectcase", "selected_int_kind", "selected_real_kind", "send",
	"sequence", "sequential", "set_exponent", "shape", "shared", "shift",
	"shiftl", "shiftr", "sign", "sin", "sind", "sinh", "size", "sizeof",
	"sngl", "spacing", "span", "spread", "sqrt", "stat", "static", "status",
	"stdcall", "stop", "structure", "subroutine", "sum", "system_clock", "tan",
	"tand", "tanh", "target", "taskcommon", "then", "tiny", "to", "transfer",
	"transparent", "transpose", "trim", "true.", "type", "ubound", "uint",
	"unformatted", "union", "unit", "unpack", "use", "val", "val1", "val2",
	"val4", "value", "verify", "virtual", "volatile", "wait", "where", "while",
	"word", "write", "xabs", "xacos", "xacosd", "xasin", "xasind", "xatan",
	"xatan2", "xatan2d", "xatand", "xcmplx", "xconjg", "xcos", "xcosd",
	"xcosh", "xdim", "xexp", "xfloat", "xfloti", "xflotj", "ximag", "xint",
	"xlog", "xlog10", "xmax1", "xmin1", "xmod", "xnint", "xor", "xor.",
	"xreal", "xsign", "xsin", "xsind", "xsinh", "xsqrt", "xtan", "xtand",
	"xtanh", "zabs", "zcos", "zexp", "zext", "zlog", "zsin", "zsqrt"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
FortranCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew FortranCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
FortranCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

FortranCompleter::FortranCompleter()
	:
	StringCompleter(kFortranLang, kKeywordCount, kKeywordList, JString::kIgnoreCase)
{
	UpdateWordList();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FortranCompleter::~FortranCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
FortranCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum();
}

/******************************************************************************
 MatchCase (virtual protected)

 ******************************************************************************/

void
FortranCompleter::MatchCase
	(
	const JString&	source,
	JString*		target
	)
	const
{
	target->ToLower();
	target->MatchCase(source, JCharacterRange(1, source.GetCharacterCount()));
}
