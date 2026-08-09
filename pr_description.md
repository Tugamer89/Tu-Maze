🎯 **What:**
The PRNG seed generation in `Maze::generate` relied on `std::chrono::high_resolution_clock::now().time_since_epoch().count()`. This is not a secure way to generate seeds. It was replaced with `std::random_device`, which provides a cryptographically secure random number, when an explicit seed is not provided.

⚠️ **Risk:**
Using system time for random seed generation leads to predictable maze configurations. Since time is easily observable or guessable, an attacker could predict the exact layout of the maze generated at a given moment, potentially exploiting game mechanics relying on this randomization.

🛡️ **Solution:**
The vulnerability was resolved by replacing the time-based seed generation with `std::random_device`, which is designed for non-deterministic random number generation where available, substantially increasing the randomness of the generated maze when no specific seed is specified.
