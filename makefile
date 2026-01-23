CC := g++

# Use pkg-config to get exact Nix store paths for the LSP
SDL_CFLAGS := $(shell pkg-config --cflags sdl3 sdl3-ttf)
SDL_LIBS   := $(shell pkg-config --libs sdl3 sdl3-ttf)

ARGS =

IMGUI_DIR := imgui
IMGUI_BACKENDS := $(IMGUI_DIR)/backends

#IMGUIZMO_DIR := imguizmo

CPPFLAGS := -Wall -Wextra -g -Iinclude $(SDL_CFLAGS)
CPPFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_BACKENDS)
LDFLAGS  := $(SDL_LIBS) -lm -lGL -lGLEW -lglfw

SRC_DIR   := src
TEST_SRC_DIR := tests
BUILD_DIR := build

# Your app sources
SRCS      := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRCS := $(wildcard $(TEST_SRC_DIR/*.cpp))

# ImGui sources (explicit list to avoid unwanted files)
IMGUI_SRCS := $(IMGUI_DIR)/imgui.cpp \
              $(IMGUI_DIR)/imgui_demo.cpp \
              $(IMGUI_DIR)/imgui_draw.cpp \
              $(IMGUI_DIR)/imgui_widgets.cpp \
              $(IMGUI_DIR)/imgui_tables.cpp \
              $(IMGUI_BACKENDS)/imgui_impl_sdl3.cpp \
              $(IMGUI_BACKENDS)/imgui_impl_opengl3.cpp \
	      $(IMGUI_BACKENDS)/imgui_impl_sdlrenderer3.cpp 

#IMGUIZMO_SRCS := $(IMGUIZMO_DIR)/ImGuizmo.cpp

OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TEST_OBJS := $(patsubst $(TEST_SRC_DIR)/%.cpp, $(BUILD_DIR), $(TEST_SRCS))
IMGUI_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(IMGUI_SRCS)))
#IMGUIZMO_OBJS := $(patsubst $(IMGUIZMO_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(notdir $(IMGUIZMO_SRCS)))

ALL_OBJS := $(OBJS) $(IMGUI_OBJS) #$(IMGUIZMO_OBJS)

TARGET := $(BUILD_DIR)/app

all: $(TARGET)

$(TARGET): $(ALL_OBJS)
	$(CC) $(ALL_OBJS) -o $@ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $@ $(LDFLAGS)

# Your app object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

# ImGui core files
$(BUILD_DIR)/%.o: $(IMGUI_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

# ImGui backend files
$(BUILD_DIR)/%.o: $(IMGUI_BACKENDS)/%.cpp | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

# ImGuiZmo
$(BUILD_DIR)/%.o: $(IMGUIZMO_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET) $(ARGS)

test: $(TEST_TARGET)
	./$(TARGET) --test

test-verbose: $(TEST_TARGET)
	./$(TARGET) --test-verbose

clean:
	rm -rf $(BUILD_DIR)

force:
	$(MAKE) clean all

.PHONY: all clean run force test test-verbose
