# ZXGUI

A lightweight GUI framework for your ZX Spectrum program.
It allows you to have simple scenes with buttons, labels, edits, images and selects. It uses [YAML](./example/src/welcome.yaml) files to specify objects on scene.

https://user-images.githubusercontent.com/1666014/212490226-d91f1fb4-af73-4726-9777-e61b3156174a.mov

See [gui_example.tap](https://github.com/speccytools/zxgui/files/10418537/gui_example.tap.zip) for the recording above

## Dependencies

- z88dk
- `pip3 install pyYAML Pillow`

## How to add it to your project

1. [Migrate to CMake](https://github.com/z88dk/z88dk/wiki/CMake) if you haven't already
2. Clone this repository as a submodule to your project in a folder e.g. `gui`
3. Copy/modify `example_tiles.png` into e.g. `data/gui.png`
4. Set up your scenes (*.yaml files) in your source folder (e.g. `src`)
5. Make sure to call `zxgui_generate_scenes` / `zxgui_generate_tiles` in your CMakeLists.txt
6. See [CMakeLists.txt example](./example/CMakeLists.txt)
7. ```c
   #include "zxgui.h"
   ...
   #include "scene-s.inc.h"
   int main() {
       zxgui_clear();    
       zxgui_scene_set(&welcome);

       while (1)
           zxgui_scene_iteration();
   }
   ```
   
## GUI YAML SCHEMA

You can validate you have a correct yaml schema against [gui_schema.json](./gui_schema.json)

## How does it work

It works by processing *.yaml objects with scene objects, and generating
appropriate *.inc.h objects, which contain all object initialization necessary.

No code is actually executed, as initialization is completely static.

## Supported elements:

- `label`
- `edit`
- `multiline_edit`
- `button`
- `select`
- `image`
