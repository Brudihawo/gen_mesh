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

## In-File TODOS
- [x] `./src/logging.h:10`:       TODO: Fix Verbosity thing
- [ ] `./src/datastructs.c:36`:   TODO: maybe condense this using setting of bits
- [ ] `./tests/test_qtree.c:72`:  TODO: assertion for correctness is missing
- [ ] `./tests/test_qtree.c:95`:  TODO: Add assert statement for verification
- [ ] `./tests/test_qtree.c:104`: TODO: Test insert position location correctness
- [ ] `./tests/test_qtree.c:105`: TODO: Test closest point correctness
