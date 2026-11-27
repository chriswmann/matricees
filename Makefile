# -------------------------------------------------------------------------------------
#  Build: build all, run, hot (live reload using `entr`) and clean.
# -------------------------------------------------------------------------------------
CPPFLAGS ?=
CFLAGS ?= -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
LDFLAGS ?=
LDLIBS ?=

NAME = main
SRC = src
OBJ = obj
BIN = bin

EXE = $(BIN)/$(NAME)

BIN_SRCS = $(wildcard $(SRC)/*.c)
LIB_SRCS = $(shell find $(SRC) -mindepth 2 -name '*.c' 2>/dev/null)
ALL_SRCS = $(BIN_SRCS) $(LIB_SRCS)

LIB_OBJS = $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(LIB_SRCS))
BINS = $(patsubst $(SRC)/%.c,$(BIN)/%,$(BIN_SRCS))
DEPS = $(patsubst $(SRC)/%.c,$(OBJ)/%.d,$(ALL_SRCS))

all: $(BINS)

$(BIN)/%: $(OBJ)/%.o $(LIB_OBJS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

run: $(EXE)
	@$(EXE)

hot:
	@find $(SRC) -type f | entr -c $(MAKE) run

clean:
	@rm -rf $(BIN) $(OBJ)

-include $(DEPS)

.PHONY: all run hot clean

# -------------------------------------------------------------------------------------
#  Tests: Download Criterion to .deps/, to avoid needing it as a system dependency.
#  Currently Linux x86_64 only.
#  Note: CRITERION_VERSION and CRITERION_SHA256 need to be changed together.
# -------------------------------------------------------------------------------------

TEST_DIR = tests
DEPS_DIR = .deps

CRITERION_VERSION ?= 2.5.0
CRITERION_SHA256 ?= 02e9a2c6494949f33faa30637b5a0a87ced0787195daaf53fea99b3fdf447c35
CRITERION_ARCHIVE = criterion-$(CRITERION_VERSION)-linux-x86_64.tar.xz
CRITERION_URL = https://github.com/Snaipe/Criterion/releases/download/v$(CRITERION_VERSION)/$(CRITERION_ARCHIVE)
CRITERION_DIR = $(DEPS_DIR)/criterion-${CRITERION_VERSION}
CRITERION_STAMP = $(CRITERION_DIR)/.extracted

SAN_FLAGS ?= -fsanitize=address,undefined -fno-omit-frame-pointer -g
TEST_OBJ = $(OBJ)/test

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(BIN)/test/%,$(TEST_SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJ)/tests/%.o,$(TEST_SRCS))
TEST_LIB_OBJS = $(patsubst $(SRC)/%.c,$(TEST_OBJ)/%.o,$(LIB_SRCS))
TEST_DEPFILES = $(TEST_OBJS:.o=.d) $(TEST_LIB_OBJS:.o=.d)

$(DEPS_DIR)/$(CRITERION_ARCHIVE):
	@mkdir -p $(@D)
	curl -fsSL --retry 3 -o $@.part $(CRITERION_URL)
	echo "$(CRITERION_SHA256)  $@.part" | sha256sum -c --quiet - || { rm -f $@.part; exit 1; }
	mv $@.part $@

$(CRITERION_STAMP): $(DEPS_DIR)/$(CRITERION_ARCHIVE)
	rm -rf $(CRITERION_DIR)
	mkdir -p $(CRITERION_DIR)
	tar -xJf $< -C $(CRITERION_DIR) --strip-components=1
	touch $@

# Library sources rebuilt with sanitizers, kept apart from the normal objects.
$(TEST_LIB_OBJS): $(TEST_OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SAN_FLAGS) -MMD -MP -c $< -o $@

$(TEST_OBJS): $(TEST_OBJ)/tests/%.o: $(TEST_DIR)/%.c | $(CRITERION_STAMP)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -I$(SRC) -I$(TEST_DIR) -I$(CRITERION_DIR)/include $(CFLAGS) $(SAN_FLAGS) -MMD -MP -c $< -o $@

$(TEST_BINS): $(BIN)/test/%: $(TEST_OBJ)/tests/%.o $(TEST_LIB_OBJS)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(SAN_FLAGS) $^ -L$(CRITERION_DIR)/lib -lcriterion \
		-Wl,-rpath,$(abspath $(CRITERION_DIR))/lib $(LDLIBS) -o $@

test: $(TEST_BINS)
	@set -e; for t in $(TEST_BINS); do echo "== $$t"; $$t $(TEST_ARGS); done

distclean: clean
	@rm -rf $(DEPS_DIR)

-include $(DEPS) $(TEST_DEPFILES)

.PHONY: all run hot clean test distclean