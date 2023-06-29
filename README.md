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

## Usage
When running the program, two windows will appear. One with the space to draw (It's white). Another with the different parameters.

#### Draw your own shape
**Draw the shape**
You can draw by left clicking on the white space window.

**Run the pipeline**
Click on the **START** button in the parameters window.

**Save to STL**
you can click on **Mesh STL file** to generate a STL file of the generated mesh.

**Save your 2D shape**
In the text input **shape file** write the name of the file where you want to save your 2D shape then click **Save shape file**

**Load a 2D shape**
In the text input **shape file** write the name of the shape that you want to load your 2D shape from then click **Load shape file**.
Once you load the shape, you can click **START** to generate its 3D mesh.

**Display a texture**
To help you draw, you might want to havean image in the drawing space. To do that, write the image file name in the **Texture file** text input then click on **Use texture**. Once the drawing is finished click again on **Use texture**.

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
