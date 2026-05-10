## Overview
**Rootworld** is a Terraria-like sandbox game built on top of a custom **ECS** implementation. Made using C++ and the SFML Graphics library

<img width="1919" height="986" alt="Screenshot 2026-05-10 174601" src="https://github.com/user-attachments/assets/7293e615-6e21-4b74-9c39-b52d3b37a51b" />
<img width="1919" height="987" alt="Screenshot 2026-05-10 174713" src="https://github.com/user-attachments/assets/615882d1-f485-4f90-8f99-eb55893bb5d7" />
<img width="1919" height="989" alt="Screenshot 2026-05-10 174901" src="https://github.com/user-attachments/assets/d0ee634d-71c4-4bbd-a38f-f4a55bced7cd" />
<img width="1913" height="986" alt="Screenshot 2026-05-10 180122" src="https://github.com/user-attachments/assets/d5cde200-3f09-4c9d-802e-228820cb6cb8" />

## Steps To Build
1. Install CMake
2. Run the following commands in PowerShell/terminal in the folder where CMakeLists.txt is located
```
mkdir build
cd build
cmake ../
cmake --build .
```

## Features

- Entity component system architecture for robust entity management.
- Sprite rendering and player movement
- Animation support
- Infinite world generation using a chunk system
- Terrain generation using 1D perlin noise
- Cave generation using 2D perlin noise and drunkard's walk
- Tree generation with randomized shapes and sizes
- Biome generation (Forest, Desert, Tundra)
- Player collisions with the tiled environment
- Ability to place and destroy blocks
- Smart Zombie AI using A* pathfinding
- Flying enemies (Blood Bats)
- Combat System with hit feedback and physics-based knockback
- Health System
- Lighting System using BFS Blood-fill algorithm.
- Day and Night system
- Inventory + Hotbar system for management of items. (50 item capacity)
- Audio system with volume control
- Settings window
  
## Controls

- W to Jump, A and D to move left and right
- Right-click to place blocks, left-click to destroy blocks/attack with sword
- E to Open/Close Inventory
- Number keys to choose items in the hotbar

## Libraries used
- SFML for Graphics and Audio
- ImGui for UI rendering


