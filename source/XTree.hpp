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

template<typename T, size_t N, size_t M>
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
    HyperRectangle<T, N> MBR;
    XEntry();
    virtual std::shared_ptr<XNode> get_child() { return std::shared_ptr<XNode>; }
    virtual HyperPoint<T, N> get_data() { return HyperPoint<T, N>; }
  };

  struct XInternal_Entry :public XEntry {
    std::shared_ptr<XNode> child;
    XInternal_Entry(std::shared_ptr<XNode> &node);
    std::shared_ptr<XNode> get_child() override;
  };

  struct XLeaf_Entry :public XEntry {
    HyperPoint<T, N> data;
    XLeaf_Entry(HyperPoint<T, N> &s_data);
    HyperPoint<T, N> get_data() override;
  };

  struct XNode {
    XNode();
    bool is_supernode();
    bool is_leaf();
    void upgrade_to_supernode();
  private:
    bool supernode_mode;
    std::vector<std::shared_ptr<XEntry>> entries;
    size_t node_size;
  };

  void topological_split();
  size_t choose_split_axis();
  size_t choose_split_index();
  void overlap_minimal_split();
  bool split_node();
  void insert(XLeaf_Entry &new_sp_obj);
  void choose_leaf(XLeaf_Entry data_entry, std::stack<std::shared_ptr<XNode>> &parents_path);

  std::shared_ptr<XNode> root;
public:
  XTree();
  void assign(std::vector<HyperPoint<T, N>> &raw_data_set);
};

//===============================================XTree Implementation==================================================

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XTree() {
  assert(M > size_t(2));
  root = std::make_shared<XNode>();
}

template<typename T, size_t N, size_t M>
void XTree<T, N, M>::assign(std::vector<HyperPoint<T, N>> &raw_data_set) {
  for (HyperPoint<T, N> hp : raw_data_set) {
    XLeaf_Entry new_entry(hp);
    insert(new_entry);
  }
}

template<typename T, size_t N, size_t M>
void XTree<T, N, M>::topological_split() {

}

template<typename T, size_t N, size_t M>
size_t XTree<T, N, M>::choose_split_axis() {

}

template<typename T, size_t N, size_t M>
size_t XTree<T, N, M>::choose_split_index() {

}

template<typename T, size_t N, size_t M>
void XTree<T, N, M>::overlap_minimal_split() {

}

template<typename T, size_t N, size_t M>
bool XTree<T, N, M>::split_node() {

}

template<typename T, size_t N, size_t M>
void XTree<T, N, M>::insert(XLeaf_Entry &new_sp_obj) {
  std::stack<std::shared_ptr<XNode>> parents; choose_leaf(new_sp_obj, parents);
  while (!parents.empty()) {
    std::shared_ptr<XNode>& current = parents.top();
    parents.pop();

  }
  int action = 0;
  if (action == SPLIT) {

  }
  else if (action == SUPERNODE) {

  }
  return NO_SPLIT;
}

template<typename T, size_t N, size_t M>
void XTree<T, N, M>::choose_leaf(XLeaf_Entry data_entry, std::stack<std::shared_ptr<XNode>> &parents_path) {
  assert(root != nullptr);
  std::shared_ptr<XNode> candidate_node = root;
  while (!candidate_node->is_leaf()) {
    parents_path.push(candidate_node);
    std::shared_ptr<XNode> temp = candidate_node;
    for (size_t i(0); i < temp->node_size; ++i) {
      candidate_node = temp->entries[i]->get_element();
      if (temp->entries[i]->MBR.contains(data_entry.data))
        break;
    }
  }
  parents_path.push(candidate_node);
}

//===============================================XNode Implementation==================================================

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XNode::XNode() {
  supernode_mode = false;
  node_size = size_t(0);
  entries.resize(M);
}

template<typename T, size_t N, size_t M>
bool XTree<T, N, M>::XNode::is_supernode() {
  return supernode_mode;
}

template<typename T, size_t N, size_t M>
bool XTree<T, N, M>::XNode::is_leaf() {
  return typeid(*entries[0]) == typeid(XLeaf_Entry);
}

template<typename T, size_t N, size_t M>
void XTree<T, N, M>::XNode::upgrade_to_supernode() {
  supernode_mode = true;
}

//===============================================XEntry Implementation=================================================

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XEntry::XEntry() {

}

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XInternal_Entry::XInternal_Entry(std::shared_ptr<XNode> &node) {
  child = node;
}

template<typename T, size_t N, size_t M>
std::shared_ptr<typename XTree<T, N, M>::XNode> XTree<T, N, M>::XInternal_Entry::get_child() {
  return child;
}

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XLeaf_Entry::XLeaf_Entry(HyperPoint<T, N> &s_data) {
  data = s_data;
}

template<typename T, size_t N, size_t M>
HyperPoint<T, N> XTree<T, N, M>::XLeaf_Entry::get_data() {
  return data;
}