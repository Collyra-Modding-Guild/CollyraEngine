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
Once the engine starts, the house fbx will be loaded into the scene automatically. As you can see, at the top of the window there is a main bar, this hold a button to exit the Engine (inside the App tab); Editor is still in development, Game Object opens a menu to create empty GameObjects & various primitives into the scene, the Windows Tab allows you to enable & disable the engine's windows (here you will find the About window, disabled by default, that contains information about the project & a link to the GitHub repository) & finally Preferences Tab lets you choose your favorite UI style between three options.

### Hierarchy
At the left of the engine's window, you will find the hierarchy. This represents the GameObject structure of the window, you can click in one object so its information may show in the inspector window detaileeed below. Once a GameObject is selected, you can also delete it (& its childs) pressing SHIFT or you can drag a Texture so it is loaded into it. The node structure can be unfolded/folded only by pressing each node's little arrow at the left of its name; if a node has no childs, no arrow will be displayed.

### Inspector
Once a GameObject is selected in the hierarchy, its information will be displayed in the inspector. Here, you will see its general information & change it: name, if its active or not (little check box at the left of the name), if its static (this is still in development) & change its layer & tag (for the moment, you can not add more, but it internally changes). Please note that, to change the name you must press ENTER.

Here, you will also see the GameObject's components; for the moment, there are only three: transform (that indicates your position, rotation & scale respect your parent; this is a component that can not be disabled & its on every GameObject created), Mesh (if the GameObject has a Mesh, it will display its information & different local enablers to display: such as wire mode, vertex normals, etc.) & Material (if the GameObject has a texture it will display it, alongside it's material's color). Please take note that, such as the GameObject, individual components can be enabled/disabled, using the check box at its left (except for transform) & that, although there are fields that can be dragged to change them, this function has not been implemented yet & they will not change.

### Configuration
Alongside the inspector window, there is the configuration window. Here you will find how to change the configuration for the most important of the engine's components (such as renderer, window, input, etc.) and/or check important information (such as fps, enable/disable vsync, software versions, etc.).

### Console
Finally, in the console you will see the system messages displayed as the code runs. You can also clear, search for certain logs, enable/disable auto-scroll, etc. You can see all the controls detailed there!

## Controls

### General 
- Left click to interactuate will all the buttons, input boxes, drag inputs, etc.
- Drag & Drop of Meshes (supported fbx) will load the fbx hierarchy into the scene with its respective GameObjects & components
- Drag & Drop of Textures (supported Png & Dds) will load a texture into the selected fbx, creating a Material Component if necessary.

### Camera
- Right Click + WASD to move for a fps-like movement
- Middle mouse click + Mouse movement to pan in the plane of the Camera
- Mouse wheel should zoom in and out, if an object is focused it doesn't change the rotation pivot
- Alt+Left click orbits arrounds a point in space, if an object is selected, rotates arround that object
- F focus the camera to the selected GameObject, once it's selected Alt+Left Click to rotate arround it; any other movement that it's not that one or zoom will "break" the focus.
- Holding SHIFT duplicates movement speed for every movement (except zoom)

### Hierarchy
- Left click an item to focus it
- Supr will delete the focus element & all its childs
- Left click on the arrow to fold/unfold the node's structure.

NOTE: for now, take into account that you can only load files from inside the project folder &  taht, if you load an fbx with an associated texture, it will search for a relative path & then try to see if the texture is in the texture's folder.


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
