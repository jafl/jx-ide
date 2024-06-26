/******************************************************************************
 SQLCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2002 by John Lindal.

 ******************************************************************************/

#include "SQLCompleter.h"
#include <jx-af/jcore/jAssert.h>

SQLCompleter* SQLCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// keywords

	"add", "agent", "aggregate", "attribute", "authid", "avg",
	"bfile_base", "binary", "blob_base", "block", "body", "both", "bound",
	"bulk", "byte", "call", "calling", "cascade", "char", "char_base",
	"character", "charsetform", "charsetid", "charset", "clob_base",
	"close", "collect", "comment", "commit", "committed", "compiled",
	"constant", "constructor", "context", "convert", "count", "cursor",
	"customdatum", "dangling", "data", "date", "date_base", "day",
	"define", "deterministic", "double", "duration", "element", "elsif",
	"empty", "escape", "except", "exceptions", "execute", "exit",
	"external", "final", "fixed", "float", "forall", "force", "function",
	"general", "hash", "heap", "hidden", "hour", "immediate", "including",
	"indicator", "indices", "infinite", "inner", "instantiable", "int",
	"integer", "interface", "interval", "invalidate", "isolation", "java",
	"join", "key", "language", "large", "leading", "left", "length", "level",
	"library", "like", "limit", "limited", "local", "long", "loop", "map",
	"max", "maxlen", "member", "merge", "min", "minute", "mod", "modify",
	"month", "multiset", "name", "nan", "national", "native", "nchar",
	"new", "nocopy", "number_base", "numeric", "object", "only", "opaque",
	"open", "operator", "others", "out", "outer", "overriding", "package",
	"parallel_enable", "parameter", "parameters", "partition", "pascal",
	"pipe", "pipelined", "pragma", "precision", "primary", "private", "raise",
	"range", "raw", "read", "real", "record", "ref", "references", "rem",
	"remainder", "rename", "result", "return", "returning", "reverse",
	"right", "rollback", "row", "sample", "save", "savepoint", "sb1",
	"sb2", "sb4", "second", "segment", "self", "separate", "sequence",
	"serializable", "set", "short", "size_t", "smallint", "some", "sparse",
	"sqlcode", "sqldata", "sqlname", "sqlstate", "standard", "static",
	"stddev", "stored", "string", "struct", "style", "submultiset",
	"subpartition", "substitutable", "subtype", "sum", "synonym", "tdo",
	"the", "time", "timestamp", "timezone_abbr", "timezone_hour",
	"timezone_minute", "timezone_region", "trailing", "transac",
	"transactional", "trusted", "type", "ub1", "ub2", "ub4", "under",
	"unsigned", "untrusted", "using", "valist", "value", "variable",
	"variance", "varray", "varying", "void", "while", "work", "wrapped",
	"write", "year", "zone",

	// reserved words

	"all", "alter", "and", "any", "array", "arrow", "as", "asc", "at",
	"begin", "between", "by", "case", "check", "cluster", "clusters",
	"colauth", "columns", "compress", "connect", "crash", "create",
	"current", "decimal", "declare", "default", "delete", "desc",
	"distinct", "drop", "else", "end", "exception", "exclusive", "exists",
	"fetch", "form", "for", "from", "goto", "grant", "group", "having",
	"identified", "if", "in", "index", "indexes", "insert", "intersect",
	"into", "is", "like", "lock", "minus", "mode", "nocompress", "not",
	"nowait", "null", "of", "on", "option", "or", "order", "overlaps",
	"prior", "procedure", "public", "range", "record", "resource",
	"revoke", "select", "share", "size", "sql", "start", "subtype",
	"tabauth", "table", "then", "to", "type", "union", "unique", "update",
	"use", "values", "view", "views", "when", "where", "with",

	// built-in functions

	"current_date", "current_time", "current_timestamp", "current_user",
	"user", "session_user", "system_user",

	"case", "cast",

	"abs", "bit_length", "ceil", "ceiling", "char_length", "exp",
	"extract", "floor", "ln", "octet_length", "position", "power", "sqrt",
	"width_bucket",

	"minute_second", "hour_minute", "day_hour", "year_month",
	"hour_second", "day_minute", "day_second",

	"lower", "overlay", "substring", "translate", "trim", "upper",

	// MySQL

	"acos", "adddate", "addtime", "aes_decrypt", "aes_encrypt",
	"any_value", "ascii", "asin", "asymmetric_decrypt",
	"asymmetric_derive", "asymmetric_encrypt", "asymmetric_sign",
	"asymmetric_verify", "atan", "atan2", "auto_increment", "benchmark",
	"bigint", "bin", "bit", "bit_and", "bit_count", "bit_or", "bit_xor",
	"character_length", "coalesce", "coercibility", "collation", "concat",
	"concat_ws", "connection_id", "conv", "convert_tz", "cos", "cot",
	"crc32", "create_asymmetric_priv_key", "create_asymmetric_pub_key",
	"create_dh_parameters", "create_digest", "curdate", "curtime",
	"date_add", "date_format", "date_sub", "datediff", "datetime",
	"dayname", "dayofmonth", "dayofweek", "dayofyear", "decode", "degrees",
	"div", "elt", "encode", "engine", "enum", "export_set", "extractvalue", "field",
	"find_in_set", "flush", "format", "found_rows", "from_base64", "from_days",
	"from_unixtime", "geometrycollection", "get_format", "get_lock",
	"greatest", "group_concat", "gtid_subset", "gtid_subtract", "hex",
	"ifnull", "inet_aton", "inet_ntoa", "inet6_aton", "inet6_ntoa",
	"instr", "is_free_lock", "is_ipv4", "is_ipv4_compat", "is_ipv4_mapped",
	"is_ivp6", "is_used_lock", "isnull", "json", "json_append",
	"json_array", "json_array_append", "json_array_insert",
	"json_contains", "json_contains_path", "json_depth", "json_extract",
	"json_insert", "json_keys", "json_length", "json_merge", "json_object",
	"json_quote", "json_remove", "json_replace", "json_search", "json_set",
	"json_type", "json_unquote", "json_valid", "last_day",
	"last_insert_id", "lcase", "least", "linestring", "load_file",
	"localtime", "localtimestamp", "locate", "log", "log10", "log2",
	"lpad", "ltrim", "make_set", "makedate", "maketime", "master_pos_wait",
	"match", "mbrcontains", "mbrcoveredby", "mbrcovers", "mbrdisjoint",
	"mbrequals", "mbrintersects", "mbroverlaps", "mbrtouches", "mbrwithin",
	"md5", "mediumint", "microsecond", "mid", "monthname",
	"multilinestring", "multipoint", "multipolygon", "name_const", "now",
	"nullif", "oct", "old_password", "ord", "period_add", "period_diff",
	"pi", "point", "polygon", "pow", "privileges", "quarter", "quote", "radians", "rand",
	"random_bytes", "regexp", "release_all_locks", "release_lock",
	"repeat", "replace", "reverse", "rlike", "round", "row_count", "rpad",
	"rtrim", "schema", "sec_to_time", "sha1", "sha", "sha2", "sign", "sin",
	"sleep", "soundex", "sounds", "space", "std", "stddev_pop",
	"stddev_samp", "str_to_date", "strcmp", "subdate", "substr",
	"substring", "substring_index", "subtime", "sysdate", "tan",
	"time_format", "time_to_sec", "timediff", "timestampadd",
	"timestampdiff", "tinyint", "to_base64", "to_days", "to_seconds",
	"truncate", "ucase", "uncompress", "uncompressed_length", "unhex",
	"unix_timestamp", "updatexml", "utc_date", "utc_time", "utc_timestamp",
	"uuid", "uuid_short", "validate_password_strength", "var_pop",
	"var_samp", "varbinary", "varchar", "version",
	"wait_for_executed_gtid_set", "wait_until_sql_thread_after_gtids",
	"week", "weekday", "weekofyear", "weight_string", "xor", "yearweek",

	// Postgresql

	"analyse", "analyze", "do", "freeze", "ilike", "notnull",
	"offset", "verbose",

	// Oracle

	"access", "audit", "bfile", "binary_double", "binary_float", "blob",
	"clob", "file", "increment", "initial", "like2", "like4", "likec",
	"maxextents", "mlslabel", "nclob", "nested_table_id", "noaudit",
	"number", "nvarchar2", "ocicoll", "ocidatetime", "ocidate",
	"ociduration", "ociinterval", "ociloblocator", "ocinumber", "ociraw",
	"ocirefcursor", "ociref", "ocirowid", "ocistring", "ocitype",
	"offline", "online", "oracle", "oradata", "organization", "orlany",
	"orlvary", "pctfree", "rowid","rownum", "rows", "session",
	"successful", "trigger", "uid", "uritype", "validate", "varchar2",
	"whenever", "xmltype"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
SQLCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew SQLCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
SQLCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

SQLCompleter::SQLCompleter()
	:
	StringCompleter(kSQLLang, kKeywordCount, kKeywordList, JString::kIgnoreCase)
{
	UpdateWordList();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SQLCompleter::~SQLCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
SQLCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || c == '$';
}

/******************************************************************************
 MatchCase (virtual protected)

 ******************************************************************************/

void
SQLCompleter::MatchCase
	(
	const JString&	source,
	JString*		target
	)
	const
{
	target->ToLower();
	target->MatchCase(source, JCharacterRange(1, source.GetCharacterCount()));
}
