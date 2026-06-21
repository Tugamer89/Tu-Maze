#pragma once

#include <chrono>
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

    void updateElapsedTime();

   public:
    SessionManager() = default;

    [[nodiscard]] bool isRunning() const { return running; }

    void start();
    void stop();
    void reset();
    void update();

    [[nodiscard]] std::string getFormattedTime() const;

    void saveScore(unsigned int seed, int difficulty) const;

    [[nodiscard]] std::vector<ScoreRecord> getLeaderboard() const;
};
