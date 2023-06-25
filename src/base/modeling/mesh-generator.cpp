#include "mesh-generator.hpp"

#include "delaunay.hpp"

#include <geometry/draw-2d.hpp>

class LimbConnexions {
public:
  LimbConnexions() {}

  /*Insert connexion p->q or q->p.
  Ensure that only one of those connexions exist
  And that each point is connected to at most one point (which can be the itself).*/
  inline void insertConnexion(
    unsigned p, unsigned q,
    unsigned pCylinder, bool pIsA,
    unsigned qCylinder, bool qIsA
  ) {
    auto connP = connexions.find(p);
    auto connQ = connexions.find(q);
    if(connP != connexions.end()) {
      // If p is connected to some point
      if(connP->second != q) {
        // If p is not connected to q
        // We connect it to q
        connP->second = q;
      }
    }
    else {

      if(connQ != connexions.end()) {
        // If q is connected to some point
        if(connQ->second != p) {
          // If q is not connected to p
          // We connect it to p
          connQ->second = p;
        }
      }

      else {
        // Is p is not connected to q and q is not connected to p
        // We connect then
        connexions.insert({p, q});
      }
      
    }

    pointToCylinders.insert({p, {pCylinder, pIsA}});
    pointToCylinders.insert({q, {qCylinder, qIsA}});
  }

  /*Return -1 is no direct connexion is found.*/
  inline int getPointConnexion(unsigned p) {
    auto conn = connexions.find(p);
    if(conn == connexions.end()) {
      return -1;
    }
    else {
      return conn->second;
    }
  }

  inline std::vector<unsigned> getBothWayConnexion(unsigned p) {
    std::vector<unsigned> res;
    for(auto conn : connexions) {
      if(conn.first == p) res.push_back(conn.second);
      if(conn.second == p) res.push_back(conn.first);
    }
    return res;
  }

  std::map<unsigned, std::pair<unsigned, bool>> pointToCylinders;

private:
  std::map<unsigned, unsigned> connexions;
};

MeshGenerator::~MeshGenerator() {
  if(limbConnexions) {
    delete limbConnexions;
    limbConnexions = nullptr;
  }
}

static std::map<unsigned, std::pair<unsigned, bool>>::const_iterator find(
  const std::map<unsigned, std::pair<unsigned, bool>> & ps,
  unsigned p
) {
  for(auto it=ps.begin(); it!=ps.end(); it++) {
    if(it->first == p) return it;
  }
  return ps.end();
}

/*Follow the shape to find the next stop point after the point.*/
static int findConnectedStopPoint(
  const std::vector<glm::vec2> & points,
  const std::map<unsigned, std::pair<unsigned, bool>> & stopPoints,
  int point, int step,
  std::pair<unsigned, bool> & cyl
) {
  int i=point+step;
  auto it = find(stopPoints, i%points.size());
  while(
    it==stopPoints.end()
  ) {
    i += step;
    it = find(stopPoints, i%points.size());
  }
  cyl = it->second;
  return i;
}

static bool isInCylinderLastChordList(
  unsigned a, unsigned b,
  const std::list<CylinderGenerator> & cylinders
) {
  for(auto & cyl : cylinders) {
    auto c = cyl.getLastChord();
    if((c.a==a && c.b==b) || (c.a==b && c.b==a)) return true;
  }
  return false;
}

