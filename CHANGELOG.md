# Changelog

## [1.2.3](https://github.com/Tugamer89/Tu-Maze/compare/v1.2.2...v1.2.3) (2026-06-23)


### Bug Fixes

* update mouse position retrieval to use default window context ([0f89d67](https://github.com/Tugamer89/Tu-Maze/commit/0f89d67072903d926ada2cf3d98b29955bd4349a))

## [1.2.2](https://github.com/Tugamer89/Tu-Maze/compare/v1.2.1...v1.2.2) (2026-06-23)


### Bug Fixes

* remove redundant comment about Y-axis inversion in mouse movement ([772c3df](https://github.com/Tugamer89/Tu-Maze/commit/772c3df2482286b41c1a56d82e673843b7969662))
* simplify conditions in run_engine for clarity and performance ([320cd82](https://github.com/Tugamer89/Tu-Maze/commit/320cd82a3dfa2ff4dd5f63c05c0395ebc7193046))

## [1.2.1](https://github.com/Tugamer89/Tu-Maze/compare/v1.2.0...v1.2.1) (2026-06-23)


### Bug Fixes

* simplify event handling in RawMouse for improved delta accumulation ([8d7f0b1](https://github.com/Tugamer89/Tu-Maze/commit/8d7f0b11138702340a1eabf1bce935fb0c45dca1))

## [1.2.0](https://github.com/Tugamer89/Tu-Maze/compare/v1.1.1...v1.2.0) (2026-06-23)


### Features

* implement RawMouse class for improved mouse handling on macOS ([1a30683](https://github.com/Tugamer89/Tu-Maze/commit/1a3068399d837232561ff6335dd59fa3dfd811a3))


### Documentation

* update README.md to reorganize user interface controls and add stage-specific commands ([e38e817](https://github.com/Tugamer89/Tu-Maze/commit/e38e81729618bcf6cd17a79523fdbb0757f9606b))
* update REPORT.md to format key controls with keyboard notation ([c5495e7](https://github.com/Tugamer89/Tu-Maze/commit/c5495e734ee9db76e2925ee25ad605febd08c8a5))
* update REPORT.md to include Stage 33 details and clarify macOS compatibility issues ([d0424c2](https://github.com/Tugamer89/Tu-Maze/commit/d0424c2b66710a33c9efab0d5b9edc75a24f1c34))

## [1.1.1](https://github.com/Tugamer89/Tu-Maze/compare/v1.1.0...v1.1.1) (2026-06-23)


### Documentation

* add new screenshots for Stage 29 and Stage 32 ([c14fbcb](https://github.com/Tugamer89/Tu-Maze/commit/c14fbcbc9e623534543949636ed44bdc90d35385))
* enhanced REPORT.md ([f06eed2](https://github.com/Tugamer89/Tu-Maze/commit/f06eed2ce5e725ac50427e62f000fa47949d922d))

## [1.1.0](https://github.com/Tugamer89/Tu-Maze/compare/v1.0.0...v1.1.0) (2026-06-22)


### Features

* splitted minimap and controls & camera settings into separate tabs ([739033c](https://github.com/Tugamer89/Tu-Maze/commit/739033ca3767f5a3141dc13b5a6ad6f16c685363))


### Documentation

* updated with Stage 32 features ([e1daf04](https://github.com/Tugamer89/Tu-Maze/commit/e1daf0421cd6c931e196d1125a026524cd881ded))

## [1.0.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.31.0...v1.0.0) (2026-06-21)


### ⚠ BREAKING CHANGES

* bump new version

### Features

* bump new version ([400a3b0](https://github.com/Tugamer89/Tu-Maze/commit/400a3b0c48f31e17f06e796d8b768cee3bda0886))
* bump new version ([c1716b4](https://github.com/Tugamer89/Tu-Maze/commit/c1716b41e54f222befada97444e453a6374e81ec))


### Bug Fixes

* **build:** update source file globbing to include main.cpp and subdirectory sources ([91dd6b9](https://github.com/Tugamer89/Tu-Maze/commit/91dd6b9b702da0560217c1815e5b6a8a53ac20a2))
* **format:** include additional source files in clang-format command ([7c12012](https://github.com/Tugamer89/Tu-Maze/commit/7c12012fb6d29619266b56e31f1555a72ad2465d))
* **format:** update file extension from .hh to .hpp in clang-format command ([e6e6f1d](https://github.com/Tugamer89/Tu-Maze/commit/e6e6f1d1a85e4fee372b433abe0ad331caa80be9))
* **release:** set bump-minor-pre-major to false in release configuration ([d6059d5](https://github.com/Tugamer89/Tu-Maze/commit/d6059d533269b8fba376fad614105fdefaa033fd))


### Code Refactoring

* **arch:** reorganize project files into subdirectories ([f1bbc66](https://github.com/Tugamer89/Tu-Maze/commit/f1bbc66b637a42fe82e7fc8ce43720d8488fe94f))
* **arch:** separate interfaces from implementations and modularize source ([7ffb423](https://github.com/Tugamer89/Tu-Maze/commit/7ffb42323caac945325eebc325e3f6a9bfdd0777))
* **engine:** split Gui class and group Camera fields to meet SonarQube limits ([d84f4a2](https://github.com/Tugamer89/Tu-Maze/commit/d84f4a2707308a9c17cdbd0cb24dda39728f97ff))
* renamed headers from .hh to .hpp ([4682dff](https://github.com/Tugamer89/Tu-Maze/commit/4682dffa7a37e05df1874133cdd2fb36b00fbc92))


### Documentation

* update Stage 31 REPORT ([08f7ce5](https://github.com/Tugamer89/Tu-Maze/commit/08f7ce57c605c2d72048f2eb8e5132401f5a3567))
* update Stage 31 version from v0.32.x to v1.0.x ([d7bf25d](https://github.com/Tugamer89/Tu-Maze/commit/d7bf25deed6d2024caf4fea0ccee046dd3a46ce2))
* update Stage 31 versioning ([396bcae](https://github.com/Tugamer89/Tu-Maze/commit/396bcae977d36a6ea8c97c7e47934159e1c2d020))

## [0.31.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.30.0...v0.31.0) (2026-06-21)


### Features

* bump new version ([8c7c671](https://github.com/Tugamer89/Tu-Maze/commit/8c7c67196864090f9b70540ffadb7b816ec19b59))


### Bug Fixes

* update format target to include all header files in subdirectories ([df6b064](https://github.com/Tugamer89/Tu-Maze/commit/df6b0646edac4fd88beb7841d3fb168e66d2c267))


### Code Refactoring

* error handling and improve encapsulation across multiple classes ([a89767e](https://github.com/Tugamer89/Tu-Maze/commit/a89767e894d0e350eadd9a089d6abe4e0a1b3480))
* improved codebase for readability and maintainability ([fac4a5b](https://github.com/Tugamer89/Tu-Maze/commit/fac4a5b1093b77869f1eb0e7f0ecfacefc4f0f92))


### Documentation

* update Stage 30 versioning ([0695c98](https://github.com/Tugamer89/Tu-Maze/commit/0695c98234f20740a97ba8fe150d439d0269595e))
* updated with Stage 30 features ([bbf5c55](https://github.com/Tugamer89/Tu-Maze/commit/bbf5c55348348821a588c0b249453790cdc260b4))

## [0.30.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.29.0...v0.30.0) (2026-06-21)


### Features

* enhance camera controls with sensitivity and bobbing settings ([64c9600](https://github.com/Tugamer89/Tu-Maze/commit/64c96005f297a21226f54d3e1c1c3b6b4413fa83))


### Documentation

* updated with Stage 29 features ([030a8ef](https://github.com/Tugamer89/Tu-Maze/commit/030a8ef42b423fdf3db394bb96b308064a621f14))

## [0.29.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.28.0...v0.29.0) (2026-06-21)


### Features

* add macOS cross-compilation support and update Makefile targets ([b0af22a](https://github.com/Tugamer89/Tu-Maze/commit/b0af22ac28431fd0fa3279f8c067557c6e97565e))
* add mouse movement handling for macOS to improve user experience ([b864b99](https://github.com/Tugamer89/Tu-Maze/commit/b864b99054b03362adbf8515cb2456062b71bde8))
* fixed minimap rendering on MacOS using ImGui instead of native OpenGL ([bb2570f](https://github.com/Tugamer89/Tu-Maze/commit/bb2570f17b7354a70c88c3ead75698849a541076))
* implement mouse state management and improve mouse handling in game events ([7e3d969](https://github.com/Tugamer89/Tu-Maze/commit/7e3d9691e90462acbf0e857024daeab843bbe569))


### Bug Fixes

* adjust minimap display size for improved visibility ([c90057c](https://github.com/Tugamer89/Tu-Maze/commit/c90057cb8d81782d6f3fc6eaea5ef7d5bd5bd606))
* correct depth attachment type in framebuffer setup and fix typo in comment ([d828c33](https://github.com/Tugamer89/Tu-Maze/commit/d828c339d02754d5ecd912a8dd2e435df120fab8))
* correct depthBits setting in SFML context to 24 ([a44d32a](https://github.com/Tugamer89/Tu-Maze/commit/a44d32ade84e32d70a08c4a6fce54fc08280a0d9))
* disable mouse cursor grabbing on macOS for improved compatibility ([ab286e3](https://github.com/Tugamer89/Tu-Maze/commit/ab286e3144325efaa28cd43766e1e59d1de46e2a))
* improve FBO initialization for better MSAA compatibility on macOS ([048e29a](https://github.com/Tugamer89/Tu-Maze/commit/048e29a4b0c110147d4ecdd9a6dc0012c95f77d6))
* improve mouse cursor handling for macOS compatibility ([f09dd19](https://github.com/Tugamer89/Tu-Maze/commit/f09dd19a83124ffacc6e55ad66c2a8e460baeb7e))
* removed cursor grabbed for MacOS compatibility ([b38482f](https://github.com/Tugamer89/Tu-Maze/commit/b38482fd1c5ca57ff58eb8ffe78501df8b6ac517))
* update artifact name format to include 'tu-maze' prefix ([50c2dae](https://github.com/Tugamer89/Tu-Maze/commit/50c2dae6a20133c0576d58caa4af66272bb2c1ab))
* update renderbuffer and texture formats to use GL_RGBA8 for improved color accuracy ([e28cdd3](https://github.com/Tugamer89/Tu-Maze/commit/e28cdd39106e7ef0e4a2bf80da997e92133c9333))


### Documentation

* update macOS configuration notes and improve cross-platform compatibility details ([3b46c0b](https://github.com/Tugamer89/Tu-Maze/commit/3b46c0b8aa11eb3eafe0b9d1004c7b3e0e7d3123))
* update macOS configuration notes to clarify mouse movement issues ([c90a719](https://github.com/Tugamer89/Tu-Maze/commit/c90a719a1f93dc4afa828ca0d682487ec8b5e170))
* updated REPORT with latest changes ([43a08c2](https://github.com/Tugamer89/Tu-Maze/commit/43a08c2c3c3258b0ff2d8035bac4c9fd3ea15132))
* updated with Stage 28 features ([d71606d](https://github.com/Tugamer89/Tu-Maze/commit/d71606d165d2ed3556fad3a89ddf0a824f90a9c9))

## [0.28.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.27.0...v0.28.0) (2026-06-18)


### Features

* add Windows resources including icon and metadata for application ([0644c2b](https://github.com/Tugamer89/Tu-Maze/commit/0644c2b5771997f50d2743c06fcd4a663b5ab94a))


### Documentation

* updated with Stage 27 features ([8757d18](https://github.com/Tugamer89/Tu-Maze/commit/8757d18cbf28ea2a7f98239461e8b1df4a2994fd))

## [0.27.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.26.1...v0.27.0) (2026-06-18)


### Features

* enhance difficulty selection and score tracking in GUI and session management ([8540638](https://github.com/Tugamer89/Tu-Maze/commit/8540638aff4bec7236da0bb7b25cbb1001557c9f))


### Bug Fixes

* update restartGame lambda to require seed parameter ([4b87402](https://github.com/Tugamer89/Tu-Maze/commit/4b874023e1ad92dc47345098c894857fc14150da))


### Documentation

* updated with Stage 26 features ([4b2f9bc](https://github.com/Tugamer89/Tu-Maze/commit/4b2f9bcb133610a727787e8dfcbead485d779051))

## [0.26.1](https://github.com/Tugamer89/Tu-Maze/compare/v0.26.0...v0.26.1) (2026-06-18)


### Documentation

* include stage 25 demo GIF ([43e9442](https://github.com/Tugamer89/Tu-Maze/commit/43e94421639638e6ce629566bbb0d5e3a3377f3c))

## [0.26.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.25.0...v0.26.0) (2026-06-17)


### Features

* added dynamic FOV while sprinting ([e29f704](https://github.com/Tugamer89/Tu-Maze/commit/e29f704e6e0dc80bc7005a59123e3f920166e92b))
* implement head-bobbing effect ([4f16ef0](https://github.com/Tugamer89/Tu-Maze/commit/4f16ef003888c922e298edb45f93e5c584b4f101))
* toggle light state with 'F' key and update parameters accordingly ([d49ed30](https://github.com/Tugamer89/Tu-Maze/commit/d49ed30d6c3acce7a833f7d931634bb3a42bf190))


### Documentation

* update README and REPORT with Stage 25 features ([f39a502](https://github.com/Tugamer89/Tu-Maze/commit/f39a50260acc6563f007d268391497729dd68cc3))

## [0.25.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.24.0...v0.25.0) (2026-06-17)


### Features

* bump new version ([52e9902](https://github.com/Tugamer89/Tu-Maze/commit/52e99024f3d566522dec07a1bd1d4f3115a33bf9))


### Code Refactoring

* optimize collision detection logic in Camera class ([75e415b](https://github.com/Tugamer89/Tu-Maze/commit/75e415b5c4521a7afc09ac5d4b78e9a25951bffc))
* remove unused matrices header ([eec5277](https://github.com/Tugamer89/Tu-Maze/commit/eec5277de41cb10818e9d1e0469036f578e05faf))
* remove unused matrices header and minimized texture binding ([4490dfd](https://github.com/Tugamer89/Tu-Maze/commit/4490dfd474b2fa7785014fb355b8b339b7427c28))
* update background color for main menu ([9a0128e](https://github.com/Tugamer89/Tu-Maze/commit/9a0128e7f31e440bb1fe1517f90a7d8de58a53da))


### Documentation

* added stage 24 to REPORT.md ([88f422b](https://github.com/Tugamer89/Tu-Maze/commit/88f422bbbbc3b1a7ddc6a593a59d941c64dc89f9))

## [0.24.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.23.0...v0.24.0) (2026-06-16)


### Features

* implement play again functionality with custom seed memory ([2432102](https://github.com/Tugamer89/Tu-Maze/commit/24321023cd177e8cdc5cb9103eafcad68520eb6d))


### Code Refactoring

* group renderUI callbacks into GuiCallbacks struct ([277de86](https://github.com/Tugamer89/Tu-Maze/commit/277de8689d8842fd077077d213baf43c7db7330a))


### Documentation

* add stage 23 description ([34b9452](https://github.com/Tugamer89/Tu-Maze/commit/34b94525c1b939ff06dbeb6a050e15b5e2c18b74))

## [0.23.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.22.0...v0.23.0) (2026-06-15)


### Features

* add settings menu functionality and toggle in main menu ([be90673](https://github.com/Tugamer89/Tu-Maze/commit/be9067346e587e89ff8c0572858bcbf0c2dcb18b))


### Documentation

* add Stage 22 keyboard ([77ba9cd](https://github.com/Tugamer89/Tu-Maze/commit/77ba9cd49dd85c68e4235d5b0ca428949d219e3e))

## [0.22.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.21.0...v0.22.0) (2026-06-15)


### Features

* enhance GUI with main menu and leaderboard functionality; refactor maze generation with optional seed ([901e3be](https://github.com/Tugamer89/Tu-Maze/commit/901e3be5d3563b51ea31ec0183c1e3ec0f65bc0d))


### Bug Fixes

* removed the use of `std::chrono` zonetimes ([2f497c1](https://github.com/Tugamer89/Tu-Maze/commit/2f497c103f62cada21ea3c6bf85ddeedbdd7e114))


### Documentation

* add Stage 21 ([362d3dc](https://github.com/Tugamer89/Tu-Maze/commit/362d3dcc33abecee309df68ad3ceb8046c47a76f))

## [0.21.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.20.0...v0.21.0) (2026-06-15)


### Features

* enhance GUI with FPS overlay and modern styling adjustments ([cbc5a4c](https://github.com/Tugamer89/Tu-Maze/commit/cbc5a4c841d4dfa790f4f9dd76cf71c186763a62))


### Documentation

* add Stage 20 in REPORT.md ([295cf5b](https://github.com/Tugamer89/Tu-Maze/commit/295cf5bedd2ed0797c959e004dbc9dec83bc4616))

## [0.20.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.19.0...v0.20.0) (2026-06-15)


### Features

* add pause functionality and improve session management in GUI ([21d58cf](https://github.com/Tugamer89/Tu-Maze/commit/21d58cfb976d0bd42af794a7ce38c03fafe7d9e0))


### Documentation

* add Stage 19 details ([64bd0f6](https://github.com/Tugamer89/Tu-Maze/commit/64bd0f63e021e4e85acd1c886650a52688aa59e6))

## [0.19.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.18.1...v0.19.0) (2026-06-15)


### Features

* implement default shader with flat and phong shading options; remove old flat shaders ([d31b54b](https://github.com/Tugamer89/Tu-Maze/commit/d31b54b0d1ece28e70a77582d8da975ca74194e7))


### Bug Fixes

* improve normal calculation and gamma correction in fragment shader ([0f4d2a6](https://github.com/Tugamer89/Tu-Maze/commit/0f4d2a60e411c643b0e87d695ca3c7354fb2e2ea))


### Documentation

* add Stage 18 details and controls; introduce new Uber Shader and optimize fragment shader ([8fce5bf](https://github.com/Tugamer89/Tu-Maze/commit/8fce5bfbb3fe212e68054292cc5fa560dd767777))

## [0.18.1](https://github.com/Tugamer89/Tu-Maze/compare/v0.18.0...v0.18.1) (2026-06-15)


### Bug Fixes

* adjust win condition distance threshold for gameplay balance ([a5ffaa1](https://github.com/Tugamer89/Tu-Maze/commit/a5ffaa130b0aaaf3628026c979a107990eba3393))
* update export script to copy REPORT.md and Makefile to FCG_Stages ([bcf411a](https://github.com/Tugamer89/Tu-Maze/commit/bcf411adea4f085bd58f0ca616af41fabd08694e))


### Documentation

* add new screenshots and animations for stages 14, 16, and 17 ([ab2c65a](https://github.com/Tugamer89/Tu-Maze/commit/ab2c65a9dac30db720d9bef4d1867242c40c9965))
* update Stage 17 description to clarify gameplay completion and performance tracking integration ([85363e1](https://github.com/Tugamer89/Tu-Maze/commit/85363e1c8f9d96a0ff13880d0e8a3e45497b2026))

## [0.18.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.17.1...v0.18.0) (2026-06-15)


### Features

* add alpha property to Material struct and update shader calculations ([e77ceca](https://github.com/Tugamer89/Tu-Maze/commit/e77ceca9e1b850fc342d272317479ec0b22cbb11))
* implement session management for game timing and score tracking ([b5147e5](https://github.com/Tugamer89/Tu-Maze/commit/b5147e580ec46fb1e7c42b9ca47e830a9ac66b01))


### Bug Fixes

* refactor collision detection logic in Camera class for improved clarity ([ab9003a](https://github.com/Tugamer89/Tu-Maze/commit/ab9003a887b60fbec591d0047cd384555421e476))


### Documentation

* updated report to include Stage 17 ([9e73bc2](https://github.com/Tugamer89/Tu-Maze/commit/9e73bc21f4b00a85e51bc55818bff05a76c3e82f))

## [0.17.1](https://github.com/Tugamer89/Tu-Maze/compare/v0.17.0...v0.17.1) (2026-06-14)


### Bug Fixes

* remove inexistent Material parameter ([1ea443c](https://github.com/Tugamer89/Tu-Maze/commit/1ea443c74ce6ec841e9050735defd4056b184185))
* remove unnecessary alpha parameter from asset registration ([9092928](https://github.com/Tugamer89/Tu-Maze/commit/90929282a88a7253ea8e6674f4b4884a1e784335))


### Code Refactoring

* simplify asset registration by removing unnecessary parameters ([a6fdbb3](https://github.com/Tugamer89/Tu-Maze/commit/a6fdbb3a84e25dca3ba70a88aa56bdacc832f7b3))

## [0.17.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.16.0...v0.17.0) (2026-06-14)


### Features

* add gameplay update logic and win condition check ([779880d](https://github.com/Tugamer89/Tu-Maze/commit/779880d70cdf24306b75ac159f9c2d7c121a4968))
* dynamically rename CMake target to avoid conflicts in build tree ([a94c5c5](https://github.com/Tugamer89/Tu-Maze/commit/a94c5c51cc244ac1d2e14ab2e4048bdb8eb0d4eb))
* enable parallel build execution in compile_command function ([613f795](https://github.com/Tugamer89/Tu-Maze/commit/613f79557b318cd3338f9030b8102ad21a5750d3))
* implement gameplay and victory condition, add goal marker and animations ([4df1509](https://github.com/Tugamer89/Tu-Maze/commit/4df150944576d3bf224cc13474dbc7b8250719fb))


### Bug Fixes

* standardize use of texture uniform variable naming in shaders and scene management ([d6c53c9](https://github.com/Tugamer89/Tu-Maze/commit/d6c53c9056baec44b09457f5a74053c0921422e2))
* update build commands in README and export script for consistency ([598631f](https://github.com/Tugamer89/Tu-Maze/commit/598631fc59f8cd4b7971711aaee2a667278abb37))


### Code Refactoring

* improve gameplay update logic and asset registration ([615082d](https://github.com/Tugamer89/Tu-Maze/commit/615082d93728ad9f43d248553ae883ad2a0d692a))

## [0.16.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.15.0...v0.16.0) (2026-06-14)


### Features

* add Stage 15 keyboard and mouse controls to README and REPORT, implement collision detection and sprint mechanics in camera movement and replace hardcoded values with constants for cell size and wall dimensions ([da85f8e](https://github.com/Tugamer89/Tu-Maze/commit/da85f8e73122d05f9a478e01bf251f7abcd09427))


### Bug Fixes

* update .gitignore entry for FCG_Stages to include trailing slash ([c2952d4](https://github.com/Tugamer89/Tu-Maze/commit/c2952d49d75efaa745188524f56e1c7165c21ab7))


### Documentation

* **README:** update keyboard controls section for clarity ([29ee5c2](https://github.com/Tugamer89/Tu-Maze/commit/29ee5c20cfe494c84b9edc012d35cd2bc9eb11d6))

## [0.15.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.14.0...v0.15.0) (2026-06-14)


### Features

* **graphics:** implement per-object materials and gloomy dungeon lighting ([e3708b8](https://github.com/Tugamer89/Tu-Maze/commit/e3708b886ff879f5ef94b9e0647c69f436129949))


### Bug Fixes

* **lights:** adjust light color values for better ambiance ([e1f82c8](https://github.com/Tugamer89/Tu-Maze/commit/e1f82c8adcc640aca5cd614947b47d104b10b6dc))
* update SARIF filter patterns to exclude additional directories ([5463cd6](https://github.com/Tugamer89/Tu-Maze/commit/5463cd6bfea523e26eb4de4f9e10ef50871a20be))


### Code Refactoring

* **main:** remove commented code for clarity ([c8a3919](https://github.com/Tugamer89/Tu-Maze/commit/c8a3919e9963d67e679bb605d2cb28f94a8f297a))


### Documentation

* **report:** add Stage 14 section for upcoming features and improvements ([d3fab65](https://github.com/Tugamer89/Tu-Maze/commit/d3fab65806e573888407f618ac284a96b80ad609))

## [0.14.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.13.0...v0.14.0) (2026-06-14)


### Features

* enhance build system with cross-compilation support for Windows ([fde10b9](https://github.com/Tugamer89/Tu-Maze/commit/fde10b92baa6dfbb2f54a3c537c89bf0b669457d))


### Bug Fixes

* ensure keyboard capture only when window is focused ([66fd319](https://github.com/Tugamer89/Tu-Maze/commit/66fd319bf1058bdf723cf1629b024a1ab7f62b2d)), closes [#15](https://github.com/Tugamer89/Tu-Maze/issues/15)
* initialize GPUMesh in constructor for better safety ([65ee23e](https://github.com/Tugamer89/Tu-Maze/commit/65ee23e8e3dce4ff43b5962b122a496b4153dc47))
* MSAA handling in GUI and Minimap ([5f20149](https://github.com/Tugamer89/Tu-Maze/commit/5f201499ecbd64e911a8929fd7f7cb82669b6a09)), closes [#14](https://github.com/Tugamer89/Tu-Maze/issues/14)


### Documentation

* remove cloning instructions from README.md for clarity ([1a2d918](https://github.com/Tugamer89/Tu-Maze/commit/1a2d91842678b61e74c47a09ae1cdc8e66020344))
* updated REPORT.md to stage 13 ([b30b74f](https://github.com/Tugamer89/Tu-Maze/commit/b30b74f74adcd58b16dd157bcd743b98bf38a442))

## [0.13.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.12.0...v0.13.0) (2026-06-13)


### Features

* add SonarLint configuration for connected mode project ([fbd2e2c](https://github.com/Tugamer89/Tu-Maze/commit/fbd2e2cb804cafc7820745b48fd3ef36241b96ea))
* optimize maze rendering by implementing static batching and enhancing mesh handling ([85525c7](https://github.com/Tugamer89/Tu-Maze/commit/85525c7ce1641682cd5c54f53aae9b85bd7df3cc))


### Code Refactoring

* improve memory management and safety in GPUMesh, Shaders and Maze classes ([49660f4](https://github.com/Tugamer89/Tu-Maze/commit/49660f4f58ddfd5a16a9c553c94169f58b92a615))
* remove unused GPUMesh pointers ([2902ff0](https://github.com/Tugamer89/Tu-Maze/commit/2902ff00fc37fae050addc498965addf79f8371b))

## [0.12.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.11.0...v0.12.0) (2026-06-11)


### Features

* implement asset loading system with task queue for improved efficiency ([d5f6c77](https://github.com/Tugamer89/Tu-Maze/commit/d5f6c772ab9e21601ec2884c2d71a20a8eedba2c))

## [0.11.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.10.0...v0.11.0) (2026-06-11)


### Features

* implement minimap feature with performance optimizations and UI controls ([eb07137](https://github.com/Tugamer89/Tu-Maze/commit/eb071374e155b01f0819f1e7e894afcc6aa5140b))
* renamed shaders and added stage 10 in report ([2f481ed](https://github.com/Tugamer89/Tu-Maze/commit/2f481ed4aaeda0681336f1d635a443ad72587ffc))


### Bug Fixes

* simplify floor mesh definition by reducing vertex and face counts ([84f11fc](https://github.com/Tugamer89/Tu-Maze/commit/84f11fc7bc5c2b9cc712b368c330c92a56f5f98e))
* update include paths for glad headers in multiple files ([9158a3a](https://github.com/Tugamer89/Tu-Maze/commit/9158a3a6b2668bf124fc154d7da5fbb62aa70065))

## [0.10.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.9.0...v0.10.0) (2026-06-11)


### Features

* add formatting step to Makefile for consistent code style ([5432025](https://github.com/Tugamer89/Tu-Maze/commit/5432025499ac1f4a6dca5748eee6bea189f2065f))
* enhance GUI settings with V-Sync, MSAA, and FOV adjustments; improve rendering performance and user experience ([3963ea8](https://github.com/Tugamer89/Tu-Maze/commit/3963ea885db7b4d458bc270b2394d02ea9672329))

## [0.9.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.8.0...v0.9.0) (2026-06-11)


### Features

* add texture quality settings and persistence, update GUI for performance adjustments ([c25621f](https://github.com/Tugamer89/Tu-Maze/commit/c25621f412f14e1c4fb70306cb52e53d62e255ad))
* implement frustum culling for improved rendering performance and update drawing logic ([00588c2](https://github.com/Tugamer89/Tu-Maze/commit/00588c23b2ae67fa1251d06f76229bf7bcabe123))

## [0.8.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.7.1...v0.8.0) (2026-06-11)


### Features

* add Makefile for simplified build process and update README with usage instructions ([d1cdad3](https://github.com/Tugamer89/Tu-Maze/commit/d1cdad37568fb73226f494340db9e347bd5c0603))
* add torch position slider to GUI and update shader on change ([028e0fe](https://github.com/Tugamer89/Tu-Maze/commit/028e0fe4e32623e68ef4040166b76a7994c37651))
* added antisotropic filter ([7e7e428](https://github.com/Tugamer89/Tu-Maze/commit/7e7e428db1fbfdd2e6a629ea1db2b71313632913))
* implement first-person camera movement and optimize rendering settings ([10ab94b](https://github.com/Tugamer89/Tu-Maze/commit/10ab94bf7a05200fd3aa8ac7d6cc51d437da01c2))
* update field of view to 60 degrees and clean up camera vector calculation ([a19e092](https://github.com/Tugamer89/Tu-Maze/commit/a19e092ba8f3a6c26d1c1e7c83c0684cdccbd118))
* update keyboard and mouse controls for stages 4 and 6 in README ([c26c89b](https://github.com/Tugamer89/Tu-Maze/commit/c26c89b735d0baf2a1702350f5eb08d764b7fc4a))
* update stage 7 details in README and REPORT ([b09230f](https://github.com/Tugamer89/Tu-Maze/commit/b09230fc94bb57bca155a7dc1aab84de7c8b1181))


### Bug Fixes

* ensure grid resizing uses size_t for width and height in Maze constructor ([d4a9ed2](https://github.com/Tugamer89/Tu-Maze/commit/d4a9ed20a63ca28d812b62f6843ef7edbf582750))

## [0.7.1](https://github.com/Tugamer89/Tu-Maze/compare/v0.7.0...v0.7.1) (2026-06-09)


### Performance Improvements

* optimize transformation calculations in Node class and update scene rendering ([1094e31](https://github.com/Tugamer89/Tu-Maze/commit/1094e318351f1cc26e4586f09f1dfafad13d50e0))

## [0.7.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.6.0...v0.7.0) (2026-06-08)


### Features

* add pre-commit hook for automatic code formatting with clang-format ([730aa80](https://github.com/Tugamer89/Tu-Maze/commit/730aa8058d3c3255c788cfc2272b633a842a0e69))
* add Stage 6 section to REPORT.md ([9ebaf19](https://github.com/Tugamer89/Tu-Maze/commit/9ebaf19e286f8efbd2a8eeda2bcfb524d8269803))
* enhance REPORT.md with detailed project descriptions and performance optimization insights ([f3c2820](https://github.com/Tugamer89/Tu-Maze/commit/f3c2820c36db22055df56533c5c9c77da8f2ec88))
* implement Box Mapping optimization for texture sampling in shaders ([f372409](https://github.com/Tugamer89/Tu-Maze/commit/f37240996af2470acf3dd18db6bbc1d469ba1e3b))


### Bug Fixes

* update GitHub Actions workflow for clang-format to use app token and correct user details ([cf958e2](https://github.com/Tugamer89/Tu-Maze/commit/cf958e205b90f0ac19d67dec5a107181d930a7fc))

## [0.6.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.5.0...v0.6.0) (2026-06-08)


### Features

* enhance loading screen and GUI with FPS counter and loading feedback ([9f1a019](https://github.com/Tugamer89/Tu-Maze/commit/9f1a019302d7500db6fda86a5e3d4f76fb8111f6))
* enhance loading screen rendering and buffer clearing in main loop ([ccb99a2](https://github.com/Tugamer89/Tu-Maze/commit/ccb99a22fb12718675bc4ff947d542fb623d7d53))
* enhance wall rendering and improve normal calculations in shader ([13b730e](https://github.com/Tugamer89/Tu-Maze/commit/13b730eeb4529449fbce8601960cc631f75af7fe))
* update stage5 screenshot to reflect recent changes ([00c5b2e](https://github.com/Tugamer89/Tu-Maze/commit/00c5b2e46c336f754707a560807b921fcdc615f2))


### Bug Fixes

* correct grammatical error in stage 5 rendering description ([70707dd](https://github.com/Tugamer89/Tu-Maze/commit/70707dd2442893366b4ecaaadddeacfb3e1a22b4))

## [0.5.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.4.0...v0.5.0) (2026-06-07)


### Features

* add high-quality textures for Stage 4 and update REPORT.md ([c3b2e27](https://github.com/Tugamer89/Tu-Maze/commit/c3b2e27123197fd7d4b5c570b3ea3c4a1d185592))
* implement triplanar mapping and material system for enhanced texture handling ([39dff50](https://github.com/Tugamer89/Tu-Maze/commit/39dff50777495987f747a737a68be23aaa08625c))


### Code Refactoring

* delete copy constructors for GPUMesh, Gui, and Shaders classes ([6d2daf8](https://github.com/Tugamer89/Tu-Maze/commit/6d2daf8bdb0386041e0d1dfe5509f3ba40d2b64c))
* remove Gouraud and Normal shaders, update controls in documentation ([9209990](https://github.com/Tugamer89/Tu-Maze/commit/920999045e5d4942d6eb65d64aea8fe25c15a0a7))
* reorder includes and improve code formatting in maze and texture headers, and main file ([b4f5331](https://github.com/Tugamer89/Tu-Maze/commit/b4f533114d868057ea7574c566211f060c342dc2))
* update shader UV mapping and improve maze wall dimensions ([5fa0c5e](https://github.com/Tugamer89/Tu-Maze/commit/5fa0c5eba4ee6b89d74c2d418573e20f408fa033))

## [0.4.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.3.0...v0.4.0) (2026-06-07)


### Features

* enhance maze generation with bounds checking and random seed initialization ([b0e29ef](https://github.com/Tugamer89/Tu-Maze/commit/b0e29ef786fc3dfd31145796b9bcb25b4272187e))


### Code Refactoring

* simplify maze generation logic using direction table ([b1c7403](https://github.com/Tugamer89/Tu-Maze/commit/b1c7403740f8c52375f11a9019bd3a810cf03f4d))


### Documentation

* add Stage 3 description and maze generation algorithm details to REPORT.md ([8a10843](https://github.com/Tugamer89/Tu-Maze/commit/8a10843237f06c9816f9c0a0c08fd8f22e22f6a0))
* add Stage 3 image to REPORT.md and update maze generation description ([f660d0d](https://github.com/Tugamer89/Tu-Maze/commit/f660d0da3b68c6391669a0f79d5a2d7deb06065a))

## [0.3.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.2.0...v0.3.0) (2026-06-07)


### Features

* center maze cells at the origin during scene population ([70c4b28](https://github.com/Tugamer89/Tu-Maze/commit/70c4b28ff3c9f5fb036617002885f1878780432f))
* implement maze generation and scene management with walls and floors ([cb041b0](https://github.com/Tugamer89/Tu-Maze/commit/cb041b080af5d0202b20e0ebf1ccc12c969e91f2))


### Bug Fixes

* update normal_fd value in Camera class for correct focal distance ([86f8182](https://github.com/Tugamer89/Tu-Maze/commit/86f8182844652c49d62a2a49ba85b268bb17ada0))


### Code Refactoring

* remove unused field variables fov and aspect_ratio from Camera class ([8921368](https://github.com/Tugamer89/Tu-Maze/commit/892136828c50b48b1111fba0fdacf762f6c22875))
* simplify wall addition logic in Maze class ([a0ff7de](https://github.com/Tugamer89/Tu-Maze/commit/a0ff7debd63c6f99806c5ee438001378491c53e1))

## [0.2.0](https://github.com/Tugamer89/Tu-Maze/compare/v0.1.0...v0.2.0) (2026-06-07)


### Features

* add core rendering components including camera, lighting, and mesh handling ([9672499](https://github.com/Tugamer89/Tu-Maze/commit/9672499a2a1a65df53611c4a3a235b46bae7b7e8))


### Code Refactoring

* replace specific function types with templates in getInfoLog for improved flexibility ([722be31](https://github.com/Tugamer89/Tu-Maze/commit/722be310258385bac6d06d16d55e014878575443))
* update camera and mesh class members, improve resource management in setup ([5672d57](https://github.com/Tugamer89/Tu-Maze/commit/5672d5766555346926102d3ef733c3ddf2d12c94))
