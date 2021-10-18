/******************************************************************************
 StylerStubs.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_StylerStubs
#define _H_StylerStubs

#include "StylerBase.h"

class BourneShellStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class CSharpStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class CShellStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class EiffelStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class INIStyler
{
public:

	static StylerBase*	Instance();
	static void				Shutdown();
};

class JavaScriptStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class MakeStyler
{
public:

    static StylerBase*  Instance();
    static void         Shutdown();
};

class PerlStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class PropertiesStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class PythonStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class RubyStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class SQLStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

class TCLStyler
{
public:

	static StylerBase*	Instance();
	static void			Shutdown();
};

#endif
