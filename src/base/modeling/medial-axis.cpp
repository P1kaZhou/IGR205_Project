#include "medial-axis.hpp"

void MedialAxis::showAsAdjMatrix() {
  unsigned N = points.size();
  int mat[N][N];
  memset(mat, 0, sizeof(int)*N*N);
  for(unsigned i=0; i<N; i++) {
    auto p = points[i];
    for(unsigned k=0; k<p->getAdjs().size(); k++) {
      auto q = p->getAdjs()[k];
      unsigned j = getAxisPointIndex(q->getPoint());
      mat[i][j] = 1;
    }
  }
  for(unsigned i=0; i<N; i++) {
    for(unsigned j=0; j<N; j++) {
      std::cout << mat[i][j];
      if(j<N-1) std::cout << ", ";
    }
    std::cout << std::endl;
  }
}

void MedialAxis::showAsAdjList() {
  for(unsigned i=0; i<points.size(); i++) {
    auto p = points[i];
    std::cout << i;
    showVec(p->getPoint(), " point");
    std::cout << "count " << p->getAdjs().size() << std::endl;
    for(auto q : p->getAdjs()) {
      std::cout << "\t" << getAxisPointIndex(q->getPoint());
      showVec(q->getPoint(), " n");
    }
  }
}

