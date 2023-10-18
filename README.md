# Cortex
A Vulkan based 3D renderer and Game Engine, currently being built for MacOS.

### 8/8/2023

Currently I'm working on the renderer, which has been a massive learning experience. 
I've managed to get models loaded from .obj, painted with a texture, and displayed to the screen. 
The next step is to refine the API somewhat and introduce a basic material system, to get ready for the introduction of lighting. 
Once we have multiple objects rendered, each with their own materials/material instances, 
we can begin looking at other crucial features such as font rendering (oof) and the integration of some sort of IMGUI solution. 

Aside from the renderer, there are some other basic features such as a simple and lightweight event system, logging, and the basic shell of a 'scene'.
All of these will need to be revisited eventually, especially the event system which is currently blocking, but thats a job for future Sam.

https://github.com/SamHaskell/Cortex/assets/85114745/652e218f-56a0-4c41-b631-1fb2c37bd965
