#ifndef ASSETLOADER_HH
#define ASSETLOADER_HH

#include <functional>
#include <queue>
#include <string>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

struct LoadingTask {
    std::string description;
    std::function<void()> action;
};

class AssetLoader {
   public:
    void addTask(std::string desc, std::function<void()> act) {
        tasks.emplace(std::move(desc), std::move(act));
        totalTasks++;
    }

    bool isFinished() const { return tasks.empty(); }

    float getProgress() const {
        if (totalTasks == 0) return 1.0f;
        // Calculate progress based on remaining tasks
        return 1.0f - (static_cast<float>(tasks.size()) / static_cast<float>(totalTasks));
    }

    std::string processNext() {
        if (tasks.empty()) {
            return "";
        }

        // Fetch the next task
        LoadingTask currentTask = std::move(tasks.front());
        tasks.pop();

        // Execute the task
        currentTask.action();

        return currentTask.description;
    }

   private:
    std::queue<LoadingTask> tasks;
    size_t totalTasks = 0;
};

#endif
