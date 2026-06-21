#include "utils/assetloader.hpp"

void AssetLoader::addTask(std::string desc, std::function<void()> act) {
    tasks.emplace(std::move(desc), std::move(act));
    totalTasks++;
}

std::string AssetLoader::processNext() {
    if (tasks.empty()) {
        return "";
    }

    LoadingTask currentTask = std::move(tasks.front());
    tasks.pop();
    currentTask.action();

    return currentTask.description;
}
