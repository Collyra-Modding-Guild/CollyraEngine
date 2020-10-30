# Collyra Engine
Collyra Engine is an open source student Game Engine made for the Game Engines class of the Citm's Bachelor's degree in Video Game Design and Development, made by Junior Students.
> You can check our repository here: https://github.com/Collyra-Modding-Guild/CollyraEngine

## Authors
- Aarón Guerrero Cruz
> Github profile: https://github.com/AaronGCProg

- Àlex Melenchón Maza
> Github profile: https://github.com/AlexMelenchon

## How to use
### General
Once the engine starts, the house fbx will be loaded into the scene automatically. As you can see, at the top of the window there is a main bar, this hold a button to exit the Engine (inside the App tab); Editor is still in development, Game Object opens a menu to create empty GameObjects & various primitives into the scene & finally, the Windows Tab allows you to enable & disable the engine's windows (here you will find the About window, disabled by default, that contains information about the project & a link to the GitHub repository).

### Hierarchy
At the left of the engine's window, you will find the hierarchy. This represents the GameObject structure of the window, you can click in one object so its information may show in the inspector window detaileeed below. Once a GameObject is selected, you can also delete it (& its childs) pressing SHIFT or you can drag a Texture so it is loaded into it. The node structure can be unfolded/folded only by pressing each node's little arrow at the left of its name; if a node has no childs, no arrow will be displayed.

### Inspector
Once a GameObject is selected in the hierarchy, its information will be displayed in the inspector. Here, you will see its general information & change it: name, if its active or not (little check box at the left of the name), if its static (this is still in development) & change its layer & tag (for the moment, you can not add more, but it internally changes). Please note that, to change the name you must press ENTER.
Here, you will also see the GameObject's components; for the moment, there are only three: transform (that indicates your position, rotation & scale respect your parent; this is a component that can not be disabled & its on every GameObject created), Mesh (if the GameObject has a Mesh, it will display its information & different local enablers to display: such as wire mode, vertex normals, etc.) & Material (if the GameObject has a texture it will display it, alongside it's material's color). Please take note that, such as the GameObject, individual components can be enabled/disabled, using the check box at its left (except for transform) & that, although there are fields that can be dragged to change them, this function has not been implemented yet & they will not change.

## License:
MIT License

Copyright (c) [2020] [Aarón Guerrero Cruz / Àlex Melenchón Maza]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
