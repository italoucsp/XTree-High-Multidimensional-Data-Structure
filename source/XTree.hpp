#include <xtree_utils.hpp>

#define DEFAULT_MAX_OVERLAP

#ifndef DEFAULT_MAX_OVERLAP
  const double Tcpu = 1;//ms
  const double Ttr = 4;//ms
  const double Tio = 20;//ms
#endif

#define SPLIT 1
#define SUPERNODE 2
#define NO_SPLIT 3

template<typename T, size_t DIMENSIONALITY, size_t M>
class XTree {
private:
  const size_t MINFANOUT = (M * 45) / 100;//35% - 45%
#ifdef DEFAULT_MAX_OVERLAP
  const double MAX_OVERLAP = 0.2;
#else
  const double MAX_OVERLAP = (Ttr + Tcpu) / (Tio + Ttr + Tcpu);
#endif

  struct XNode;
  struct XEntry {//[MBR | split_history | child/data]
    HyperRectangle<T, DIMENSIONALITY> MBR;
    XEntry();
  };
  struct XInternal_Entry :public XEntry {
    std::shared_ptr<XNode> child;
    XInternal_Entry(std::shared_ptr<XNode> &node);
  };
  struct XLeaf_Entry :public XEntry {
    HyperPoint<T, DIMENSIONALITY> data;
    XLeaf_Entry(HyperPoint<T, DIMENSIONALITY> &s_data);
  };
  struct XNode {
    XNode();
    bool is_supernode();
    void upgrade_to_supernode();
    std::shared_ptr<XNode> choose_subtree(XLeaf_Entry data_entry);
    int insert(XLeaf_Entry &new_sp_obj);
  private:
    bool supernode_mode;
    std::vector<XEntry> entries;
    size_t node_size;
  };

  void topological_split();
  void overlap_minimal_split();
  bool split_node();

  std::shared_ptr<XNode> root;
public:
  XTree();
  void assign(std::vector<HyperPoint<T, DIMENSIONALITY>> &raw_data_set);
};

//===============================================XTree Implementation==================================================

template<typename T, size_t DIMENSIONALITY, size_t M>
XTree<T, DIMENSIONALITY, M>::XTree() {
  assert(M > size_t(2));
  root = std::make_shared<XNode>();
}

template<typename T, size_t DIMENSIONALITY, size_t M>
void XTree<T, DIMENSIONALITY, M>::assign(std::vector<HyperPoint<T, DIMENSIONALITY>> &raw_data_set) {
  for (HyperPoint<T, DIMENSIONALITY> hp : raw_data_set) {

  }
}

//===============================================XNode Implementation==================================================

template<typename T, size_t DIMENSIONALITY, size_t M>
XTree<T, DIMENSIONALITY, M>::XNode::XNode() {
  supernode_mode = false;
  node_size = size_t(0);
  entries.resize(M);
}

template<typename T, size_t DIMENSIONALITY, size_t M>
bool XTree<T, DIMENSIONALITY, M>::XNode::is_supernode() {
  return supernode_mode;
}

template<typename T, size_t DIMENSIONALITY, size_t M>
void XTree<T, DIMENSIONALITY, M>::XNode::upgrade_to_supernode() {
  supernode_mode = true;
}

template<typename T, size_t DIMENSIONALITY, size_t M>
std::shared_ptr<typename XTree<T, DIMENSIONALITY, M>::XNode> XTree<T, DIMENSIONALITY, M>::XNode::choose_subtree(XLeaf_Entry data_entry) {
  std::shared_ptr<XNode> follow;
  for (size_t i(0); i < node_size; ++i) {

  }
  return follow;
}

template<typename T, size_t DIMENSIONALITY, size_t M>
int XTree<T, DIMENSIONALITY, M>::XNode::insert(XLeaf_Entry &new_sp_obj) {
  std::shared_ptr<XNode> follow = choose_subtree(new_sp_obj);
  int action = insert(new_sp_obj);
  if (action == SPLIT) {

  }
  else if (action == SUPERNODE) {

  }
  return NO_SPLIT;
}