# MiniEngine

A custom 3D game engine and dungeon crawler built in C++17 and OpenGL 3.3 Core Profile.

---

## Rendering Features

**Forward rendering pipeline — 4 passes per frame**
1. Shadow depth pass — directional light, orthographic projection
2. Scene to offscreen buffer at 320×240 (PS1 resolution)
3. Nearest-neighbor upscale to 1280×720
4. Dear ImGui debug overlay

**Lighting and shading**
- Blinn-Phong with normal mapping (TBN matrix in vertex shader)
- PCF shadow mapping — 3×3 kernel, single directional light
- 128 point lights with cubic attenuation
- Exponential fog

**PS1 aesthetic**
- Fixed offscreen resolution rendered to FBO, nearest-neighbor upscaled — two numbers in `OffscreenBuffer::Init` control the look, no other changes needed
- Vertex snapping via `u_SnapStrength` uniform — geometry wobble in clip space
- Skeletal animation with no interpolation — snaps to nearest keyframe, looks like actual PS1 animation

**Particles**
- `ParticleEmitter` — batched `GL_POINTS`, one draw call per emitter
- Per-emitter configurable: color gradient, velocity range, lifetime, gravity, size
- Distance culling beyond 200 units
- 64 torch emitters in dungeon

---

## Dungeon Generation

BSP room placement feeding into A\* corridor carving.

- Configurable floor size, room size range, node padding, target room count
- Room types assigned post-generation: Start, Combat, Treasure, Boss
- A\* corridor carver prefers existing open space (wall traversal cost 100) — produces organic T-junction corridors rather than grid-aligned hallways
- Separate wall and floor meshes built from the grid — different materials, no seams between rooms and corridors
- AABB collision against the grid with per-axis wall sliding

---

## Architecture

```
Rendering:   Shader, Mesh, SkinnedMesh, DynamicMesh, Material, Model, SkinnedModel
             ShadowMap, OffscreenBuffer, ParticleEmitter, RenderContext
Animation:   Skeleton, AnimationClip, Animator, BoneChannel
Dungeon:     DungeonGenerator, DungeonGrid, DungeonMeshBuilder, AStar
Core:        MiniEngineApp, SceneManager, AssetManager, InputManager
World:       Entity → Actor → PlayerActor / EnemyActor
Scene:       IScene → Scene → DungeonScene
                    → Scene2D → MenuScene → MainMenu
```

Scene transitions work via `GetNextScene()` — `SceneManager` checks this each frame and hot-swaps. Scenes are fully self-contained: `DungeonScene` generates the dungeon, builds the mesh, spawns entities, and places lights entirely within its own `Init`. `MiniEngineApp` only handles window, OpenGL context, and the render passes.

Components are stored as `unordered_map<type_index, unique_ptr<Component>>` on `Entity`. Assets are cached by path in `AssetManager` as `shared_ptr` — one load, many users. Every class owning a GPU handle has a move constructor and deleted copy constructor.

---

## Skeletal Animation

Full pipeline from Assimp bone extraction to per-frame GPU matrix upload.

- Bone hierarchy traversal with `GlobalInverseTransform` applied at root
- `OriginalId` per bone ensures vertex `BoneIds` match `FinalMatrices` indices regardless of Assimp node ordering
- Root motion stripped on Hips bone
- `ResizeBoneMatrices(256)` — fixed-size upload to avoid per-frame reallocation
- PS1 snap: `BoneChannel::GetLocalTransform` picks nearest keyframe, no LERP/SLERP

---

## Enemy AI

- A\* patrol between random rooms, re-paths every 0.5 seconds
- Switches to chase within 40 units of the player
- Faces movement direction each frame
- `IsSkinnedActor` flag on `Entity` prevents bind-pose shadow artifact in the depth pass

---

## Build

**Requirements:** Windows, Visual Studio 2022, CMake 3.16+

All dependencies are vendored in `external/`:
- GLFW, GLAD, GLM, Assimp, Dear ImGui, stb_image

```bash
cmake -S . -B out/build -G "Visual Studio 17 2022"
cmake --build out/build --config Debug
```

Assets are copied to the build directory automatically via a CMake post-build step.

---

## Third-Party Assets

| Asset | Creator | Source |
|---|---|---|
| PS1 Dark Fantasy Horror Game Assets — nature collection, palace props, skeleton character, skull, sword | Stark Crafts | [itch.io](https://starkcrafts.itch.io/ps1-dark-fantasy-horror-game-assets-by-stark-crafts) · [YouTube](https://www.youtube.com/c/starkcraftsyt) |
| Survival Guitar Backpack (low poly) | Berk Gedik, modified by Joey de Vries | [Sketchfab](https://sketchfab.com/3d-models/survival-guitar-backpack-low-poly-799f8c4511f84fab8c3f12887f7e6b36) |

If i have made an attribution error or missed a credit, please reach out and i will correct it promptly.

---

## Stack

| | |
|---|---|
| Language | C++17 |
| Renderer | OpenGL 3.3 Core Profile |
| Window / Input | GLFW |
| Math | GLM |
| Asset loading | Assimp (FBX, OBJ, GLTF) |
| UI | Dear ImGui |
| Image loading | stb_image |
| Platform | Windows (MSVC) |

---

## Planned

- Timeline system — T1/T2/T3 are three drafts of the same dungeon space; gates, traps, and torches carry per-timeline state; visual feedback on switch via fog color and torch color
- Dungeon decorator — prop placement by room type
- Combat — player attack hitbox, damage, death
- CollisionWorld — AABB vs AABB for props and enemies
- Idle/walk animation blending on enemy
- SSAO
- Sound (miniaudio or OpenAL)
