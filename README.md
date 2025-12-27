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

### Specialized Clustered Deffered PBR Renderer

Using a clustered renderer a tonne of lights can be rendered efficiently. Using a custom specialized deffered renderer can provide fast, quality visuals with the downside of loosing out on versitility. Although some sort of custom shader effects, materials, or object rendering support will be implemented. The renderer will follow the following outline: (as long as i don't change my mind again)

Cluster Compute -> G Buffer Pass (with possible custom shader tag and support) -> Deffered shading -> Indirect lighting (AO, GI, reflections) -> Post processing -> UI

More stages might be added or some might change but this is a general idea of the rendering process. Some other things that will get implemented is some sort of transparency support (most likely OIT), instanced rendering, etc.

## Dependencies

These need to be installed and compiled separatly for now.

- GLFW3
- Vulkan
