# Changelog

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
