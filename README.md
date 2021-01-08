# Collyra Engine
Collyra Engine is an open source student Game Engine made for the Game Engines class of the Citm's Bachelor's degree in Video Game Design and Development, made by Junior Students.

> You can check our web page here: https://collyra-modding-guild.github.io/CollyraEngine/

> You can check our repository here: https://github.com/Collyra-Modding-Guild/CollyraEngine

## Authors
- Aarón Guerrero Cruz
> Github profile: https://github.com/AaronGCProg

- Àlex Melenchón Maza
> Github profile: https://github.com/AlexMelenchon

## How to use
### General
Once the engine starts, the street fbx will be loaded into the scene automatically. As you can see, at the top of the window there is a main bar, this hold a button to exit the Engine (inside the App tab); Editor is still in development, Game Object opens a menu to create empty GameObjects, cameras, childs & various primitives into the scene, the Windows Tab allows you to enable & disable the engine's windows (here you will find the About window, disabled by default, that contains information about the project & a link to the GitHub repository) & finally Preferences Tab lets you choose your favorite UI style between three options.

### Hierarchy
At the left of the engine's window, you will find the hierarchy. This represents the GameObject structure of the window, you can click in one object so its information may show in the inspector window detaileeed below. Once a GameObject is selected, you can also delete it (& its childs) pressing SUPR. All the resources you drag will be loaded in this Game Object (Materials, Meshes & Textures). The node structure can be unfolded/folded only by pressing each node's little arrow at the left of its name; if a node has no childs, no arrow will be displayed. You can also left click and drag with the mouse to swap the hierarchy's nodes relationship.

### Inspector
Once a GameObject is selected in the hierarchy, its information will be displayed in the inspector. Here, you will see its general information & change it: name, if its active or not (little check box at the left of the name), if its static (this is still in development) & change its layer & tag (for the moment, you can not add more, but it internally changes). Please note that, to change the name you must press ENTER.

Here, you will also see the GameObject's components; for the moment, there are only four: transform (that indicates your position, rotation & scale respect your parent; this is a component that can not be disabled & its on every GameObject created), Mesh (if the GameObject has a Mesh, it will display its information & different local enablers to display: such as wire mode, vertex normals, etc.) , Material (if the GameObject has a texture it will display it, alongside it's material's color) & Camera (with the frustum & culling settings). Please take note that, such as the GameObject, individual components can be enabled/disabled, using the check box at its left (except for transform) & that, although there are fields that can be dragged to change them, this function has not been implemented yet & they will not change.

### Configuration
Alongside the inspector window, there is the configuration window. Here you will find how to change the configuration for the most important of the engine's components (such as renderer, window, input, scene camera, etc.) and/or check important information (such as fps, enable/disable vsync, software versions, outline, bounding boxes, scene camera culling, etc.).

### Console
In the console you will see the system messages displayed as the code runs. You can also clear, search for certain logs, enable/disable auto-scroll, etc. You can see all the controls detailed there!

### Resource counting
It will show all the resources that are loaded and how many instances of that resources are on the scene.

### File Explorer 
It shows all the resources that are ready to load. You can double click or drag into the scene window to load them. WARNING: If you try to load a material, texture or a mesh you will have to select a Game Object first. You can aslo middle click to delete an object.

### Play Bar
Finally, in the play bar you will see the buttons that manage the game state time. Play will save the current scene and enter into the game state, Stop will recover the scene before the game state, Pause will stop the game into the game state, Resume will run if the game was paused, and Advance will do just 1 loop to test and debug with precision. Time multiplier will accelerate or desaccelerate the game state time with debug purposes.
There is also a selection for the Play Camera: this needs to be selected & will affect which camera is used when play is pressed.

### Gameplay System
In the inspector, as you add a component (for the time being, only Script Components can be added), you will be able to select the scripts loaded into the scene or to create a new one.
If you choose to create a new one, a pop-up will show up indicating that you pass it a number for it to become the name of the C++ class you will generate; then, and this is a KEY STEP is to go to your VS project & include the files manually from there, we apologize for the inconvenience, we are working in the issue.
Once the script is added, it will execute once the function Start() & will continously call Update() each frame if the scene is currently playing; if you want to know more about which function can you actually call, please check the CollObject parent class associated with each object.
You can assume that you can do any modifications to the .cpp & .h file that will be generated, just make sure that the function CreateX (being X the name of your class is untouched, unless you want to change the name).
Furthermore, as you can see, this Gameplay system is based on C++ & supports Hot-Reloading; and there are two ways you can perform it:
- In the Main Menu Bar, inside the App item, there is a button you can press to Hot-Reload
- The Engine will also automatically refresh if it detects any change to the .h or .cpp documents that are currently loadded in to the engine; but this option is disabled by default, if you wish to enable it, check the configuration window, under the Gameplay Systems menu.


## Controls
### General 
- Left click to interactuate will all the buttons, input boxes, drag inputs, etc.
- Drag & Drop of Assets (supported fbx for models; dds, jpg and png for textures) from anywhere on the disk and shows them into the Assets window.
- Drag & Drop of Imported Files (our double click) to load them into the scene window and middle click to delete an imported file.
- Gizmos Modes (once a Game Object focused): keyboard key W (Translate), E (Rotate) & R (Scale).

### Camera
- Right Click + WASD to move for a fps-like movement
- Middle mouse click + Mouse movement to pan in the plane of the Camera
- Mouse wheel should zoom in and out, if an object is focused it doesn't change the rotation pivot
- Alt+Left click orbits arrounds a point in space, if an object is selected, rotates arround that object
- F focus the camera to the selected GameObject, once it's selected Alt+Left Click to rotate arround it; any other movement that it's not that one or zoom will "break" the focus.
- Holding SHIFT duplicates movement speed for every movement (except zoom)

All the inputs that involve a mouse click, must be in the scene window, otherwise they won't work.

### Hierarchy
- Left click an item to focus it
- SUPR will delete the focus element & all its childs
- Left click on the arrow to fold/unfold the node's structure.
- Left click + Drag to re-parent an object.

### File Explorer
- Double click or drag into the scene window to load them.
- Middle click to delete an object.

## Extra
### Additional Functionalities
- Mesh outline when selected.
- Gizmo functionalities.
- Asset import by drag and drop from anywhere on the disk.
- Supports hot reloaded of textures, also reimports if you modify a file, delete it by accident or drag a new asset in runtime.

### Additional Comments
#### A2
- The Resource counting window only shows you the resources that are currently loaded in the scene.
- The Assets window could take a couple of seconds to refresh.
- You can not delete loaded resources in the Asset window because they will be reimported again, if you desire to delete a resources, please delete the Assets associated to it.
- Currently, if you delete an imported FBX, it will delete only the "model". It will leave as is the meshes, textures & materials imported from that model.
- WARNING: Time multiplier affects Scene Camera movement, but it's just for you to see it is well implemented. It will only work at Game State in a future. You can test also Play and Stop moving objects in Game State, and pressing Stop to recover its original position.

#### A3
- The Gameplay System assumes that you have VS installed & the Desktop Development with C/C++ extension, if you don't have this, please install them for the engine to execute succesfully.
- WARNING: The engine uses MSBuild to compile the Gameplay System & needs you absolute path to VsVarshall.bat to execute, if your path is in a different folder than the one that comes by default, please go to Config/config.collConfig & change the path so it fits your own.
- This applies also for the temprary folder.

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
