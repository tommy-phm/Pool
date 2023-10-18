# Pool Simulator.

Pool Simulator is a 2D simulation of a pool game, implemented in C++. Players can interacte by hiting around the cue ball with control over direction and force.

![Alt Text](resource/img.PNG)

## Table of Contents

- [Project Name](#project-name)
  - [Table of Contents](#table-of-contents)
  - [Description](#description)
  - [Prerequisites](#prerequisites)
    - [Compiling the Program](#compiling-the-program)
      - [Using Makefile](#using-Makefile)
      - [Using Visual Studio Code](#using-visual-studio-code)
      - [Using the Precompiled Version](#using-the-precompiled-version)
  - [Usage](#usage)
  - [References](#references)

## Prerequisites
- [OpenGL](https://www.opengl.org/)
- [GLFW](https://www.glfw.org/download.html)
- [glm](https://sourceforge.net/projects/glm.mirror/)
- [glad](https://glad.dav1d.de/)

## Compiling the Program

You have two options for compiling the program: using Visual Studio Code or the provided Makefile. 
For linux, you will need to install libglfw and libglfw-dev package yourself.

### Using the Makefile

1. Open your terminal or command prompt.
2. Navigate to the project directory.
3. Run the `make` command to compile the program. 

### Using Visual Studio Code

1. Open the folder in Visual Studio Code.
2. Run the build task to compile the code. Shortcut is Crtl-Shift-B.
3. Select your operating system

### Using the Precompiled Version

If you'd rather use a precompiled version of the program, there is billard_simulator for Linux or billard_simulator.exe for Windows.

## Usage
To interact with the cue ball:

1. Click and drag on the cue ball.
2. A directional line will appear to adjust direction and force.
3. Release to hit the ball. Note that the cue ball can only be hit when all other balls are stationary.


## References

- [HTML5 & JavaScript Game Development Tutorial](https://spicyyoghurt.com/tutorials/html5-javascript-game-development/collision-detection-physics): Circle collision tutorial
- [Black Octagon](https://globalsymbols.com/symbolsets/openmoji/symbols/44281?locale=en): Image use for the icon
