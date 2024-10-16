# Minimal makefile for mkdocs documentation
#

MKDOCSBUILD   ?= mkdocs
SOURCEDIR     = .

all: html

html:
	@$(MKDOCSBUILD) build

# Put it first so that "make" without argument is like "make help".
#help:
#	@$(MKDOCSBUILD) build --help

serve:
	@$(MKDOCSBUILD) serve


clean:
	@$(MKDOCSBUILD) build -c

