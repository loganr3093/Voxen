# Voxen Game Engine

Voxen is a cutting-edge game engine designed for flexibility and performance. While fully supported on Windows, Voxen can also run on most other platforms, making it a versatile choice for game developers.

## Features

- **Modular Architecture**: Customize and extend the engine with ease.
- **Cross-Platform Support**: Runs on various platforms with full support for Windows.
- **Advanced Rendering**: Utilizes modern rendering techniques for high-quality graphics.
- **Scene Management**: Efficiently manage game objects and properties.
- **Integrated Editor**: User-friendly interface for creating and managing assets and scenes.

## Getting Started

Follow these instructions to get a copy of Voxen up and running on your local machine.

### Prerequisites

- Visual Studio 2022 or similar [*Untested on earlier versions of Visual studio*]
- Git

### Installation

1. **Clone the Repository**

   Open a terminal and run the following command to clone the Voxen repository:

   ```sh
   git clone https://github.com/loganr3093/Voxen
   ```
2. **Initialize and Update Submodules**

   Navigate into the cloned directory and initialize the submodules:

   ```sh
   cd Voxen
   ```
   ```sh
   git submodule init
   ```
   ```sh
   git submodule update
   ```
3. **Run Premake5**

   Navigate to the GenerateProjects.bat file in the root directory and run it.
4. **Open in Visual Studio**

   Open the **Voxen** directory in Visual Studio. Ensure that you have all necessary components installed for C++ development.
5. **Set Startup Project**

   In Visual Studio, set the **Sandbox** project as the startup project if it isnt by default. This can be done by right-clicking on the **Sandbox** project in the Solution Explorer and selecting 'Set as Startup Project'.
6. **Build and Run**

   Build the solution and run the **Sandbox** project. This will launch the engine.

### Contributing
We welcome contributions from the community! Please fork the repository and create a pull request for any enhancements, bug fixes, or new features.
1. **Fork the Repository**
2. **Create a Feature Branch**
     ```sh
     git checkout -b feature/your-feature-name
     ```
3. **Commit Your Changes**
     ```sh
     git add "path/to/new-files
     ```
     ```sh
     git commit -m 'Add some feature'
     ```
4. **Create a Feature Branch**
     ```sh
     git push origin feature/your-feature-name
     ```
5. **Open a Pull Request**

### License
**Voxen** is released under the **Apache-2.0 License**. See **LICENSE** for more information.

### Contributors
- **Logan Rivera**: Project Lead
Your name could go here!
