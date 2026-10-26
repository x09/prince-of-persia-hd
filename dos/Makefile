CC = gcc
CPPFLAGS += -D_GNU_SOURCE=1 -Isrc $(shell pkg-config --cflags sdl2 SDL2_image)
CFLAGS ?= -std=c99 -O2 -Wall
LDLIBS += $(shell pkg-config --libs sdl2 SDL2_image) -lm
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
.PHONY: all clean check smoke assets-check
all: prince pophd
build:
	mkdir -p build
# Test executables live in build/; resolve their artwork from this source tree
# before running, rather than falling back to an installed package's data.
build/data: | build
	ln -sfn ../data $@
build/%.o: src/%.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@
prince: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)
build/check_assets: tests/check_assets.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
build/smoke_driver.so: tests/smoke_driver.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(shell pkg-config --libs sdl2) -ldl
smoke: all
	mkdir -p build/level-smoke
	@set -e; for n in $$(seq 0 14); do SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 20 ./prince megahit $$n --screenshot > build/level-smoke/level$$n.log 2>&1; done
assets-check: build/check_assets
	./build/check_assets
check: smoke build/check_assets
	./prince --version
	./build/check_assets
clean:
	rm -f build/*.o build/*.d build/check_assets build/smoke_driver.so prince pophd
-include $(OBJECTS:.o=.d)

# Experimental HD art preview, independent of DOS simulation.
build/pack_hd_run: tools/pack_hd_run.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
.PHONY: hd-preview hd-check hd-repack
hd-repack: build/pack_hd_run
	./build/pack_hd_run
hd-preview: all
	./prince --hd-preview
hd-check: all
	./prince --hd-check
	SDL_VIDEODRIVER=dummy ./prince --hd-preview --hd-smoke --hd-capture build/hd-preview.png

# Compile the original entry under its own name; hd_entry.c dispatches modes.
build/main.o: src/main.c Makefile | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -Dmain=pop_dos_main -MMD -MP -c $< -o $@

build/hd_controls_driver.so: tests/hd_controls_driver.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(shell pkg-config --libs sdl2) -ldl
.PHONY: hd-controls-check
hd-controls-check: all build/hd_controls_driver.so
	SDL_VIDEODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/hd_controls_driver.so ./prince --hd-preview --hd-smoke

# Live HD renderer and reproducible asset packing.
build/build_hd_assets: tools/build_hd_assets_isotropic.c tools/hd_fit.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
build/test_hd_game: tests/test_hd_game.c src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_game.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-assets run-hd hd-game-check
hd-assets: build/build_hd_assets build/pack_hd_hero build/pack_hd_bones
	mkdir -p data/hd/KID data/hd/VDUNGEON data/hd/VPALACE data/hd/PRINCE
	./build/build_hd_assets
	./build/pack_hd_hero
	./build/pack_hd_bones
	$(MAKE) hd-architecture-pack
	$(MAKE) hd-chomper-pack
run-hd: all
	./prince --hd
hd-game-check: all hd-artifact-check build/test_hd_game build/smoke_driver.so
	./build/test_hd_game
	mkdir -p build/hd-level-smoke
	@set -e; for n in $$(seq 0 14); do SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 20 ./prince --hd megahit $$n seed=1 --screenshot > build/hd-level-smoke/level$$n.log 2>&1; done
	SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_SMOKE_MS=10000 POP_SMOKE_INPUT=1 LD_PRELOAD=$(CURDIR)/build/smoke_driver.so timeout 20 ./prince --hd megahit 1 seed=1 > build/hd-play-smoke.log 2>&1
	SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_SMOKE_MS=10000 POP_SMOKE_INPUT=1 LD_PRELOAD=$(CURDIR)/build/smoke_driver.so timeout 20 ./prince --hd megahit 2 seed=1 > build/hd-play-level2.log 2>&1
	@test $$(grep -E -l 'presents=1 failures=0' build/hd-level-smoke/level*.log | wc -l) -eq 15
	@test $$(grep -E -c '^SMOKE input=' build/hd-play-smoke.log) -eq 12
	@grep -E 'HD: art .* failures=0' build/hd-play-smoke.log
	@test $$(grep -E -c '^SMOKE input=' build/hd-play-level2.log) -eq 12
	@grep -E 'HD: art .* failures=0' build/hd-play-level2.log
	@! grep -Er 'HD rejected|HD .* error:|failures=[1-9]|fallback=[1-9]' build/hd-level-smoke/ build/hd-play-smoke.log build/hd-play-level2.log

build/pack_hd_hero: tools/pack_hd_hero.c tools/hd_fit.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
build/check_hd_hero: tests/check_hd_hero.c src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/check_hd_hero.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-hero-check hd-hero-pack
hd-hero-pack: build/pack_hd_hero
	./build/pack_hd_hero
hd-hero-check: build/check_hd_hero
	ln -sfn ../data build/data
	./build/check_hd_hero

build/test_hd_torch: tests/test_hd_torch.c src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_torch.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
build/test_hd_gate: tests/test_hd_gate.c src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_gate.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
build/test_hd_overlap: tests/test_hd_overlap.c tools/hd_fit.h src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_overlap.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-artifact-check
hd-artifact-check: hd-hero-check build/test_hd_torch build/test_hd_gate build/test_hd_overlap
	./build/test_hd_torch
	./build/test_hd_gate
	./build/test_hd_overlap

build/hd_scene_driver.so: tests/hd_scene_driver.c src/common.h src/types.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(LDLIBS) -ldl
build/prince-render-test: $(OBJECTS)
	$(CC) -rdynamic $(OBJECTS) -o $@ $(LDLIBS)
.PHONY: hd-gate-scene-check
hd-gate-scene-check: build/prince-render-test build/hd_scene_driver.so
	ln -sfn ../data build/data
	timeout 15 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/hd_scene_driver.so ./build/prince-render-test --hd megahit 1 seed=1 >build/gate-fixture.log 2>&1
	@grep -E 'frame=56 modifier=' build/gate-fixture.log
	@! grep -E 'error:|failures=[1-9]|fallback=[1-9]' build/gate-fixture.log

# Ordinary guard: generated poses packed without changing native assets.
build/pack_hd_guard: tools/pack_hd_guard.c tools/hd_fit.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
build/check_hd_guard: tests/check_hd_guard.c src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/check_hd_guard.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-guard-pack hd-guard-check
hd-guard-pack: build/pack_hd_guard
	mkdir -p data/hd/GUARD
	./build/pack_hd_guard
hd-guard-check: build/check_hd_guard
	ln -sfn ../data build/data
	./build/check_hd_guard
	./build/check_hd_swords
hd-assets: hd-guard-pack
hd-game-check: hd-guard-check

build/hd_guard_scene_driver.so: tests/hd_guard_scene_driver.c src/common.h src/types.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(LDLIBS) -ldl
.PHONY: hd-guard-scene-check
hd-guard-scene-check: build/prince-render-test build/hd_guard_scene_driver.so
	ln -sfn ../data build/data
	timeout 40 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/hd_guard_scene_driver.so ./build/prince-render-test --hd megahit 2 seed=1 >build/guard-scene.log 2>&1
	@grep -E 'GUARD encounter observed=[1-9]' build/guard-scene.log
	@! grep -E 'HD rejected|error:|failures=[1-9]|fallback=[1-9]' build/guard-scene.log

build/build_hd_swords: tools/build_hd_swords.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
.PHONY: hd-swords-pack
hd-swords-pack: build/build_hd_swords
	mkdir -p data/hd/PRINCE
	./build/build_hd_swords
hd-assets: hd-swords-pack

build/check_hd_swords: tests/check_hd_swords.c src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/check_hd_swords.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
hd-guard-check: build/check_hd_swords

build/pack_hd_characters: tools/pack_hd_characters.c tools/hd_characters.h tools/hd_fit.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
build/check_hd_characters: tests/check_hd_characters.c tools/hd_characters.h src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/check_hd_characters.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-characters-pack hd-characters-check
hd-characters-pack: build/pack_hd_characters
	mkdir -p data/hd/SKEL data/hd/FAT data/hd/VIZIER data/hd/SHADOW data/hd/PV
	./build/pack_hd_characters
hd-characters-check: build/check_hd_characters
	ln -sfn ../data build/data
	./build/check_hd_characters
hd-assets: hd-characters-pack
hd-game-check: hd-characters-check

[executed on device: lenovo (6f28d5e1-f939-44f4-a819-b764e172eeb2)]
build/test_hd_font: tests/test_hd_font.c src/hd_game.c src/hd_font.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_font.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-font-check
hd-font-check: build/test_hd_font
	ln -sfn ../data build/data
	./build/test_hd_font
hd-game-check: hd-font-check

build/finish_hd_assets: tools/finish_hd_assets.c tools/hd_remaining.tsv src/hd_font.c tools/vendor/stb_truetype.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tools/finish_hd_assets.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-finish-pack
hd-finish-pack: build/finish_hd_assets
	mkdir -p data/hd/TITLE
	./build/finish_hd_assets
hd-assets: hd-finish-pack

build/check_hd_coverage: tests/check_hd_coverage.c src/hd_game.c src/hd_font.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/check_hd_coverage.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-coverage-check
hd-coverage-check: build/check_hd_coverage
	ln -sfn ../data build/data
	./build/check_hd_coverage
hd-game-check: hd-coverage-check

HD_RENDER_TESTS := build/test_hd_game build/test_hd_torch build/test_hd_gate build/test_hd_overlap build/check_hd_hero build/check_hd_guard build/check_hd_swords build/check_hd_characters build/check_hd_coverage build/test_hd_font
$(HD_RENDER_TESTS) build/test_hd_chomper build/test_hd_floor_overlay build/prince-render-test: | build/data
$(HD_RENDER_TESTS): src/hd_font.c tools/hd_characters.h tools/vendor/stb_truetype.h

build/test_hd_layers: tests/test_hd_layers.c src/hd_game.c src/hd_font.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_layers.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-layers-check
hd-layers-check: build/test_hd_layers
	ln -sfn ../data build/data
	./build/test_hd_layers
hd-game-check: hd-layers-check

data/hd/fonts/DejaVuSerif.ttf: data/hd-source/DejaVuSerif.ttf data/hd-source/DejaVu-LICENSE.txt
	mkdir -p data/hd/fonts
	cp data/hd-source/DejaVuSerif.ttf $@
	cp data/hd-source/DejaVu-LICENSE.txt data/hd/fonts/LICENSE.txt
.PHONY: hd-font-assets
hd-font-assets: data/hd/fonts/DejaVuSerif.ttf
all hd-font-check hd-assets: hd-font-assets

.PHONY: hd-enemies-check
hd-enemies-check: build/prince-render-test build/hd_guard_scene_driver.so
	ln -sfn ../data build/data
	@set -e; for n in 3 6 12 13; do timeout 50 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_GUARD_LEVEL=$$n LD_PRELOAD=$(CURDIR)/build/hd_guard_scene_driver.so ./build/prince-render-test --hd megahit $$n seed=1 >build/hd-enemy-$$n.log 2>&1; grep -E 'GUARD encounter observed=[1-9]' build/hd-enemy-$$n.log; ! grep -E 'HD rejected|error:|failures=[1-9]|fallback=[1-9]' build/hd-enemy-$$n.log; done

build/test_hd_ui: tests/test_hd_ui.c src/hd_game.c src/hd_font.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_ui.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-ui-check
hd-ui-check: build/test_hd_ui
	ln -sfn ../data build/data
	./build/test_hd_ui
hd-game-check: hd-ui-check

# HD grip attachment and exhaustive enemy rendering fixtures.
$(HD_RENDER_TESTS) build/test_hd_layers build/test_hd_ui: src/hd_weapon.h
build/test_hd_grips: tests/test_hd_grips.c $(filter-out build/main.o build/hd_entry.o,$(OBJECTS))
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(filter-out build/main.o build/hd_entry.o,$(OBJECTS)) -o $@ $(LDLIBS)
build/audit_guard_poses: tools/audit_guard_poses.c src/hd_weapon.h $(filter-out build/main.o build/hd_entry.o,$(OBJECTS))
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(filter-out build/main.o build/hd_entry.o,$(OBJECTS)) -o $@ $(LDLIBS)
build/hd_guard_pose_driver.so: tests/hd_guard_pose_driver.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(LDLIBS) -ldl
.PHONY: hd-grips-check hd-guard-poses-check
hd-grips-check: build/test_hd_grips
	ln -sfn ../data build/data
	./build/test_hd_grips
hd-guard-poses-check: build/prince-render-test build/hd_guard_pose_driver.so build/audit_guard_poses
	./build/audit_guard_poses
	rm -f build/guard-engine-poses.tsv
	@set -e; for n in 2 3 6 12 13; do timeout 45 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_GUARD_LEVEL=$$n LD_PRELOAD=$(CURDIR)/build/hd_guard_pose_driver.so ./build/prince-render-test --hd megahit $$n seed=1 >build/guard-engine-level$$n.log 2>&1; grep -E 'GUARD exhaustive.*poses/directions=[6-9][0-9]' build/guard-engine-level$$n.log; ! grep -E 'HD rejected|error:|failures=[1-9]|fallback=[1-9]' build/guard-engine-level$$n.log; done
hd-game-check: hd-grips-check

build/pack_hd_bones: tools/pack_hd_bones.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
.PHONY: hd-bones-pack
hd-bones-pack: build/pack_hd_bones
	./build/pack_hd_bones
hd-assets: build/pack_hd_bones

build/hd_bones_scene_driver.so: tests/hd_bones_scene_driver.c src/common.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(LDLIBS) -ldl
.PHONY: hd-bones-check
hd-bones-check: all build/prince-render-test build/hd_bones_scene_driver.so
	timeout 45 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/hd_bones_scene_driver.so ./build/prince-render-test --hd megahit 3 seed=1 > build/hd-bones-scene.log 2>&1
	@grep -E 'BONES wake before=21 after=1 guard=4 verified=1' build/hd-bones-scene.log
	@! grep -E 'HD rejected|Tried to use|error:|failures=[1-9]|fallback=[1-9]' build/hd-bones-scene.log

# Replay-based campaign regression: reaching a new level and staying alive are required.
CAMPAIGN_HOOKS := seg000 seg001 seg006 seg009 replay
CAMPAIGN_OBJECTS := $(addprefix build/campaign-observer/,$(addsuffix .o,$(CAMPAIGN_HOOKS)))
build/campaign-observer:
	mkdir -p $@
	ln -s ../../data $@/data
build/campaign-observer/%.o: src/%.c | build/campaign-observer
	$(CC) $(CPPFLAGS) $(CFLAGS) -DPOP_CAMPAIGN_TEST -MMD -MP -c $< -o $@
build/campaign-observer/driver.o: tests/campaign_replay_driver.c src/common.h | build/campaign-observer
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
build/campaign-observer/prince: $(OBJECTS) $(CAMPAIGN_OBJECTS) build/campaign-observer/driver.o
	$(CC) $(LDFLAGS) $(filter-out $(addprefix build/,$(addsuffix .o,$(CAMPAIGN_HOOKS))),$(OBJECTS)) $(CAMPAIGN_OBJECTS) build/campaign-observer/driver.o -o $@ $(LDLIBS)
-include $(CAMPAIGN_OBJECTS:.o=.d)

.PHONY: campaign-check
campaign-check: all
	mkdir -p build/campaign-validation
	@set -eu; for n in $$(seq 1 14); do \
	 f=$$(printf 'tests/campaign/level%02d.p1r' $$n); \
	 test -s "$$f" || { echo "Missing campaign replay: $$f"; exit 1; }; \
	 log=$$(printf 'build/campaign-validation/level%02d.log' $$n); \
	 SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 30 ./prince validate "$$f" > "$$log" 2>&1; \
	 expected=$$((n+1)); if [ $$n -eq 14 ]; then expected=14; fi; \
	 grep -E -q '^Kid is alive\.' "$$log"; \
	 grep -E -q '^Play duration matches replay length\.' "$$log"; \
	 grep -E -q "^Replay state: next_level=$$expected " "$$log"; \
	 if [ $$n -eq 14 ]; then grep -E -q '^Replay ended in level 14, room 5\.' "$$log"; fi; \
	 echo "Campaign level $$n -> $$expected PASS"; \
	 done

.PHONY: campaign-full-check
campaign-full-check: all build/campaign-observer/prince
	mkdir -p build/campaign-validation/screens
	timeout 60 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy ./prince validate tests/campaign/full-campaign.p1r > build/campaign-validation/full-production.log 2>&1
	@grep -E -q '^Replay ended in level 14, room 5\.' build/campaign-validation/full-production.log
	@grep -E -q '^Kid is alive\.' build/campaign-validation/full-production.log
	@grep -E -q '^Play duration matches replay length\.' build/campaign-validation/full-production.log
	timeout 60 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_FULL_REQUIRE_ALL=1 POP_FULL_AUDIT=$(CURDIR)/build/campaign-validation/full-native.csv ./build/campaign-observer/prince validate tests/campaign/full-campaign.p1r > build/campaign-validation/full-native.log 2>&1
	@grep -E '^FULL WIN|^FULL EVENTS' build/campaign-validation/full-native.log
	timeout 180 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_FULL_REQUIRE_ALL=1 POP_FULL_RENDER=1 POP_FULL_ENDING=1 POP_FULL_CAPTURES=$(CURDIR)/build/campaign-validation/screens POP_FULL_AUDIT=$(CURDIR)/build/campaign-validation/full-hd.csv ./build/campaign-observer/prince --hd validate tests/campaign/full-campaign.p1r > build/campaign-validation/full-hd.log 2>&1
	@grep -E '^FULL WIN|^FULL EVENTS|^FULL ENDING|^HD: art' build/campaign-validation/full-hd.log
	@grep -E -q '^FULL ENDING animation and victory card PASS' build/campaign-validation/full-hd.log
	@! grep -E 'FULL INVALID|FULL DEAD|FULL CAPTURE FAILED|HD rejected|failures=[1-9]|fallback=[1-9]' build/campaign-validation/full-hd.log
	cmp build/campaign-validation/full-native.csv build/campaign-validation/full-hd.csv
	@echo 'Full campaign: production victory, all story events, identical DOS/HD simulation, ending PASS'

# User-facing HD executable; prince remains available for native regression tests.
pophd: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

prefix ?= /usr
bindir ?= $(prefix)/bin
datadir ?= $(prefix)/share
docdir ?= $(datadir)/doc/prince-of-persia-hd-1.0
.PHONY: install
install: all
	install -Dm755 pophd $(DESTDIR)$(bindir)/pophd
	install -d $(DESTDIR)$(datadir)/prince-of-persia-hd/data
	cp -R data/. $(DESTDIR)$(datadir)/prince-of-persia-hd/data/
	rm -rf $(DESTDIR)$(datadir)/prince-of-persia-hd/data/hd-source $(DESTDIR)$(datadir)/prince-of-persia-hd/data/hd-preview
	install -m644 PoPHD.ini $(DESTDIR)$(datadir)/prince-of-persia-hd/
	install -Dm644 ../packaging/pophd.desktop $(DESTDIR)$(datadir)/applications/pophd.desktop
	install -Dm644 ../packaging/pophd.png $(DESTDIR)$(datadir)/icons/hicolor/256x256/apps/pophd.png
	install -d $(DESTDIR)$(docdir)
	install -m644 COPYING README.SDLPoP.md ../README.md ../README-ru.md ../packaging/ASSET_NOTICES.md $(DESTDIR)$(docdir)/

.PHONY: hd-dungeon-material
hd-dungeon-material: build/build_hd_assets
	./build/build_hd_assets --dungeon-only

build/persiality_scene_driver.so: tests/persiality_scene_driver.c src/hd_persiality.h src/common.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(LDLIBS) -ldl
.PHONY: persiality-check
persiality-check: all build/prince-render-test build/persiality_scene_driver.so build/pack_persiality
	./build/pack_persiality --check
	mkdir -p build/persiality/home
	timeout 30 env HOME=$(CURDIR)/build/persiality/home SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/persiality_scene_driver.so ./build/prince-render-test --hd megahit 2 seed=1 mute >build/persiality/right.log 2>&1
	grep -q 'PERSIALITY TEST PASS' build/persiality/right.log
	timeout 30 env HOME=$(CURDIR)/build/persiality/home SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_PERSIALITY_LEFT=1 POP_PERSIALITY_UPPER=1 LD_PRELOAD=$(CURDIR)/build/persiality_scene_driver.so ./build/prince-render-test --hd megahit 2 seed=1 mute >build/persiality/left.log 2>&1
	grep -q 'PERSIALITY TEST PASS' build/persiality/left.log

.PHONY: persiality-special-check
persiality-special-check: all build/prince-render-test build/persiality_scene_driver.so build/pack_persiality
	mkdir -p build/persiality/home
	@set -e; for level in 3 6 12 13; do \
	 timeout 35 env HOME=$(CURDIR)/build/persiality/home SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_PERSIALITY_LEVEL=$$level LD_PRELOAD=$(CURDIR)/build/persiality_scene_driver.so ./build/prince-render-test --hd megahit $$level seed=1 mute >build/persiality/level$$level.log 2>&1; \
	 grep 'PERSIALITY TEST PASS' build/persiality/level$$level.log; \
	done

# Independent frames prevent texture sampling from neighboring poses.
build/pack_persiality: tools/pack_persiality.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
.PHONY: hd-persiality-pack
hd-persiality-pack: build/pack_persiality
	./build/pack_persiality
hd-assets: hd-persiality-pack

# Painted modular columns and exit frame; run after the general material packer.
build/pack_hd_architecture: tools/pack_hd_architecture.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
.PHONY: hd-architecture-pack
hd-architecture-pack: build/pack_hd_architecture
	./build/pack_hd_architecture

build/pack_hd_chomper: tools/pack_hd_chomper.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
.PHONY: hd-chomper-pack
hd-chomper-pack: build/pack_hd_chomper
	./build/pack_hd_chomper

build/test_hd_chomper: tests/test_hd_chomper.c src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_chomper.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-chomper-check
hd-chomper-check: build/test_hd_chomper
	./build/test_hd_chomper

build/hd_chomper_scene_driver.so: tests/hd_chomper_scene_driver.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(shell pkg-config --libs sdl2) -ldl
.PHONY: hd-chomper-scene-check
hd-chomper-scene-check: all build/prince-render-test build/hd_chomper_scene_driver.so hd-chomper-check
	timeout 35 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/hd_chomper_scene_driver.so ./build/prince-render-test --hd megahit 2 > build/chomper-scene-2.log 2>&1
	timeout 35 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/hd_chomper_scene_driver.so ./build/prince-render-test --hd megahit 4 > build/chomper-scene-4.log 2>&1
	@! grep -E 'HD rejected|HD .* error:|failures=[1-9]' build/chomper-scene-2.log build/chomper-scene-4.log

build/hd_architecture_scene_driver.so: tests/hd_architecture_scene_driver.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(shell pkg-config --libs sdl2) -ldl
.PHONY: hd-architecture-check
hd-architecture-check: all build/prince-render-test build/hd_architecture_scene_driver.so
	timeout 30 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/hd_architecture_scene_driver.so ./build/prince-render-test --hd megahit 2 > build/architecture-check.log 2>&1
	@! grep -E 'HD rejected|HD .* error:|failures=[1-9]' build/architecture-check.log
	./build/check_hd_columns

build/check_hd_columns: tests/check_hd_columns.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)
hd-architecture-check: build/check_hd_columns

build/hd_loose_scene_driver.so: tests/hd_loose_scene_driver.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -shared -fPIC $< -o $@ $(LDLIBS) -ldl
.PHONY: hd-loose-check
hd-loose-check: all build/prince-render-test build/hd_loose_scene_driver.so
	timeout 40 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy LD_PRELOAD=$(CURDIR)/build/hd_loose_scene_driver.so ./build/prince-render-test --hd megahit 2 > build/loose-scene.log 2>&1
	timeout 40 env SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy POP_LOOSE_STAGGER=1 LD_PRELOAD=$(CURDIR)/build/hd_loose_scene_driver.so ./build/prince-render-test --hd megahit 2 > build/loose-stagger.log 2>&1
	@grep -E 'CLIMB floor occlusion PASS|LOOSE adjacent wall PASS' build/loose-scene.log build/loose-stagger.log

build/test_hd_floor_overlay: tests/test_hd_floor_overlay.c src/hd_game.c src/hd_game.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hd_floor_overlay.c src/hd_game.c src/hd_font.c -o $@ $(LDLIBS)
.PHONY: hd-floor-overlay-check
hd-floor-overlay-check: build/test_hd_floor_overlay
	./build/test_hd_floor_overlay
hd-artifact-check: hd-floor-overlay-check
