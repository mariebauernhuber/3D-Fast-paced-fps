# ANSI Color codes for pretty output
COM_STRING  := "Compiling"
OK_STRING   := "[OK]"
ERROR_STRING:= "[ERROR]"
WARN_STRING := "[WARNING]"
COM_COLOR := \033[1;34m # Bright blue
OBJ_COLOR := \033[1;36m # Bright cyan
OK_COLOR := \033[1;92m # Bright green (OK)
ERROR_COLOR := \033[1;91m # Bright red (ERROR)
WARN_COLOR := \033[1;93m # Bright yellow (WARNING)
NO_COLOR := \033[0m # Reset / no color
CC := g++

# Use pkg-config to get exact Nix store paths for the LSP
SDL_CFLAGS := $(shell pkg-config --cflags sdl3)
SDL_LIBS   := $(shell pkg-config --libs sdl3)

ARGS =

IMGUI_DIR := imgui
IMGUI_BACKENDS := $(IMGUI_DIR)/backends

CPPFLAGS := -Wall -Wextra -g -Iinclude $(SDL_CFLAGS)
CPPFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_BACKENDS)
LDFLAGS  := $(SDL_LIBS) -lm -lGL -lGLEW -lglfw -lassimp

SRC_DIR   := src
TEST_SRC_DIR := tests
BUILD_DIR := build

# Your app sources
SRCS      := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS := $(wildcard $(TEST_SRC_DIR)/*.cpp)

# ImGui sources (explicit list to avoid unwanted files)
IMGUI_SRCS := $(IMGUI_DIR)/imgui.cpp \
              $(IMGUI_DIR)/imgui_demo.cpp \
              $(IMGUI_DIR)/imgui_draw.cpp \
              $(IMGUI_DIR)/imgui_widgets.cpp \
              $(IMGUI_DIR)/imgui_tables.cpp \
              $(IMGUI_BACKENDS)/imgui_impl_sdl3.cpp \
              $(IMGUI_BACKENDS)/imgui_impl_opengl3.cpp \
              $(IMGUI_BACKENDS)/imgui_impl_sdlrenderer3.cpp

OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TEST_OBJS := $(patsubst $(TEST_SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SRCS))
IMGUI_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(IMGUI_SRCS)))

ALL_OBJS := $(OBJS) $(IMGUI_OBJS)
TARGET := $(BUILD_DIR)/app
TEST_TARGET := $(BUILD_DIR)/test

all: $(TARGET)

$(TARGET): $(ALL_OBJS) | $(BUILD_DIR)
	@printf "%bCompiling final executable...%b\n" "$(COM_COLOR)" "$(NO_COLOR)"
	@$(CC) $(ALL_OBJS) -o $@ $(LDFLAGS); \
	RESULT=$$?; \
	if [ $$RESULT -eq 0 ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)Linking $(OBJ_COLOR)$(TARGET)$(NO_COLOR)" "$(OK_COLOR)$(OK_STRING)$(NO_COLOR)"; \
	else \
		printf "%-60b%b\n" "$(COM_COLOR)Linking $(OBJ_COLOR)$(TARGET)$(NO_COLOR)" "$(ERROR_COLOR)$(ERROR_STRING)$(NO_COLOR)"; \
		exit $$RESULT; \
	fi

$(TEST_TARGET): $(TEST_OBJS) | $(BUILD_DIR)
	@printf "%bCompiling tests...%b\n" "$(COM_COLOR)" "$(NO_COLOR)"
	@$(CC) $(TEST_OBJS) -o $@ $(LDFLAGS); \
	RESULT=$$?; \
	if [ $$RESULT -eq 0 ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)Linking $(OBJ_COLOR)$(TEST_TARGET)$(NO_COLOR)" "$(OK_COLOR)$(OK_STRING)$(NO_COLOR)"; \
	else \
		printf "%-60b%b\n" "$(COM_COLOR)Linking $(OBJ_COLOR)$(TEST_TARGET)$(NO_COLOR)" "$(ERROR_COLOR)$(ERROR_STRING)$(NO_COLOR)"; \
		exit $$RESULT; \
	fi

# Pretty compilation pattern rules
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(@D)
	@printf "%b%b %b$(OBJ_COLOR)%s%b\n" "$(COM_COLOR)$(COM_STRING)" "$(NO_COLOR)" "$(OBJ_COLOR)$(@F)$(NO_COLOR)"
	@$(CC) $(CPPFLAGS) -c $< -o $@ 2> $@.tmp.log; \
	RESULT=$$?; \
	if [ $$RESULT -ne 0 ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) $(OBJ_COLOR)$(@F)" "$(ERROR_COLOR)$(ERROR_STRING)$(NO_COLOR)"; \
		cat $@.tmp.log; \
	elif [ -s $@.tmp.log ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) $(OBJ_COLOR)$(@F)" "$(WARN_COLOR)$(WARN_STRING)$(NO_COLOR)"; \
		cat $@.tmp.log; \
	else \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) $(OBJ_COLOR)$(@F)" "$(OK_COLOR)$(OK_STRING)$(NO_COLOR)"; \
	fi; \
	rm -f $@.tmp.log; \
	exit $$RESULT

$(BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(@D)
	@printf "%b%b %b$(OBJ_COLOR)test_%s%b\n" "$(COM_COLOR)$(COM_STRING)" "$(NO_COLOR)" "$(OBJ_COLOR)$(notdir $(<F))$(NO_COLOR)"
	@$(CC) $(CPPFLAGS) -c $< -o $@ 2> $@.tmp.log; \
	RESULT=$$?; \
	if [ $$RESULT -ne 0 ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) test_$(OBJ_COLOR)$(notdir $(<F))" "$(ERROR_COLOR)$(ERROR_STRING)$(NO_COLOR)"; \
		cat $@.tmp.log; \
	elif [ -s $@.tmp.log ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) test_$(OBJ_COLOR)$(notdir $(<F))" "$(WARN_COLOR)$(WARN_STRING)$(NO_COLOR)"; \
		cat $@.tmp.log; \
	else \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) test_$(OBJ_COLOR)$(notdir $(<F))" "$(OK_COLOR)$(OK_STRING)$(NO_COLOR)"; \
	fi; \
	rm -f $@.tmp.log; \
	exit $$RESULT

# ImGui core files
$(BUILD_DIR)/%.o: $(IMGUI_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(@D)
	@printf "%b%b %bimgui/$(OBJ_COLOR)%s%b\n" "$(COM_COLOR)$(COM_STRING)" "$(NO_COLOR)" "$(OBJ_COLOR)$(<F)$(NO_COLOR)"
	@$(CC) $(CPPFLAGS) -c $< -o $@ 2> $@.tmp.log; \
	RESULT=$$?; \
	if [ $$RESULT -ne 0 ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) imgui/$(OBJ_COLOR)$(<F)" "$(ERROR_COLOR)$(ERROR_STRING)$(NO_COLOR)"; \
		cat $@.tmp.log; \
	elif [ -s $@.tmp.log ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) imgui/$(OBJ_COLOR)$(<F)" "$(WARN_COLOR)$(WARN_STRING)$(NO_COLOR)"; \
		cat $@.tmp.log; \
	else \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) imgui/$(OBJ_COLOR)$(<F)" "$(OK_COLOR)$(OK_STRING)$(NO_COLOR)"; \
	fi; \
	rm -f $@.tmp.log; \
	exit $$RESULT

# ImGui backend files
$(BUILD_DIR)/%.o: $(IMGUI_BACKENDS)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(@D)
	@printf "%b%b %bbackends/$(OBJ_COLOR)%s%b\n" "$(COM_COLOR)$(COM_STRING)" "$(NO_COLOR)" "$(OBJ_COLOR)$(<F)$(NO_COLOR)"
	@$(CC) $(CPPFLAGS) -c $< -o $@ 2> $@.tmp.log; \
	RESULT=$$?; \
	if [ $$RESULT -ne 0 ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) backends/$(OBJ_COLOR)$(<F)" "$(ERROR_COLOR)$(ERROR_STRING)$(NO_COLOR)"; \
		cat $@.tmp.log; \
	elif [ $$S $@.tmp.log ]; then \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) backends/$(OBJ_COLOR)$(<F)" "$(WARN_COLOR)$(WARN_STRING)$(NO_COLOR)"; \
		cat $@.tmp.log; \
	else \
		printf "%-60b%b\n" "$(COM_COLOR)$(COM_STRING) backends/$(OBJ_COLOR)$(<F)" "$(OK_COLOR)$(OK_STRING)$(NO_COLOR)"; \
	fi; \
	rm -f $@.tmp.log; \
	exit $$RESULT

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET) $(ARGS)

test: $(TARGET)
	./$(TARGET) $(ARGS) --test

test-verbose: $(TARGET)
	./$(TARGET) --test-verbose

clean:
	@printf "%bCleaning build directory...%b\n" "$(WARN_COLOR)" "$(NO_COLOR)"
	rm -rf $(BUILD_DIR)
	@printf "%bClean complete%b\n" "$(OK_COLOR)" "$(NO_COLOR)"

force: clean all

.PHONY: all clean run force test test-verbose
