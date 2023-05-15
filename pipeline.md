## The pipeline

At each stroke, from the 2D Sketch of a shape
1. Decompose the shape into 2D cylinders with well defined symmetry axes
2. Generate the 3D surface
3. Generate the skeleton
4. Skin weights computation

Operations
1. Cutting
2. Merging
3. Skeleton refinement


### At each stroke

#### 1. shape to cylinders - Chordal axis transform (CAT)

Getting the cylinders junction triangles and the chordal axis:
- Sample points along the shape for the Triangulation.
- Compute a **Constrained Delaunay Triangulation** of sampled points with the initial edges as the constraints.
- Compute the **chordal/medial axis** of the shape (maybe an approximation with Voronoi cells)
- We get the **junction triangles** from the triangulation as the triangles in which at least three medial axis are joined.

Refinement:
- We **prune** the chordal axis to remove unsignificant axis
- We apply **Laplacien smoothing** to
    - the chordal axis 
    - the **orientation** of internal edges of the triangulation (the chords), they become perpendicular to the chordal axis. (?)

Connecting region:
- The region between to junction triangles are merge with those triangles to form junction regions.

The result :
- a set of cylindrical regions
- a set of junction regions

#### 2. Generate the 3D surface

- The cylindrical regions are easily transformed into 3D cylinders :
  - the chordial axis are the cylinders axis
  - the chords become the radius of each cross section
  - adjacent cross sections are merge by triangles

- The junction regions to 3D ?

The entire mesh is then smoothed with **Least-squares Meshes** :
- The cylinders vertices are constrained
- The junction region vertices are unconstrained

#### 3. Generate the skeleton

**For the cylindrical regions:**
We down sample the chordal axis into **joints** and **bones**. To do so we use a modified version of the Douglas-Peucker algorithm : The Cylindrical Douglas-Peucker algorithm.

**For connecting regions:**
>A joint is placed at the center of each junction triangle and a bone is added across each of its three sides
> 
>If a side is adjacent to a cylindrical region, the bone is connected to the end of its skeleton; otherwise, the side must be adjacent
to another junction triangle, and the bone connects both junction
triangles’ centers

#### 4. Skin weights computation

The method listed in the `RigMesh` paper involves making bones radiating heat onto the surface, and the equilibrium temperature at
each vertex is computed by solving a linear system of equations. 


### Operations

#### 1. Cutting

#### 2. Merging

#### 3. Skeleton refinement
