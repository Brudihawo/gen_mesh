# Mesh creator in C using SDL2

## Quickstart
```commandline
  $ make
  $ ./build/gen_mesh
```

## Controls
| Key | Action                 |
|-----|------------------------|
|  D  | toggle QTree drawing   |
|  G  | regenerate QTree       |
|  Q  | kill application       |
|  U  | undo last insertion    |
|  P  | print QTree            |
| TAB | cycle application mode |

## Application Modes
1. Insert Boundary
2. Insert Inner points
3. ... (view mesh (TODO))

## TODOs
- [ ] Use qtree to compute closest point
- [ ] Outside of boundary bounds check for inner point insertion
- [ ] Compute Triangulation


## Faraway TODOs
- [ ] Mesh Editor
- [ ] Holes in Mesh (maybe via boundary insertion afterwards)
- [ ] Delaunay Triangulation for moar regularity (or figure something out myself)
- [ ] serialization as VTK (or STL or something)
