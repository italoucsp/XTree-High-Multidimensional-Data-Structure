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

#define SUCCESS 1
#define FAILURE 0

//COMPARATORS
#define COMP_ENTRIES_TSPLIT(axis) [=](const std::shared_ptr<XEntry>& A, const std::shared_ptr<XEntry>& B)->bool {return A->MBR.get_min_bound()[axis] < B->MBR.get_min_bound()[axis]; }

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
    XInternal_Entry(std::shared_ptr<XNode>& node);
    std::shared_ptr<XNode> get_child() override;
  };

  struct XLeaf_Entry :public XEntry {
    HyperPoint<T, N> data;
    XLeaf_Entry(HyperPoint<T, N>& s_data);
    HyperPoint<T, N> get_data() override;
  };

  struct XNode {
    XNode();
    bool is_supernode();
    bool is_leaf();
    void upgrade_to_supernode();
    std::vector<std::shared_ptr<XEntry>> get_entries_4_test();
  private:
    bool supernode_mode;
    std::vector<std::shared_ptr<XEntry>> entries;
    size_t node_size;
  };
  HyperRectangle<T, N> calculate_bounding_hyper_rectangle(std::vector<std::shared_ptr<XEntry>> group);
  double calculate_hyper_margin(std::vector<std::shared_ptr<XEntry>> group);
  double calculate_hyper_area(std::vector<std::shared_ptr<XEntry>> group);
  double calculate_overlap_hyper_area(std::vector<std::shared_ptr<XEntry>> group_1, std::vector<std::shared_ptr<XEntry>> group_2);

  bool topological_split(std::vector<std::shared_ptr<XEntry>> entries_2_split, std::vector<std::shared_ptr<XEntry>>& set_a, std::vector<std::shared_ptr<XEntry>>& set_b);
  double choose_split_index(size_t chosen_axis, std::vector<std::shared_ptr<XEntry>> SET_ENTRIES, std::vector<std::shared_ptr<XEntry>>& GA, std::vector<std::shared_ptr<XEntry>>& GB);
  size_t choose_split_axis(std::vector<std::shared_ptr<XEntry>> SET_ENTRIES);
  bool overlap_minimal_split(std::vector<std::shared_ptr<XEntry>> entries_2_split, std::vector<std::shared_ptr<XEntry>>& set_a, std::vector<std::shared_ptr<XEntry>>& set_b);
  bool split_node(std::shared_ptr<XNode>& node_2_split, XLeaf_Entry& data_overflow, std::shared_ptr<XNode>& set_a, std::shared_ptr<XNode>& set_b);
  void insert(XLeaf_Entry& new_sp_obj);
  void choose_leaf(XLeaf_Entry data_entry, std::stack<std::shared_ptr<XNode>>& parents_path);

  std::shared_ptr<XNode> root;
public:
  XTree();
  void assign(std::vector<HyperPoint<T, N>>& raw_data_set);
};

//===============================================XTree Implementation==================================================

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XTree() {
  assert(M > size_t(2));
  root = std::make_shared<XNode>();
}

template<typename T, size_t N, size_t M>
void XTree<T, N, M>::assign(std::vector<HyperPoint<T, N>>& raw_data_set) {
  for (HyperPoint<T, N> hp : raw_data_set) {
    XLeaf_Entry new_entry(hp);
    insert(new_entry);
  }
}

template<typename T, size_t N, size_t M>
HyperRectangle<T, N> XTree<T, N, M>::calculate_bounding_hyper_rectangle(std::vector<std::shared_ptr<XEntry>> group) {
  assert(group.size() > 0);
  HyperRectangle<T, N> BIG_MBR = group[0]->MBR;
  for each (std::shared_ptr<XEntry> entry in group) {
    BIG_MBR.expand(entry->MBR);
  }
  return BIG_MBR;
}

template<typename T, size_t N, size_t M>
double XTree<T, N, M>::calculate_hyper_margin(std::vector<std::shared_ptr<XEntry>> group) {
  return calculate_bounding_hyper_rectangle(group).get_hyper_margin();
}

template<typename T, size_t N, size_t M>
double XTree<T, N, M>::calculate_hyper_area(std::vector<std::shared_ptr<XEntry>> group) {
  return calculate_bounding_hyper_rectangle(group).get_hyper_area();
}

template<typename T, size_t N, size_t M>
double XTree<T, N, M>::calculate_overlap_hyper_area(std::vector<std::shared_ptr<XEntry>> group_1, std::vector<std::shared_ptr<XEntry>> group_2) {
  HyperRectangle<T, N> BIG_MBR_G1 = calculate_bounding_hyper_rectangle(group_1);
  HyperRectangle<T, N> BIG_MBR_G2 = calculate_bounding_hyper_rectangle(group_2);
  if (BIG_MBR_G1.get_max_bound() > BIG_MBR_G2.get_min_bound()) {
    HyperRectangle<T, N> OVERLAP_HYPER_RECT(BIG_MBR_G2.get_min_bound(), BIG_MBR_G1.get_max_bound());
    return OVERLAP_HYPER_RECT.get_hyper_area();
  }
  else if (BIG_MBR_G1.get_min_bound() < BIG_MBR_G2.get_max_bound()) {
    HyperRectangle<T, N> OVERLAP_HYPER_RECT(BIG_MBR_G1.get_min_bound(), BIG_MBR_G2.get_max_bound());
    return OVERLAP_HYPER_RECT.get_hyper_area();
  }
  else {
    return 0.0;
  }
}

