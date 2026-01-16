# Overture

Overture is a game engine written in c that exists I guess...

This is the third time I try to make this engine, but unlike previous atempts I now finally know what I'm doing... kinda. Overture is not trying to be the best game engine out there, and in fact will have many stupid features because I learned something cool and wanted to use it in a stupid way. I took inspiration from the gldsrc and source engines although that isn't really apparent, except when looking at the jank, then their identical. I'm trying to make this old school ish but also new ish... simply said its a mess. I'm just doing this for fun.

## Features & Guidelines

The following is a list of features and guidelines I will try to implement or follow during the development of this engine.

### Fun to make

This isn't the best engine nor is it trying to be, all this is, is a fun project for me to loose my mind on.

### Entity Component System

This works using an ECS... duh. But also, the core engine features will aslo work using it, so windows, rendering, etc, will all use the ecs as a core.

### Initialize entities from a "map" file

All entities will be able to be initialized from a map file. Some sort of general serialize and desirialize functionality will be implemented for the components, and using a map editor entities can be spawned in with all the fun components.

### Simple to use

Most of the design of the engine will relly on trying to make it as easy to use as possible, and many many hacks or overcomplicated designs will be implemented for this reason. This will mainly manifest in macro abuse, compiler abuse and many other fun ways to torture C purists.

### Some sort of Clustered Renderer

Using a clustered renderer a lot of lights can be renderer efficiently. Now i have two options for the rest of the engine architecture:

1. Forward Rendering
2. Deffered rendering

Forward Rendering would allow easy custom shader support while giving up a bit of performance that a deffered renderer can provide. Also MSAA...

Deffered is faster, less versatile, cause you're pretty much stuck with one renderering pipeline though it is possible to implement another pass for custom shaders. Also no MSAA...

Most likely I'll do a clustured forward renderer with IOT and a z prepass for even less lighting calculations. Most likely during the main forward pass a normal buffer will be created for SSAO and other post effects.

## Dependencies

These need to be installed and compiled separatly for now.

- GLFW3
