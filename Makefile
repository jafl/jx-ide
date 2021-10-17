ifneq ($(shell ls ../JX/Makefile 2>/dev/null),)
  JX_ROOT := ../JX
else
  JX_ROOT := /usr/local
endif

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

APPS := code_crusader code_medic code_mill

.PHONY : all
all:
	@for d in ${APPS}; do \
         pushd $$d; ${MAKE}; popd; \
     done

.PHONY : Makefiles
Makefiles:
	@for d in ${APPS}; do \
         pushd $$d; makemake; make Makefiles; popd; \
     done

.PHONY : test
test:
	@cd code_crusader; ${MAKE} test

.PHONY : release
release:
	@for d in ${APPS}; do \
         pushd $$d; ${MAKE} release; popd; \
     done