void MeshGenerator::compute() {
// We remove that last joint --------------------------------------------
  for(auto & axis : externalAxis) {
    axis.erase(axis.begin()+axis.size()-1);
  }

// We compute the limb cylinders meshes ---------------------------------------
  
  // cylinders.reserve(externalAxis.size());
  for(unsigned i=0; i<externalAxis.size(); i++) {
    auto axis = externalAxis[i];
    cylinders.emplace_back(axis, points, chords, axisPointToChord);
    cylinders.back().compute(cylinderResolution);

    for(auto f : cylinders.back().getFaces()) {
      faces.push_back({
        f.x + vertices.size(),
        f.y + vertices.size(),
        f.z + vertices.size()
      });
    }
    cylinders.back().offset = vertices.size();
    vertices.insert(
      vertices.end(),
      cylinders.back().getVertexPos().begin(),
      cylinders.back().getVertexPos().end()
    );
  }

// We find the limb last chord connexions -------------------------------------

  std::map<unsigned, std::pair<unsigned, bool>> stopPoints;
  for(unsigned index=0; index<cylinders.size(); index++) {
    auto cylinder = std::next(cylinders.begin(), index);
    stopPoints.insert({cylinder->getLastChord().a, {index, true}});
    stopPoints.insert({cylinder->getLastChord().b, {index, false}});
  }

  limbConnexions = new LimbConnexions();

  for(unsigned index=0; index<cylinders.size(); index++) {
    auto cylinder = std::next(cylinders.begin(), index);
    int pointA = cylinder->getLastChord().a;
    int pointB = cylinder->getLastChord().b;

    // Point A of chord
    std::pair<unsigned, bool> connectCyl;
    int connectedToA = findConnectedStopPoint(points, stopPoints, pointA, +1, connectCyl);
    if(connectedToA==pointB) {
      // The direction is +1 goes in the limb
      // We try -1
      connectedToA = findConnectedStopPoint(points, stopPoints, pointA, -1, connectCyl);

      if(isInCylinderLastChordList(pointA, connectedToA, cylinders)) {
        // Point A belong to two cylinder last chords
        limbConnexions->insertConnexion(pointA, pointA, index, true, index, true);
      }
      else {
        limbConnexions->insertConnexion(pointA, connectedToA, index, true, connectCyl.first, connectCyl.second);
      }
    }
    else {
      if(isInCylinderLastChordList(pointA, connectedToA, cylinders)) {
        // Point A belong to two cylinder last chords
        limbConnexions->insertConnexion(pointA, pointA, index, true, index, true);
      }
      else {
        limbConnexions->insertConnexion(pointA, connectedToA, index, true, connectCyl.first, connectCyl.second);
      }
    }

    // Point B of chord
    int connectedToB = findConnectedStopPoint(points, stopPoints, pointB, +1, connectCyl);
    if(connectedToB==pointA) {
      // The direction is +1 goes in the limb
      // We try -1
      connectedToB = findConnectedStopPoint(points, stopPoints, pointB, -1, connectCyl);

      if(isInCylinderLastChordList(pointB, connectedToB, cylinders)) {
        // Point A belong to two cylinder last chords
        limbConnexions->insertConnexion(pointB, pointB, index, false, index, false);
      }
      else {
        limbConnexions->insertConnexion(pointB, connectedToB, index, false, connectCyl.first, connectCyl.second);
      }
    }
    else {
      if(isInCylinderLastChordList(pointB, connectedToB, cylinders)) {
        // Point A belong to two cylinder last chords
        limbConnexions->insertConnexion(pointB, pointB, index, false, index, false);
      }
      else {
        limbConnexions->insertConnexion(pointB, connectedToB, index, false, connectCyl.first, connectCyl.second);
      }
    }
  }

// We create the linking parts of the mesh
  for(auto & cylinder : cylinders) {
    {
      auto pointA = cylinder.getLastChord().a;
      int connA = limbConnexions->getPointConnexion(pointA);
      if(connA >= 0) {
        auto triangles = createConnexionGrid(pointA, connA);
        faces.insert(
          faces.end(), triangles.begin(), triangles.end()
        );
      }
    }
    {
      auto pointB = cylinder.getLastChord().b;
      int connB = limbConnexions->getPointConnexion(pointB);
      if(connB >= 0) {
        auto triangles = createConnexionGrid(pointB, connB);
        faces.insert(
          faces.end(), triangles.begin(), triangles.end()
        );
      }
    }
  }

  std::vector<unsigned> frontPoints;
  std::vector<unsigned> backPoints;
  std::vector<glm::vec2> frontPointsPos;
  std::vector<glm::vec2> backPointsPos;

  cylinders.sort(
    [](const CylinderGenerator & a, const CylinderGenerator & b) {
      return a.getLastChord().a < b.getLastChord().a;
    }
  );

  for(auto & cylinder : cylinders) {
    std::cout << "cylinder : " << cylinder.getLastChord().a << "  " << cylinder.getLastChord().b << std::endl;
  }

  for(auto & cylinder : cylinders) {
    cylinder.getLastChord().a;
    unsigned pa = cylinder.getLastSectionSideAVertices().front();
    unsigned qa = cylinder.getLastSectionSideAVertices().back();
    unsigned pb = cylinder.getLastSectionSideBVertices().front();
    unsigned qb = cylinder.getLastSectionSideBVertices().back();

    if(cylinder.getVertexPos()[pa].z > 0) {
      frontPoints.push_back(pa+cylinder.offset);
      frontPointsPos.push_back(cylinder.getVertexPos()[pa]);
    }
    else {
      backPoints.push_back(pa+cylinder.offset);
      backPointsPos.push_back(cylinder.getVertexPos()[pa]);
    }

    if(cylinder.getVertexPos()[qa].z > 0) {
      frontPoints.push_back(qa+cylinder.offset);
      frontPointsPos.push_back(cylinder.getVertexPos()[qa]);
    }
    else {
      backPoints.push_back(qa+cylinder.offset);
      backPointsPos.push_back(cylinder.getVertexPos()[qa]);
    }

    if(cylinder.getVertexPos()[pb].z > 0) {
      frontPoints.push_back(pb+cylinder.offset);
      frontPointsPos.push_back(cylinder.getVertexPos()[pb]);
    }
    else {
      backPoints.push_back(pb+cylinder.offset);
      backPointsPos.push_back(cylinder.getVertexPos()[pb]);
    }

    if(cylinder.getVertexPos()[qb].z > 0) {
      frontPoints.push_back(qb+cylinder.offset);
      frontPointsPos.push_back(cylinder.getVertexPos()[qb]);
    }
    else {
      backPoints.push_back(qb+cylinder.offset);
      backPointsPos.push_back(cylinder.getVertexPos()[qb]);
    }
  }

  {
    ConstrainedDelaunayTriangulation2D triangulation(frontPointsPos);
    auto trianglesFront = triangulation.getTriangles();
    for(auto t : trianglesFront) {
      faces.push_back({
        frontPoints[t.x], frontPoints[t.y], frontPoints[t.z]
      });
    }

    {
      Geometry::DrawBuilder builder(200, 200);
      glm::vec3 chordColor = {200, 200, 0};
      glm::vec3 shapeColor = {200, 0, 200};
      glm::vec3 pointColor = {0, 200, 200};
      builder.drawTriangles(frontPointsPos, trianglesFront, chordColor);
      // builder.drawShape(true, frontPointsPos, shapeColor);
      builder.drawPoints(frontPointsPos, pointColor);
      builder.save("debug.png");
    }
  }

  {
    ConstrainedDelaunayTriangulation2D triangulation(backPointsPos);
    auto trianglesBack = triangulation.getTriangles();
    for(auto t : trianglesBack) {
      faces.push_back({
        backPoints[t.z], backPoints[t.y], backPoints[t.x]
      });
    }
  }
}

