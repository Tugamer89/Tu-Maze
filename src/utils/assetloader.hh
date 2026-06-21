#pragma once

#include <functional>
#include <queue>
#include <string>

struct LoadingTask {
    std::string description;
    std::function<void()> action;
};

// Pseudo-asynchronous queue system allowing UI updates during heavy initializations
class AssetLoader {
   public:
    void addTask(std::string desc, std::function<void()> act);

    [[nodiscard]] bool isFinished() const { return tasks.empty(); }

    [[nodiscard]] float getProgress() const {
        if (totalTasks == 0) return 1.0f;
        return 1.0f - (static_cast<float>(tasks.size()) / static_cast<float>(totalTasks));
    }

    std::string processNext();

   private:
    std::queue<LoadingTask> tasks;
    size_t totalTasks = 0;
};
