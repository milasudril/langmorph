DESTDIR?=
PREFIX?=/usr

.ONESHELL:
SHELL = /bin/bash

.PHONY: all
all:
	maike2

.PHONY:
clean:
	rm -rf __targets

.PHONY: install
install:
	./install.py '{
		"target_dir": "__targets",
		"install_to": "${PREFIX}${DESTDIR}",
		"targets":[
			"src/langmorph.app.maikerule",
			"src/langmorph.lib.maikerule"
		]
	}'
