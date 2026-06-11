# Changelog

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