std::vector<std::pair<std::list<CylinderGenerator>::iterator, bool>> MeshGenerator::getCylindersOnPoint(
  unsigned p
) {
  std::vector<std::pair<std::list<CylinderGenerator>::iterator, bool>> res;

  auto pConns = limbConnexions->getBothWayConnexion(p);
  for(auto q : pConns) {
    auto qIt = limbConnexions->pointToCylinders.find(q);
    if(qIt != limbConnexions->pointToCylinders.end()) {
      res.push_back({
        std::next(cylinders.begin(), qIt->second.first),
        qIt->second.second
      });
    }
  }

  // for(auto & cylinder : cylinders) {
  //   if(cylinder.getLastChord().a==p) {
  //     res.push_back({cylinder, true});
  //   }
  //   else if(cylinder.getLastChord().b==p) {
  //     res.push_back({cylinder, false});
  //   }
  // }

  return res;
}

static void connectSidesSameOrder(
  std::vector<unsigned> & side1, std::vector<unsigned> & side2,
  unsigned side1Offset, unsigned side2Offset,
  std::vector<glm::uvec3> & faces
) {
  for(unsigned i=0; i<side1.size()-1; i++) {
    if(i < side2.size()-1) {
      faces.push_back({
        side2[i+1]+side2Offset, side2[i]+side2Offset, side1[i]+side1Offset
      });
    }
    faces.push_back({
      side1[i+1]+side1Offset, side2[i+1]+side2Offset, side1[i]+side1Offset
    });
  }
}

