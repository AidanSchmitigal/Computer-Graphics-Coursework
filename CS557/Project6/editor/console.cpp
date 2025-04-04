#include "console.h"

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "imgui.h"

namespace ConsoleWindow {

bool viewConsolePanel = true;

int pipefd[2];
int stdout_backup;
std::thread read_thread;
std::mutex mutex;
std::deque<std::string> lines;  // Store lines separately for better scrolling
bool running = true;
static constexpr size_t MAX_LINES = 1000;  // Prevent unlimited memory usage
bool auto_scroll = true;

void ReadThread() {
  std::array<char, 4096> buffer;
  std::string partial_line;

  while (running) {
    ssize_t bytes_read = read(pipefd[0], buffer.data(), buffer.size());
    if (bytes_read > 0) {
      std::lock_guard<std::mutex> lock(mutex);

      // Process the buffer into lines
      for (size_t i = 0; i < bytes_read; i++) {
        if (buffer[i] == '\n') {
          // Add completed line
          lines.push_back(partial_line);
          if (lines.size() > MAX_LINES) {
            lines.pop_front();
          }
          partial_line.clear();
        } else {
          partial_line += buffer[i];
        }
      }
    }
  }
}

void CreateConsole() {
  // Create pipe
  if (pipe(pipefd) == -1) {
    throw std::runtime_error("Failed to create pipe");
  }

  // Save original stdout
  stdout_backup = dup(STDOUT_FILENO);

  // Redirect stdout to pipe
  dup2(pipefd[1], STDOUT_FILENO);

  // Set non-blocking mode
  fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

  // Start reading thread
  read_thread = std::thread(&ConsoleWindow::ReadThread);
}

void DestroyConsole() {
  running = false;

  // Restore original stdout
  dup2(stdout_backup, STDOUT_FILENO);
  close(pipefd[0]);
  close(pipefd[1]);

  if (read_thread.joinable()) {
    read_thread.join();
  }
}

void ShowConsolePanel() {
  if (!viewConsolePanel) return;

  ImGui::Begin("Console");

  // Add a clear button
  if (ImGui::Button("Clear")) {
    std::lock_guard<std::mutex> lock(mutex);
    lines.clear();
  }

  ImGui::SameLine();
  ImGui::Checkbox("Auto-scroll", &auto_scroll);

  // Create a child window for scrolling
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                    false, ImGuiWindowFlags_HorizontalScrollbar);

  {
    std::lock_guard<std::mutex> lock(mutex);
    for (const auto& line : lines) {
      ImGui::TextUnformatted(line.c_str());
    }
  }

  // Auto-scroll
  if (auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    ImGui::SetScrollHereY(1.0f);
  }

  ImGui::EndChild();

  // Add an input line at the bottom
  static char input_buf[256] = "";
  if (ImGui::InputText("Input", input_buf, IM_ARRAYSIZE(input_buf),
                       ImGuiInputTextFlags_EnterReturnsTrue)) {
    printf("%s\n", input_buf);  // This will be captured by our console
    input_buf[0] = '\0';        // Clear input
  }

  ImGui::End();
}

}  // namespace ConsoleWindow