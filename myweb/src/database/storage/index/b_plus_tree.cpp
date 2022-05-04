
#include <string>

#include "../../a-include/common/rid.h"
#include "../../a-include/storage/index/b_plus_tree.h"
#include "../../a-include/storage/page/header_page.h"

namespace bustub {
INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_TYPE::BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager, const KeyComparator &comparator,
                          int leaf_max_size, int internal_max_size)
    : index_name_(std::move(name)),
      root_page_id_(INVALID_PAGE_ID),
      buffer_pool_manager_(buffer_pool_manager),
      comparator_(comparator),
      leaf_max_size_(leaf_max_size),
      internal_max_size_(internal_max_size) {}

INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::IsEmpty() const {
  return root_page_id_ == INVALID_PAGE_ID;
}
INDEX_TEMPLATE_ARGUMENTS
Page *BPLUSTREE_TYPE::FindLeafPage(const KeyType &key, bool leftMost, int mode) {
  //std::cout<<"find leaf begin key:" << key<<"\n";
  
  if (IsEmpty()){
    printf("empty ????\n");
    return nullptr;
  }

  page_id_t page_id = root_page_id_;
  Page *page_ptr = nullptr;
  BPlusTreePage* tree_ptr;
  
  while(true){    
    page_ptr = buffer_pool_manager_->FetchPage(page_id);
    assert(page_ptr != nullptr);
    tree_ptr = reinterpret_cast<BPlusTreePage*>(page_ptr->GetData());
    bool is_leaf = tree_ptr->IsLeafPage();
    if (is_leaf) break;
    InternalPage *internal_ptr= reinterpret_cast<InternalPage*>(tree_ptr);
    buffer_pool_manager_->UnpinPage(page_id, false);
    if(leftMost){
      page_id = internal_ptr->ValueAt(0);
    }else{
      page_id = internal_ptr->Lookup(key, comparator_);
     // printf("find key:%d   lookup:%d\n-=-=", key, page_id);
    }
    
  }
  //std::cout<<"pageid "<<page_ptr->GetPageId()<<"--"<<"find leaf key: "<<key<<"\n";
  return page_ptr; 
}

INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::GetValue(const KeyType &key, std::vector<ValueType> *result) {
  
  if(IsEmpty()) return false;
  //printf("getvalue\n");
  Page *leaf_ptr = FindLeafPage(key, false, READ_MODE);
  LeafPage *leaf_page = reinterpret_cast<LeafPage*>(leaf_ptr->GetData());

  
  RID temp_rid;
  bool ret = leaf_page->Lookup(key, &temp_rid, comparator_);
  if(!ret){
    printf("not find in GetValue ? \n");
  }else{
   // printf("pageid:%d slotid:%d  in getvalue\n", temp_rid.GetPageId(), temp_rid.GetSlotNum());
  }

  buffer_pool_manager_->UnpinPage(leaf_ptr->GetPageId(), false);
 
  

  if (!ret) return false; 

  result->push_back(temp_rid);
 // printf("????\n");
 // printf("res size ===%d\n", result->size());
  return true;
}

INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::Insert(const KeyType &key, const ValueType &value) {
  //std::cout<<"inser begin\n";
 // std::cout<<root_page_id_<<" rootpageid"<<"\n";
  bool ret = false;
  if(IsEmpty()){
    //printf("build tree\n");
    StartNewTree(key, value);
    ret  = true;
  }else{
   // std::cout<<"insert key" << key <<"\n";
    ret = InsertIntoLeaf(key, value);
  }
  return ret;
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::StartNewTree(const KeyType &key, const ValueType &value) {                                           
  page_id_t root_page_id;
  auto root_page_ptr = SafelyNewPage(&root_page_id, "fail to build new tree");
  SetRootPageId(root_page_id);
  //printf("new tree root_page_id:%d\n", root_page_id);
  UpdateRootPageId(1);
  LeafPage *leaf_page = reinterpret_cast<LeafPage*>(root_page_ptr->GetData());
  leaf_page->Init(root_page_id, INVALID_PAGE_ID, leaf_max_size_);
  leaf_page->Insert(key, value, comparator_);
  buffer_pool_manager_->UnpinPage(root_page_id, true);
  //printf("StartNewTree %d\n", root_page_id);
}

INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::InsertIntoLeaf(const KeyType &key, const ValueType &value) {
  Page *leaf_ptr = FindLeafPage(key, false, INSERT_MODE);
  if(leaf_ptr == nullptr){
    std::cout<<"null == leaf_ptr\n";
    return false;
  }
  LeafPage *leaf_page = reinterpret_cast<LeafPage*>(leaf_ptr->GetData());
  //std::cout<<"find leaf key:" << key << "\n"; 
  int size = leaf_page->Insert(key, value, comparator_);
  //std::cout<<"size = "<<size<<"\n";
  if(size == leaf_max_size_){
   // std::cout<<"size == leaf_max_size key is:"<<key<<"\n";
    LeafPage* new_leaf_page = Split<LeafPage>(leaf_page);
    leaf_page->MoveHalfTo(new_leaf_page);
    new_leaf_page->SetNextPageId(leaf_page->GetNextPageId());
    leaf_page->SetNextPageId(new_leaf_page->GetPageId());
    InsertIntoParent(leaf_page, new_leaf_page->KeyAt(0), new_leaf_page);
    buffer_pool_manager_->UnpinPage(new_leaf_page->GetPageId(), true);
    
  }
  buffer_pool_manager_->UnpinPage(leaf_ptr->GetPageId(), true);
  leaf_ptr->SetDirty();
  return true;

}


INDEX_TEMPLATE_ARGUMENTS
template <typename N>
N *BPLUSTREE_TYPE::Split(N *node) {
  page_id_t new_page_id;
  auto page = buffer_pool_manager_->NewPage(&new_page_id);
  if (page == nullptr) {
    throw "out of memory";
  }
  N* type_n_page_ptr = reinterpret_cast<N*>(page->GetData());
  if (node->IsLeafPage()){
    type_n_page_ptr->Init(new_page_id, node->GetParentPageId(), leaf_max_size_);
  }else{
    type_n_page_ptr->Init(new_page_id, node->GetParentPageId(), internal_max_size_);
  }
  return type_n_page_ptr;
 
}


INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertIntoParent(BPlusTreePage *old_node, const KeyType &key, BPlusTreePage *new_node) {
 // printf("InsertIntoParent begin\n");
  if(old_node->IsRootPage()){
    page_id_t new_root_page_id;
    Page *new_root_ptr = SafelyNewPage(&new_root_page_id, "fail to create new root page");
    InternalPage *new_root_page = reinterpret_cast<InternalPage*>(new_root_ptr->GetData());
    new_root_page->Init(new_root_page_id, INVALID_PAGE_ID, internal_max_size_);
    old_node->SetParentPageId(new_root_page_id);
    new_node->SetParentPageId(new_root_page_id);
    new_root_page->PopulateNewRoot(old_node->GetPageId(), key, new_node->GetPageId());
    SetRootPageId(new_root_page_id);
    UpdateRootPageId(0);
    buffer_pool_manager_->UnpinPage(new_root_page_id, true);
    /*printf("old_node size:%d new_node size:%d\n", old_node->GetSize(), new_node->GetSize());
    printf("InsertIntoParent new root end\n");*/
    return;                             

  }
  page_id_t parent_page_id = old_node->GetParentPageId();
  Page *parent_ptr = SafelyGetFrame(parent_page_id, "fail to get parent page");
  InternalPage *parent_page = reinterpret_cast<InternalPage*>(parent_ptr->GetData());

  int size = parent_page->InsertNodeAfter(old_node->GetPageId(), key, new_node->GetPageId());

  if(size == internal_max_size_){
    InternalPage *new_parent_page = Split<InternalPage>((parent_page));
    parent_page->MoveHalfTo(new_parent_page, buffer_pool_manager_);
    InsertIntoParent(parent_page, new_parent_page->KeyAt(0), new_parent_page);
    buffer_pool_manager_->UnpinPage(new_parent_page->GetPageId(), true);
  }
  buffer_pool_manager_->UnpinPage(parent_page_id, true);
  //LOG_DEBUG("InsertIntoParent end");

}

INDEX_TEMPLATE_ARGUMENTS
Page* BPLUSTREE_TYPE::SafelyGetFrame(page_id_t page_id, const std::string &logout_string) {
  Page *page_ptr = buffer_pool_manager_->FetchPage(page_id);
  if (page_ptr == nullptr){
    //throw Exception(ExceptionType::OUT_OF_MEMORY, logout_string);
  }
  return page_ptr;
}


INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Remove(const KeyType &key) {

  
 
  if(IsEmpty()){
    return;
  }
  Page *leaf_ptr = FindLeafPage(key, false, DELETE_MODE);
  if(leaf_ptr == nullptr){
    return;
  }

  LeafPage *leaf_page = reinterpret_cast<LeafPage*>(leaf_ptr->GetData());


  int index = leaf_page->KeyIndex(key, comparator_);
  if(comparator_(leaf_page->KeyAt(index), key) != 0){
  	return;
  }

  leaf_page->RemoveAt(index);
  bool ret = false;
  if(leaf_page->GetMinSize() > leaf_page->GetSize()){
    ret = CoalesceOrRedistribute<LeafPage>(leaf_page);
  }
  if (ret){
    //buffer_pool_manager_->DeletePage(page_id);
  }else{
    leaf_ptr->SetDirty();
  }
}


INDEX_TEMPLATE_ARGUMENTS
template <typename N>
bool BPLUSTREE_TYPE::CoalesceOrRedistribute(N *node) {
  if(node->IsRootPage()){
    return AdjustRoot(node);
  }
  
  page_id_t parent_page_id;
  page_id_t prev_page_id = INVALID_PAGE_ID;
  page_id_t next_page_id = INVALID_PAGE_ID;
  Page *parent_page_ptr;
  Page *prev_page_ptr;
  Page *next_page_ptr;

  InternalPage *parent_page;

  N *prev_node;
  N *next_node;

  parent_page_id = node->GetParentPageId();
  parent_page_ptr = SafelyGetFrame(parent_page_id, "Out of memory in `CoalesceOrRedistribute`, get parent");
  parent_page = reinterpret_cast<InternalPage*>(parent_page_ptr->GetData());

  page_id_t page_id = node->GetPageId();

  
  int node_index = parent_page->ValueIndex(page_id);
  if(node_index > 0){
    prev_page_id = parent_page->ValueAt(node_index - 1);
    prev_page_ptr = SafelyGetFrame(prev_page_id, "Out of memory in CoalesceOrRedistribute");
    prev_node = reinterpret_cast<N*>(prev_page_ptr->GetData());
    if (prev_node->GetSize() > prev_node->GetMinSize()){
      Redistribute(prev_node, node, 1);

      buffer_pool_manager_->UnpinPage(parent_page_id, true);
      buffer_pool_manager_->UnpinPage(prev_page_id, true);
      return false;
    }
  }
 if(node_index < parent_page->GetSize() - 1){
    next_page_id = parent_page->ValueAt(node_index + 1);
    next_page_ptr = SafelyGetFrame(next_page_id, "Out of memory in CoalesceOrRedistribute");
    next_node = reinterpret_cast<N*>(next_page_ptr->GetData());
    if(next_node->GetSize() > next_node->GetMinSize()){
      Redistribute(next_node, node, 0);
      buffer_pool_manager_->UnpinPage(parent_page_id, true);
      buffer_pool_manager_->UnpinPage(next_page_id, true);
      return false;
    }
  }

  bool ret = false;
  if(prev_page_id != INVALID_PAGE_ID){
    ret = Coalesce(&prev_node, &node, &parent_page, node_index);
    buffer_pool_manager_->UnpinPage(parent_page_id, true);
    if (ret){
      //transaction->AddIntoDeletedPageSet(parent_page_id);
    }
    buffer_pool_manager_->UnpinPage(prev_page_id, true);
    if (next_page_id != INVALID_PAGE_ID){
      buffer_pool_manager_->UnpinPage(next_page_id, false);
    }
  }
  ret = Coalesce(&node, &next_node, &parent_page, node_index + 1);
  buffer_pool_manager_->UnpinPage(parent_page_id, true);
  buffer_pool_manager_->UnpinPage(next_page_id, true);
 // transaction->AddIntoDeletedPageSet(next_page_id);
  if (ret){
   //
   //transaction->AddIntoDeletedPageSet(parent_page_id);
  }
  return false;

  //LOG_DEBUG("CoalesceOrRedistribute end");
}


INDEX_TEMPLATE_ARGUMENTS
template <typename N>
bool BPLUSTREE_TYPE::Coalesce(N **neighbor_node, N **node,
                              BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator> **parent, int index){
 if((*node)->IsLeafPage()){
   LeafPage *leaf_node = reinterpret_cast<LeafPage*>(*node);
   LeafPage *neighbor_leaf_node = reinterpret_cast<LeafPage*>(*neighbor_node);
   leaf_node->MoveAllTo(neighbor_leaf_node);
 }else{
   InternalPage *internal_node = reinterpret_cast<InternalPage*>(*node);
   InternalPage *neighbor_internal_node = reinterpret_cast<InternalPage*>(*neighbor_node);
   KeyType middle_key = (*parent)->KeyAt(index);
   internal_node->MoveAllTo(neighbor_internal_node, middle_key, buffer_pool_manager_);
 }
 (*parent)->Remove(index);
  if ((*parent)->GetSize() < (*parent)->GetMinSize()){
    return CoalesceOrRedistribute(*parent);
  }                          
 return false;
}


INDEX_TEMPLATE_ARGUMENTS
template <typename N>
void BPLUSTREE_TYPE::Redistribute(N *neighbor_node, N *node, int index) {
  page_id_t parent_page_id = node->GetParentPageId();
  Page *parent_page_ptr = SafelyGetFrame(parent_page_id, "Out of memory in `Redistribute`");
  InternalPage *parent_page = reinterpret_cast<InternalPage*>(parent_page_ptr->GetData());
  if(node->IsLeafPage()){
    LeafPage *leaf_node = reinterpret_cast<LeafPage*>(node);
    LeafPage *neighbor_leaf_node = reinterpret_cast<LeafPage*>(neighbor_node);
    if(index == 0){
      neighbor_leaf_node->MoveFirstToEndOf(leaf_node);
      int node_index = parent_page->ValueIndex(neighbor_leaf_node->GetPageId());
      parent_page->SetKeyAt(node_index, neighbor_leaf_node->KeyAt(0));
    }else{
      neighbor_leaf_node->MoveLastToFrontOf(leaf_node);
      int node_index = parent_page->ValueIndex(leaf_node->GetPageId());
      parent_page->SetKeyAt(node_index, leaf_node->KeyAt(0));
    }
  }else{
    InternalPage *internal_node = reinterpret_cast<InternalPage*>(node);
    InternalPage *neighbor_internal_node = reinterpret_cast<InternalPage*>(neighbor_node);
    if(index == 0){
      int node_index = parent_page->ValueIndex(neighbor_internal_node->GetPageId());
      KeyType mid_key = parent_page->KeyAt(node_index);
      KeyType next_key = neighbor_internal_node->KeyAt(1);
      parent_page->SetKeyAt(node_index, next_key);
      neighbor_internal_node->MoveFirstToEndOf(internal_node, mid_key, buffer_pool_manager_);
    }else{
      int node_index = parent_page->ValueIndex(internal_node->GetPageId());
      KeyType mid_key = parent_page->KeyAt(node_index);
      KeyType next_key = neighbor_internal_node->KeyAt(neighbor_internal_node->GetSize() - 1);
      parent_page->SetKeyAt(node_index, next_key);
      neighbor_internal_node->MoveLastToFrontOf(internal_node, mid_key, buffer_pool_manager_);  
    }
    buffer_pool_manager_->UnpinPage(parent_page_id, true);
  }
}

INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::AdjustRoot(BPlusTreePage *old_root_node) {
  if(old_root_node->GetSize() > 1){
    return  false;
  }

  page_id_t new_root_id = INVALID_PAGE_ID;
  if(old_root_node->IsLeafPage()){
    if (old_root_node->GetSize() == 1){
      return false;
    }
  }else{
    InternalPage *old_root_page = reinterpret_cast<InternalPage*>(old_root_node);
    new_root_id = old_root_page->RemoveAndReturnOnlyChild();
    Page* new_root_page_ptr = SafelyGetFrame(new_root_id, "Out of memory in `AdjustRoot");
    LeafPage* new_root_page = reinterpret_cast<LeafPage*>(new_root_page_ptr->GetData());
    new_root_page->SetParentPageId(INVALID_PAGE_ID);
    buffer_pool_manager_->UnpinPage(new_root_id, true);
  }
  root_page_id_ = new_root_id;
  UpdateRootPageId(0);
  return true;
}


INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_TYPE::begin() { 
  Page* left_page_ptr = FindLeafPage(KeyType(), true, 0);
  return INDEXITERATOR_TYPE(left_page_ptr, 0, buffer_pool_manager_);
  
}

/*
 * Input parameter is low key, find the leaf page that contains the input key
 * first, then construct index iterator
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_TYPE::Begin(const KeyType &key) { 
  Page* page_ptr = FindLeafPage(key, false, 0);
  IN_TREE_LEAF_PAGE_TYPE* leaf_ptr =
      reinterpret_cast<IN_TREE_LEAF_PAGE_TYPE*>(page_ptr->GetData());

  int index = leaf_ptr->KeyIndex(key, comparator_);

  return INDEXITERATOR_TYPE(page_ptr, index, buffer_pool_manager_);
 }


INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_TYPE::end() { 
  return INDEXITERATOR_TYPE(nullptr, 0, buffer_pool_manager_);

 }



INDEX_TEMPLATE_ARGUMENTS
Page* BPLUSTREE_TYPE::SafelyNewPage(page_id_t *page_id, const std::string &logout_string) {
  Page* new_page_ptr = buffer_pool_manager_->NewPage(page_id);
  if (new_page_ptr == nullptr){
    //throw Exception(ExceptionType::OUT_OF_MEMORY, logout_string);
    return nullptr;
  }
  //printf("new page %d\n", *page_id);
  return new_page_ptr;
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::DeletePages() {
  /*const auto set_ptr = transaction->GetDeletedPageSet();
  for (const auto &page_id : *set_ptr){
    buffer_pool_manager_->DeletePage(page_id);
  }
  set_ptr->clear();*/
 
}
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::SetRootPageId(int root_page_id) {
  root_page_id_ = root_page_id;
}



INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::UpdateRootPageId(int insert_record) {
  /*HeaderPage *header_page = static_cast<HeaderPage *>(buffer_pool_manager_->FetchPage(HEADER_PAGE_ID));
  if (insert_record != 0) {
    // create a new record<index_name + root_page_id> in header_page
    header_page->InsertRecord(index_name_, root_page_id_);
  } else {
    // update root_page_id in header_page
    header_page->UpdateRecord(index_name_, root_page_id_);
  }
  buffer_pool_manager_->UnpinPage(HEADER_PAGE_ID, true);*/
  
}



/*
 * This method is used for test only
 * Read data from file and insert one by one
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertFromFile(const std::string &file_name) {
  int64_t key;
  std::ifstream input(file_name);
  while (input) {
    input >> key;

    KeyType index_key;
    index_key.SetFromInteger(key);
    RID rid(key);
    Insert(index_key, rid);
  }
}
/*
 * This method is used for test only
 * Read data from file and remove one by one
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::RemoveFromFile(const std::string &file_name) {
  int64_t key;
  std::ifstream input(file_name);
  while (input) {
    input >> key;
    KeyType index_key;
    index_key.SetFromInteger(key);
    Remove(index_key);
  }
}



template class BPlusTree<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTree<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTree<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTree<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
