#pragma once

#include <stdexcept>
#include <string>

namespace exceptions {

class ShaderException : public std::runtime_error {
   public:
    using runtime_error::runtime_error;
};

class MeshException : public std::runtime_error {
   public:
    using runtime_error::runtime_error;
};

class EngineSetupException : public std::runtime_error {
   public:
    using runtime_error::runtime_error;
};

class GuiException : public std::runtime_error {
   public:
    using runtime_error::runtime_error;
};

class MazeException : public std::out_of_range {
   public:
    using out_of_range::out_of_range;
};

}  // namespace exceptions
