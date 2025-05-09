# Voxen Engine

**Voxen** is a performant, voxel-based game engine with fast ray tracing and scene management. Voxen features an integrated editor, C# scripting, and a custom voxel-based rendering pipeline. Voxen was inspired by [The Hazel Engine](https://hazelengine.com) and [Unity](https://unity.com).

---

## Features

### Voxels

* **Sparse Voxel Tree (SVT)**: Voxen uses sparse voxel 64-trees as its internal data structure for voxel data, with an architecture based on this article [A guide to fast voxel ray tracing using sparse 64-trees](https://dubiousconst282.github.io/2024/10/03/voxel-ray-tracing/).
* **Memory Allocators**: The VoxMemoryAllocator manages all the SVTs in a scene, and prepares the buffers for their data.
* **Vox File Parsing**: Voxen currently only accepts models of the `.vox` file type from [MagicaVoxel](https://ephtracy.github.io).

### Rendering & Lighting

* **VoxRenderer**: Renderer that handles data transfer to and from the GPU.
* **Compute Shader Ray Tracing**: Uses an [OpenGL 4.6](https://www.khronos.org/opengl/) compute shader for ray tracing that traverses the SVT structures.
* **Deferred Lighting**: Uses a G-Buffer with multiple render targets for defferred lighting and post-processing.
* **Global Illumination**: Full‑scene indirect lighting.
* **Screen‑Space Ambient Occlusion**: SSAO with adjustable blur and strength.
* **Profiling Tools**: VOX_PROFILER profiles all of the functions ran during Voxen's lifetime. The editor also has statistics for the current scene and shaders.

### Editor

* **Content Browser**: Drag‑and‑drop asset management with file icons.
* **Viewport**: Mouse picking, transform handles, and an editor camera with Maya based controls.
* **Properties Panel**: Inspect and edit components.
* **ImGui Docking & Gizmos**: Dockable windows using ImGui.

### Entity Component System & Scripting

* **Entity Component System (ECS)**: ECS with useful prebuilt components such as Camera, PointLight, VoxRenderer, etc.
* **C# Scripting**: Native/C# interop, hot‑reload on build, and field serialization.

---

## Getting Started

### Prerequisites

* **Windows** (officially supported)
* **Visual Studio 2022** (C++ IDE)
* **Git**
* **Premake5** (included in `Scripts/`)

> *Note: Linux and macOS support are in development.*

### Clone & Initialize

```bash
# Standard clone and submodule init
git clone https://github.com/loganr3093/Voxen.git
cd Voxen
git submodule update --init --recursive

# OR clone with submodules:
git clone --recurse-submodules https://github.com/loganr3093/Voxen.git
cd Voxen
```

### Generate Project Files

1. Open a Command Prompt.
2. Navigate to the `Scripts/` folder:

   ```bash
   cd Scripts
   ```
3. Run the setup script:

   ```powershell
   ./Setup-Windows.bat
   ```

   This generates `Voxen.sln` and project files.

### Build & Run

1. Open `Voxen.sln` in Visual Studio.
2. Select **Voxen-Editor** as the startup project.
3. Build (Ctrl+Shift+B) and run (F5).

### Opening the Demo Project

1. Upon running Voxen, you will be asked to open a .vproj file, a Demo can be found under `Voxen/Voxen-Editor/DemoProject`. Select the `Demo.vproj` to open the default scene for the demo project.
2. From here, you can explore the Voxen Editor, and play with the project's scripts.
3. If you get warnings or errors about the scripting engine, you may need to separately build the `Voxen-ScriptCore` project.

---

## Contribution Guidelines

We welcome community contributions! If you want to be involved, please follow these steps:

1. **Fork** the repository and create your feature branch:

   ```bash
   git checkout -b feature/YourFeatureName
   ```
2. **Implement** your changes.
3. **Add** your documentation.
4. **Push** to your fork:

   ```bash
   git push origin feature/YourFeatureName
   ```
5. **Open** a Pull Request with `main`.

Please follow the existing coding style.

---

## License

Voxen is released under the **Apache License 2.0**. See [LICENSE](LICENSE) for details.

---

# Contributors

* **Logan Rivera** – Project Lead

---
