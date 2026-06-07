# Changelog

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
