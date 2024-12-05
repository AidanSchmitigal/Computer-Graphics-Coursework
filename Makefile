CXX = g++



EXE = sample.runme
IMGUI_DIR = ./imgui
# INCLUDES = ./imnodes ./utils ./plants ./objects
BUILD_DIR = ./build

# SOURCES = sample.cpp
# SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
# SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glut.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp
# SOURCES += $(shell find $(INCLUDES) -name "*.cpp")
# SOURCES = sample.cpp $(shell find ./imnodes ./utils ./plants ./objects -name "*.cpp")

# OBJS =  $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
# OBJS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.cpp=.o)))

# CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
# CXXFLAGS += -g -w
# -Wall -Wformat -Wno-deprecated -Wc++11-extensions
# LIBS = -framework OpenGL -framework GLUT -I.
# INCLUDES = ./imnodes ./utils ./plants ./objects ./imgui ./imgui/backends
# CXXFLAGS = -std=c++11 -g -Wall -Wformat -Wno-deprecated -Wc++11-extensions $(addprefix -I, $(INCLUDES))
# LIBS = -framework OpenGL -framework GLUT


# ECHO_MESSAGE = "Mac OS X"

SOURCES = sample.cpp $(shell find ./imnodes ./utils ./plants ./objects ./editor -name "*.cpp")
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp $(IMGUI_DIR)/imgui_stdlib.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glut.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl2.cpp
# SOURCES += $(shell find ./glm -name "*.cpp" -o -name "*.hpp" -o -name "*.inl")
OBJS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

INCLUDES = ./imnodes ./utils ./plants ./objects ./imgui ./imgui/backends ./
CXXFLAGS = -std=c++17 -g -Wall -Wformat -Wno-deprecated -Wc++11-extensions $(addprefix -I, $(INCLUDES))
LIBS = -framework OpenGL -framework GLUT

# Object file rule (preserve subdirectories)
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@) # Ensure subdirectories exist
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE) | $(BUILD_DIR)
	@echo Build complete!

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
