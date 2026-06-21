#pragma once

#include <algorithm>
#include <chrono>
#include <ctime>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct ScoreRecord {
    long long timestamp;
    unsigned int seed;
    std::string timeStr;
    int difficulty;
};

// Manages time tracking, formatting, and file I/O for high scores
class SessionManager {
   private:
    double elapsedTime = 0.0;
    double accumulatedTime = 0.0;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    bool running = false;

    static constexpr const char* scoresFile = "tu-maze_scores.txt";

    void updateElapsedTime() {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = currentTime - startTime;
        elapsedTime = accumulatedTime + elapsed.count();
    }

   public:
    SessionManager() = default;

    [[nodiscard]] bool isRunning() const { return running; }

    void start() {
        if (!running) {
            startTime = std::chrono::steady_clock::now();
            running = true;
        }
    }

    void stop() {
        if (running) {
            updateElapsedTime();
            accumulatedTime = elapsedTime;
            running = false;
        }
    }

    void reset() {
        running = false;
        elapsedTime = 0.0;
        accumulatedTime = 0.0;
    }

    void update() {
        if (running) {
            updateElapsedTime();
        }
    }

    [[nodiscard]] std::string getFormattedTime() const {
        double totalSeconds = elapsedTime;

        auto minutes = static_cast<int>(totalSeconds) / 60;
        auto seconds = static_cast<int>(totalSeconds) % 60;
        auto centiseconds = static_cast<int>((totalSeconds - static_cast<int>(totalSeconds)) * 100);

        return std::format("{:02}:{:02}:{:02}", minutes, seconds, centiseconds);
    }

    void saveScore(unsigned int seed, int difficulty) const {
        std::ofstream file(scoresFile, std::ios::app);

        if (!file.is_open()) {
            std::cerr << "Error: Unable to open score file." << std::endl;
            return;
        }

        const auto now = std::chrono::system_clock::now();
        // Epoch casting ensures universally sortable UNIX timestamp formats
        const auto timestamp_seconds =
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        file << timestamp_seconds << " " << seed << " " << getFormattedTime() << " " << difficulty
             << "\n";
        file.close();
    }

    [[nodiscard]] std::vector<ScoreRecord> getLeaderboard() const {
        std::vector<ScoreRecord> scores;
        std::ifstream file(scoresFile);

        if (!file.is_open()) return scores;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            long long ts = 0;
            unsigned int seed = 0;
            std::string tStr;
            int diff = 0;

            if (iss >> ts >> seed >> tStr >> diff) {
                scores.emplace_back(ts, seed, std::move(tStr), diff);
            }
        }

        std::ranges::sort(scores, [](const ScoreRecord& a, const ScoreRecord& b) {
            return a.timeStr == b.timeStr ? a.timestamp < b.timestamp : a.timeStr < b.timeStr;
        });

        return scores;
    }
};
