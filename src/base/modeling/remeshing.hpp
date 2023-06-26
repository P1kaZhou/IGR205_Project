#ifndef _SKETCHY_REMESHING_
#define _SKETCHY_REMESHING_

#include <utils.hpp>

#include <vcg/vcg/complex/complex.h>
#include <vcg/vcg/complex/algorithms/clean.h>
#include <vcg/vcg/complex/algorithms/isotropic_remeshing.h>

using namespace vcg;
using namespace std;

class MyEdge;
class MyFace;
class MyVertex;
struct MyUsedTypes : public UsedTypes<	Use<MyVertex>   ::AsVertexType,
        Use<MyEdge>     ::AsEdgeType,
        Use<MyFace>     ::AsFaceType>{};

class MyVertex  : public Vertex<MyUsedTypes, vertex::Coord3f, vertex::Normal3f, vertex::VFAdj, vertex::Qualityf, vertex::BitFlags,  vertex::Mark>{};
class MyFace    : public Face< MyUsedTypes, face::Mark,  face::VertexRef, face::VFAdj, face::FFAdj, face::Normal3f, face::BitFlags > {};
class MyEdge    : public Edge<MyUsedTypes>{};
class MyMesh    : public tri::TriMesh< vector<MyVertex>, vector<MyFace> , vector<MyEdge>  > {};

inline void remeshing(
  std::vector<glm::vec3> & vertices,
  std::vector<glm::uvec3> & faces
) {
  MyMesh original, toremesh;
  original.vert.reserve(vertices.size());
  original.face.reserve(faces.size());
  for(int i=0; i<vertices.size(); i++) {
    auto & v = vertices[i];
    tri::Allocator<MyMesh>::AddVertex(
      original,
      typename MyMesh::CoordType(v.x,v.y,v.z)
    );
  }
  for(auto & f : faces) {
    vcg::tri::Allocator<MyMesh>::AddFace(
      original,
      &original.vert[ f.x ],
      &original.vert[ f.y ],
      &original.vert[ f.z ]
    );
  }

  vertices.clear();
  faces.clear();

  float targetLenPerc=0.05f;
	int iterNum=2;
	float creaseAngle = 30.f;
	float maxSurfDistPerc = 0;

  // tri::Clean<MyMesh>::RemoveUnreferencedVertex(original);
	// vcg::tri::Allocator<MyMesh>::CompactEveryVector(original);

  // tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized(original);
	tri::UpdateBounding<MyMesh>::Box(original);

  vcg::tri::Append<MyMesh,MyMesh>::MeshCopy(toremesh,original);
	tri::UpdateNormal<MyMesh>::PerVertexNormalizedPerFaceNormalized(toremesh);
	tri::UpdateBounding<MyMesh>::Box(toremesh);

  tri::UpdateTopology<MyMesh>::FaceFace(toremesh);
	float lengthThr = targetLenPerc*(original.bbox.Diag()/100.f);
	float maxSurfDist = maxSurfDistPerc*(original.bbox.Diag()/100.f);

  vcg::tri::IsotropicRemeshing<MyMesh>::Params params;
	params.SetTargetLen(lengthThr);
	params.SetFeatureAngleDeg(creaseAngle);
	params.iter=iterNum;

  if (maxSurfDistPerc != 0)
	{
		params.surfDistCheck = true;
		params.maxSurfDist = maxSurfDist;
	}
	else
	{
		params.surfDistCheck = false;
	}
	params.cleanFlag = true;
	params.userSelectedCreases = false;

  vcg::tri::IsotropicRemeshing<MyMesh>::Do(toremesh, original, params);

  vertices.reserve(toremesh.vert.size());
  faces.reserve(toremesh.face.size());
  for(auto & mv : toremesh.vert) {
    vertices.push_back({mv.P().X(), mv.P().Y(), mv.P().Z()});
  }
  for(auto & mf : toremesh.face) {
    faces.push_back({mf.V(0)->VFi(), mf.V(1)->VFi(), mf.V(2)->VFi()});
  }
}

#endif
