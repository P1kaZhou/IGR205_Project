# IGR205_Project

Implementation of a Sketch-based 3D Modeling and Rigging interface.

## Build and run

**Build:**
From the project directory:
```
cmake src -B build
make -C build
```

**Run:**
From the projet directory:
```
./src/sketchy
```

## Libraries used
* [Glad](https://github.com/Dav1dde/glad) for OpenGL calls. (MIT Licence)
* [GLM](https://glm.g-truc.net/) for maths with OpenGL. (MIT Licence)
* [GLFW](https://www.glfw.org/) for OpenGL context and window system. (zlib/libpng License, a BSD-like license)
* [STB](https://github.com/nothings/stb) for image loading and writing. (MIT Licence)
* [Dear ImGUI](https://github.com/ocornut/imgui) for the GUI. (MIT Licence)
* [CDT](https://github.com/artem-ogre/CDT/tree/master) for Constrained Delaunay Triangulation. (Mozilla Public Licence)
* [jc_voronoi](https://github.com/JCash/voronoi) for Voronoi diagrams in medial axis approximation. (MIT Licence)
* [CGAL](https://www.cgal.org/index.html) for Delaunay Triangulations, Voronoi diagrams and computational geometry in general . (GNU GPL Licence)

## Resources

- [Rigmesh: automatic rigging for part-based shape modeling and
deformation](https://dl.acm.org/doi/pdf/10.1145/2366145.2366217)

- [Monster mash: a single-view approach to casual 3D modeling and
animation](https://dl.acm.org/doi/pdf/10.1145/3414685.3417805)

- [Teddy: a sketching interface for
3D freeform design](https://www.cs.toronto.edu/~jacobson/seminar/igarashi-et-al-1999.pdf)
