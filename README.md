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

### Pipeline Parameters

The pipeline parameters are in three sections in the parameters window: **Douglas-Peucker**, **Medial axis** and **3D generation**.

**Douglas-Peucker :**
- **global threshold** is the threshold for the Cylindrical Douglas-Peucker.
- **Cyl error weight** is the weight of the cylindrical distance.
- **Dist error weight** is the weight of the Douglas-Peucker distance.

**Medial axis**
- **Pruning threshold** is the threshold for the medial axis pruning threshold when removnig insignificant branches from the medial axis.
- **Smoothing size** the window size for the laplacian smoothing applied to the medial axis. The bigger the smoother are the medial axis.
- **Shape sampling** the number of point that we skip in the shape for some steps of the pipeline.

**3D generation**
- **Cylinder sampling** the number of vertices in a section of each cylinder of the mesh.

### Buttons and Utilities

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
To help you draw, you might want to havean image in the drawing space. To do that, write the image file name in the **Texture file** text input then check the **Use texture** option. Once the drawing is finished uncheck the **Use texture** option to hide the texture.

**Skeleton movement**
Once the mesh and its skeleton are generated, the section *Skeleton* on the parameters window will show the number of bones and a slider allowing you to select a bone. Once you select the bone you want to move, you can make rotations around its ivot joint using the buttons : **RotatioX+**, **RotatioX-**, **RotatioY+**, **RotatioY-**, **RotatioZ+** and **RotatioZ-**.

**Hide/show skeleton, mesh and shape**
Check/uncheck the **Show skeleton**, **Show mesh** and **Display drawing** options.

**Erase**
You can erase the current 2D shape by  click **CLEAR DRAWING**.
You can also erase the mesh and its skeleton by clicking **Clear mesh**.

**Take a photo**
click on the button **Photo** to take a photo of want you see on the screen (your mesh and its skeleton). The image is in the TGA format and save as *photoi.tga* (*photo0.tga*, *photo1.tga*, etc ...).

**Steps results screen shots**
The Results of the different steps of the pipeline are saved in images in the local directory. You can change their resolution with the sliders **resolution w** and **resolution h**.

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