static void connectSidesNotSameOrder(
  std::vector<unsigned> & side1, std::vector<unsigned> & side2,
  unsigned side1Offset, unsigned side2Offset,
  std::vector<glm::uvec3> & faces
) {
  int N = side2.size()-1;
  for(unsigned i=0; i<side1.size()-1; i++) {
    if(i < side2.size()-1) {
      faces.push_back({
        side2[N-(i+1)]+side2Offset, side2[N-i]+side2Offset, side1[i]+side1Offset
      });
    }
    faces.push_back({
      side1[i+1]+side1Offset, side2[N-(i+1)]+side2Offset, side1[i]+side1Offset
    });
  }
}

static std::vector<glm::uvec3> connexionFaces(
  const std::pair<std::list<CylinderGenerator>::iterator, bool> & connCyl1,
  const std::pair<std::list<CylinderGenerator>::iterator, bool> & connCyl2
) {
  std::vector<unsigned> side1;
  std::vector<unsigned> side2;
  unsigned side1Offset = connCyl1.first->offset;
  unsigned side2Offset = connCyl2.first->offset;
  if(connCyl1.second) {
    side1 = connCyl1.first->getLastSectionSideAVertices();
  }
  else {
    side1 = connCyl1.first->getLastSectionSideBVertices();
  }

  if(connCyl2.second) {
    side2 = connCyl2.first->getLastSectionSideAVertices();
  }
  else {
    side2 = connCyl2.first->getLastSectionSideBVertices();
  }

  if(glm::abs(int(side1.size())-int(side2.size())) > 1) {
    std::cerr << "Sides with vertices count different higher than 1" << std::endl;
    assert(false);
  }

  std::vector<glm::uvec3> faces;

  if(connCyl1.first->getVertexPos()[side1[0]].z * connCyl2.first->getVertexPos()[side2[0]].z > 0 ) {
    // Same order
    std::cout << "Same order" << std::endl;
    if(side1.size() <= side2.size()) {
      connectSidesSameOrder(side1, side2, side1Offset, side2Offset, faces);
    }
    else {
      connectSidesSameOrder(side2, side1, side2Offset, side1Offset, faces);
    }
  }
  else {
    std::cout << "Not same order" << std::endl;
    if(side1.size() <= side2.size()) {
      connectSidesNotSameOrder(side1, side2, side1Offset, side2Offset, faces);
    }
    else {
      connectSidesNotSameOrder(side2, side1, side2Offset, side1Offset, faces);
    }
  }

  return faces;
}

std::vector<glm::uvec3> MeshGenerator::createConnexionGrid(unsigned p, unsigned q) {
  if(p==q) {
    auto connCyls = getCylindersOnPoint(p);
    if(connCyls.size()==0) {
      std::cerr << "Point with no cylinder" << std::endl;
      assert(false);
    }
    else if(connCyls.size()==1) {
      std::cerr << "Double point with one cylinder" << std::endl;
      assert(false);
    }
    else if(connCyls.size()==2) {
      return connexionFaces(connCyls.at(0), connCyls.at(1));
    }
  }
  else {
    auto connCylsP = getCylindersOnPoint(p);
    auto connCylsQ = getCylindersOnPoint(q);
    if(connCylsP.size()==0 || connCylsQ.size()==0) {
      std::cerr << "Point not in a cylinder's last chord" << std::endl;
      assert(false);
    }
    else if(connCylsP.size()>1 || connCylsQ.size()>1) {
      std::cerr << "Two single points with two or more cylinders" << std::endl;
      assert(false);
    }
    else {
      return connexionFaces(connCylsP.at(0), connCylsQ.at(0));
    }
  }

  return std::vector<glm::uvec3>();
}
