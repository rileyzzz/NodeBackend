# NodeBackend
A WIP node editor using standard libraries.
Written in C++ and designed to be quickly modifiable, so it can be easily interfaced with and used as a backend underneath a renderer.
Comes with basic nodes and data types, but can be easily expanded to support any data type.
A "weird" data type is included as a simpler way to interface between C++ functions and the node graph. Casting for this data type is unsafe.
Exporting graphs is currently unfinished, but is intended to determine whether a graph is "static" or "dynamic" on export, so its output values don't need to be recalculated constantly if they're unchanging.

SDLTests is a sample implementation of the system for testing purposes, but isn't recommended for use and might not compile well.
Note that this is very old code and is likely to have a few memory leaks.

![image](https://www.dropbox.com/s/kn0szbe2v6xaqmf/nodeeditor.PNG?raw=1)
