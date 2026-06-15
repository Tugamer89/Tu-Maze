#ifndef SESSION_HH
#define SESSION_HH

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
};

class SessionManager {
   private:
    double elapsedTime = 0.0;
    double accumulatedTime = 0.0;
    std::chrono::time_point<std::chrono::steady_clock> startTime;

    void updateElapsedTime() {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = currentTime - startTime;
        elapsedTime = accumulatedTime + elapsed.count();
    }

   public:
    inline static const std::string scoresFile = "tu-maze_scores.txt";
    bool isRunning = false;

    SessionManager() = default;

    // Start or resume timer
    void start() {
        if (!isRunning) {
            startTime = std::chrono::steady_clock::now();
            isRunning = true;
        }
    }

    // Stop/Pause timer
    void stop() {
        if (isRunning) {
            updateElapsedTime();
            accumulatedTime = elapsedTime;
            isRunning = false;
        }
    }

    // Reset timer and state
    void reset() {
        isRunning = false;
        elapsedTime = 0.0;
        accumulatedTime = 0.0;
    }

    void update() {
        if (isRunning) {
            updateElapsedTime();
        }
    }

    std::string getFormattedTime() const {
        double totalSeconds = elapsedTime;

        int minutes = static_cast<int>(totalSeconds) / 60;
        int seconds = static_cast<int>(totalSeconds) % 60;
        auto centiseconds = static_cast<int>((totalSeconds - static_cast<int>(totalSeconds)) * 100);

        return std::format("{:02}:{:02}:{:02}", minutes, seconds, centiseconds);
    }

    void saveScore(unsigned int seed) const {
        std::ofstream file(scoresFile, std::ios::app);

        if (!file.is_open()) {
            std::cerr << "Error: Unable to open score file." << std::endl;
            return;
        }

        const auto now = std::chrono::system_clock::now();
        const auto timestamp_seconds =
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        file << timestamp_seconds << " " << seed << " " << getFormattedTime() << "\n";

        file.close();
    }

    std::vector<ScoreRecord> getLeaderboard() const {
        std::vector<ScoreRecord> scores;
        std::ifstream file(scoresFile);

        if (!file.is_open()) return scores;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            long long ts;
            unsigned int seed;
            std::string tStr;

            if (iss >> ts >> seed >> tStr) {
                scores.emplace_back(ts, seed, tStr);
            }
        }

        // Sort ascending by time
        std::ranges::sort(scores, [](const ScoreRecord& a, const ScoreRecord& b) {
            return a.timeStr == b.timeStr ? a.timestamp < b.timestamp : a.timeStr < b.timeStr;
        });

        // Top 50 records
        if (scores.size() > 50) {
            scores.resize(50);
        }

        return scores;
    }
};

#endif
