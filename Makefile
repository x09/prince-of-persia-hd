# Active project: native Linux DOS version. Apple II directories are archived.
.DEFAULT_GOAL := all
.PHONY: all check clean run
all check clean:
	$(MAKE) -C dos $@
run: all
	cd dos && ./prince

.PHONY: hd-preview hd-check hd-controls-check
hd-preview hd-check hd-controls-check:
	$(MAKE) -C dos $@

# Avoid simultaneous recursive builds of the same DOS object files.
.NOTPARALLEL:

.PHONY: run-hd hd-assets hd-game-check
run-hd hd-assets hd-game-check:
	$(MAKE) -C dos $@

.PHONY: hd-hero-check hd-hero-pack
hd-hero-check hd-hero-pack:
	$(MAKE) -C dos $@

.PHONY: hd-artifact-check hd-gate-scene-check
hd-artifact-check hd-gate-scene-check:
	$(MAKE) -C dos $@

.PHONY: campaign-check campaign-full-check
campaign-check campaign-full-check:
	$(MAKE) -C dos $@

.PHONY: install
install:
	$(MAKE) -C dos install $(if $(prefix),prefix=$(prefix)) $(if $(bindir),bindir=$(bindir)) $(if $(datadir),datadir=$(datadir)) $(if $(docdir),docdir=$(docdir)) DESTDIR=$(DESTDIR)

.PHONY: hd-dungeon-material
hd-dungeon-material:
	$(MAKE) -C dos hd-dungeon-material

.PHONY: persiality-check
persiality-check:
	$(MAKE) -C dos persiality-check
