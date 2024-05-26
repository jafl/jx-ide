ifndef JX_ROOT
  ifneq ($(shell ls ../JX/Makefile 2>/dev/null),)
    JX_ROOT := ../JX
  else ifeq ($(shell uname -o),Darwin)
    JX_ROOT := $(shell brew --prefix)
  else
    JX_ROOT := /usr/local
  endif
endif

MAKE_INCLUDE := ${JX_ROOT}/include/jx-af/make
include ${MAKE_INCLUDE}/jx_config

APPS := code_crusader code_medic code_mill

.PHONY : all
all:
	@for d in ${APPS}; do \
         pushd $$d; if ! ${MAKE}; then e=1; fi; popd; \
     done; \
     if [[ $$e = 1 ]]; then false; fi

.PHONY : Makefiles
Makefiles:
	@for d in ${APPS}; do \
         pushd $$d; makemake; make Makefiles; popd; \
     done

.PHONY : test
test:
	@cd code_crusader; ${MAKE} test

.PHONY : install
install:
	@for d in ${APPS}; do \
         pushd $$d; ${MAKE} install; popd; \
     done

.PHONY : release
release:
	@for d in ${APPS}; do \
         pushd $$d; ${MAKE} release; popd; \
     done

.PHONY : push_version
push_version:
	@for d in ${APPS}; do \
         pushd $$d; ${MAKE} push_version; popd; \
     done

.PHONY : update_homebrew_formula
update_homebrew_formula:
	@for d in ${APPS}; do \
         pushd $$d; ${MAKE} update_homebrew_formula; popd; \
     done

.PHONY : tidy
tidy:
	@for d in ${APPS}; do \
         pushd $$d; ${MAKE} tidy; popd; \
     done

.PHONY : clean
clean:
	@for d in ${APPS}; do \
         pushd $$d; ${MAKE} clean; popd; \
     done

.PHONY : test_snap
test_snap:
	@chmod a+r snap/gui/*
	@snapcraft --debug --shell-after