template<typename T, size_t N, size_t M>
bool XTree<T, N, M>::topological_split(std::vector<std::shared_ptr<XEntry>> entries_2_split, std::vector<std::shared_ptr<XEntry>>& set_a, std::vector<std::shared_ptr<XEntry>>& set_b) {
  double overlap = choose_split_index(choose_split_axis(entries_2_split), entries_2_split, set_a, set_b);
  return (overlap < std::min(calculate_hyper_area(set_a), calculate_hyper_area(set_b)) * MAX_OVERLAP);//if true-> success, else-> failure
}

template<typename T, size_t N, size_t M>
size_t XTree<T, N, M>::choose_split_axis(std::vector<std::shared_ptr<XEntry>> SET_ENTRIES) {
  double min_margin_value = std::numeric_limits<double>::max();
  size_t best_axis(0);
  for(size_t axis(0); axis < N; ++axis) {
    std::sort(SET_ENTRIES.begin(), SET_ENTRIES.end(), COMP_ENTRIES_TSPLIT(axis));
    size_t n_distributions = M - 2 * MINFANOUT + 2;
    double current_axis_margin_sum = 0.0;
    for (size_t k(1); k <= n_distributions; ++k) {
      std::vector<std::shared_ptr<XEntry>> first_group(SET_ENTRIES.begin(), SET_ENTRIES.begin() + (MINFANOUT - 1 + k));
      std::vector<std::shared_ptr<XEntry>> second_group(SET_ENTRIES.begin() + (MINFANOUT - 1 + k), SET_ENTRIES.end());
      current_axis_margin_sum += (calculate_hyper_margin(first_group) + calculate_hyper_margin(second_group));
    }
    if (current_axis_margin_sum < min_margin_value) {
      min_margin_value = current_axis_margin_sum;
      best_axis = axis;
    }
  }
  return best_axis;
}

template<typename T, size_t N, size_t M>
double XTree<T, N, M>::choose_split_index(size_t chosen_axis, std::vector<std::shared_ptr<XEntry>> SET_ENTRIES, std::vector<std::shared_ptr<XEntry>>& GA, std::vector<std::shared_ptr<XEntry>>& GB) {
  std::sort(SET_ENTRIES.begin(), SET_ENTRIES.end(), COMP_ENTRIES_TSPLIT(chosen_axis));
  double min_overlap_value = std::numeric_limits<double>::max();
  size_t n_distributions = M - 2 * MINFANOUT + 2;
  for (size_t k(1); k <= n_distributions; ++k) {
    std::vector<std::shared_ptr<XEntry>> first_group(SET_ENTRIES.begin(), SET_ENTRIES.begin() + (MINFANOUT - 1 + k));
    std::vector<std::shared_ptr<XEntry>> second_group(SET_ENTRIES.begin() + (MINFANOUT - 1 + k), SET_ENTRIES.end());
    double current_overlap_value = calculate_overlap_hyper_area(first_group, second_group);
    if (current_overlap_value < min_overlap_value) {
      min_overlap_value = current_overlap_value;
      GA.assign(first_group.begin(), first_group.end());
      GB.assign(second_group.begin(), second_group.end());
    }
    else if (current_overlap_value == min_overlap_value) {
      if ((calculate_hyper_area(first_group) + calculate_hyper_area(second_group)) < (calculate_hyper_area(GA) + calculate_hyper_area(GB))) {
        GA.assign(first_group.begin(), first_group.end());
        GB.assign(second_group.begin(), second_group.end());
      }
    }
  }
  return min_overlap_value;
}

template<typename T, size_t N, size_t M>
bool XTree<T, N, M>::overlap_minimal_split(std::vector<std::shared_ptr<XEntry>> entries_2_split, std::vector<std::shared_ptr<XEntry>>& set_a, std::vector<std::shared_ptr<XEntry>>& set_b) {
  return true;
}

template<typename T, size_t N, size_t M>
bool XTree<T, N, M>::split_node(std::shared_ptr<XNode>& node_2_split, XLeaf_Entry& data_overflow, std::shared_ptr<XNode>& set_a, std::shared_ptr<XNode>& set_b) {
  std::vector<std::shared_ptr<XEntry>> SET_ENTRIES_2_SPLIT(node_2_split->get_entries_4_test());
  SET_ENTRIES_2_SPLIT.push_back(std::make_shared<XLeaf_Entry>(data_overflow.data));
  if (topological_split(SET_ENTRIES_2_SPLIT, set_a, set_b) == FAILURE) {
    set_a.clear(); set_b.clear();
    if (overlap_minimal_split(SET_ENTRIES_2_SPLIT, set_a, set_b) == FAILURE) {
      return FAILURE;
    }
  }
  return SUCCESS;
}

template<typename T, size_t N, size_t M>
void XTree<T, N, M>::insert(XLeaf_Entry& new_sp_obj) {
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
void XTree<T, N, M>::choose_leaf(XLeaf_Entry data_entry, std::stack<std::shared_ptr<XNode>>& parents_path) {
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

template<typename T, size_t N, size_t M>
std::vector<std::shared_ptr<typename XTree<T, N, M>::XEntry>> XTree<T, N, M>::XNode::get_entries_4_test() {
  return entries;
}

//===============================================XEntry Implementation=================================================

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XEntry::XEntry() {

}

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XInternal_Entry::XInternal_Entry(std::shared_ptr<XNode>& node) {
  child = node;
}

template<typename T, size_t N, size_t M>
std::shared_ptr<typename XTree<T, N, M>::XNode> XTree<T, N, M>::XInternal_Entry::get_child() {
  return child;
}

template<typename T, size_t N, size_t M>
XTree<T, N, M>::XLeaf_Entry::XLeaf_Entry(HyperPoint<T, N>& s_data) {
  data = s_data;
}

template<typename T, size_t N, size_t M>
HyperPoint<T, N> XTree<T, N, M>::XLeaf_Entry::get_data() {
  return data;
}