CXX = g++

EXE = sample.runme
IMGUI_DIR = ./imgui
BUILD_DIR = ./build

SOURCES = sample.cpp $(shell find ./utils ./object ./editor -name "*.cpp")
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/imgui_stdlib.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
SOURCES += ./imguizmo/imguizmo.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

INCLUDES = ./imgui ./imgui/backends ./imguizmo ./
SYSTEM_INCLUDES = ./glfw/include
CXXFLAGS = -std=c++17 -g -Wall -Wformat -Wno-deprecated -Wc++11-extensions $(addprefix -I, $(INCLUDES)) $(addprefix -isystem, $(SYSTEM_INCLUDES))
LDFLAGS = -L./glfw/lib
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -lglfw3
#  -framework GLUT 

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE) | $(BUILD_DIR)
	@echo Build complete!

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) $(LDFLAGS) 

clean:
	rm -f $(EXE) $(OBJS)