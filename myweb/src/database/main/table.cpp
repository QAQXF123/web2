namespace bustub{
	struct TableInfo{
		page_id_t tableFirstPageId;
		page_id_t indexRootPageId;	
        page_id_t tableLastPageId;		
	};
    
    class Table{
    public:
        Table(bool isCreate, const std::string &indexPath, const std::string &tablePath,
        const std::vector<std::tuple<std::string, int, int>>& tableInfos, size_t bpmSize = 5000){
         //   printf("table begin\n");
            tableDiskManager_ = new DiskManager(tablePath);
            indexDiskManager_ = new DiskManager(indexPath);
            indexBPM_ = new BufferPoolManager(bpmSize, indexDiskManager_);
            tableBPM_ = new BufferPoolManager(bpmSize, tableDiskManager_);

            //GenericKey<64> index_key;
            std::vector<Column> cols;
            Column col("key", TypeId::BIGINT);
            cols.push_back(col);
            Schema *indexSchema = new Schema(cols);
            GenericComparator<64> comparator(indexSchema);
            std::string indexName = "indexName";
            tree_ = new BPlusTree<GenericKey<64>, RID, GenericComparator<64>>(indexName, indexBPM_, comparator);
           
            
            //make table schema


            //tableinfo: colname-coltype-collen
            cols.clear();
            for(const auto &tableInfo : tableInfos){
                TypeId typeId = GetTypeId(std::get<1>(tableInfo));
                if(typeId == TypeId::VARCHAR){
                    cols.push_back({std::get<0>(tableInfo), typeId, static_cast<uint32_t>(std::get<2>(tableInfo))});
                }else{
                    cols.push_back({std::get<0>(tableInfo), typeId});
                }
            }
            //Schema tableSchema(cols);
            tableSchema_ = new Schema(cols);
            

            if(isCreate){
                page_id_t pageId;
                
                tableHeap_ = new TableHeap(tableBPM_);
                auto tableMetadataPage = tableBPM_->NewPage(&pageId);
                TableInfo *tableInfo = reinterpret_cast<TableInfo*>(tableMetadataPage);
                tableInfo->tableFirstPageId = tableHeap_->GetFirstPageId();
                tableInfo->indexRootPageId = tree_->GetRootPageId();
                tableInfo->tableLastPageId = tableHeap_->GetFirstPageId();

                indexRootPageId_ = tree_->GetRootPageId();
                tableLastPageId_ = tableHeap_->GetLastPageId();

                tableBPM_->UnpinPage(pageId, true);
                //printf("page root id %d\n", indexRootPageId_);
                //printf("table first id %d\n", tableHeap_->GetFirstPageId());
                tableBPM_->FlushAllPages();
                indexBPM_->FlushAllPages();   
            }else{
                auto tableMetadataPage = tableBPM_->FetchPage(0);
                tableBPM_->UnpinPage(0, false);
                TableInfo *tableInfo = reinterpret_cast<TableInfo*>(tableMetadataPage);
                tree_->SetRootPageId(tableInfo->indexRootPageId);
                tableHeap_ = new TableHeap(tableBPM_, tableInfo->tableFirstPageId, tableInfo->tableLastPageId);
                /*printf("indexRootPageId :%d, tableFirstPageid :%d, tableLastPageId:%d\n", 
                tableInfo->indexRootPageId, tableInfo->tableFirstPageId, tableInfo->tableLastPageId);*/
            }
            
        }

        void ChangeTableInfo(page_id_t newIndexRootPageId, page_id_t newTableLastPageId){
            auto tableMetadataPage = tableBPM_->FetchPage(0);
            TableInfo *tableInfo = reinterpret_cast<TableInfo*>(tableMetadataPage);
            tableInfo->indexRootPageId = newIndexRootPageId;
            tableInfo->tableLastPageId = newTableLastPageId;
            indexRootPageId_ = newIndexRootPageId;
            tableLastPageId_ = newTableLastPageId;
            tableBPM_->UnpinPage(0, true);
        }

        TypeId GetTypeId(int id){
            TypeId typeId;
            switch (id)
                {
                case 1:
                    typeId = TypeId::INTEGER;
                    break;
                case 2:
                    typeId = TypeId::BIGINT;
                    break;
                case 3:
                    typeId = TypeId::VARCHAR;
                    break;
                default:
                    typeId = TypeId::INVALID;
                }
            return typeId;

        }

        bool Insert(const std::vector<std::string> &colValues){
            GenericKey<64> index_key;
            RID rid;
            auto cols = tableSchema_->GetColumns();
            int n = colValues.size();
          //  std::cout<<"n="<<n<<", cols size:"<<cols.size()<<"\n";
            assert(n == (int)cols.size());
            
            std::vector<Value> values;
            values.reserve(n);
            
            for(int i = 0; i < n; i++){
                TypeId typeId = cols[i].GetType();
                switch (typeId)
                {
                case TypeId::INTEGER:
                    values.push_back({TypeId::INTEGER, stoi(colValues[i])});  
                    break;
                case TypeId::BIGINT:
                    values.push_back({TypeId::BIGINT, stoul(colValues[i])});  
                    break;
                case TypeId::VARCHAR:
                    values.push_back({TypeId::VARCHAR, colValues[i]});   
                    break;
                default:    
                    break;
                }
            }
            Tuple tuple(values, tableSchema_);
            bool tableInsert = tableHeap_->InsertTuple(tuple, &rid);
            if(!tableInsert) return false;
            index_key.SetFromInteger(Value::GetBigInt(values[0]));
            bool treeInser = tree_->Insert(index_key, rid);
            if(!treeInser) return false;

            page_id_t newIndexRootPageId = tree_->GetRootPageId();
            page_id_t newTableLastPageId = tableHeap_->GetLastPageId();
            if(indexRootPageId_ != newIndexRootPageId || tableLastPageId_ != newTableLastPageId){
                ChangeTableInfo(newIndexRootPageId, newTableLastPageId);
            }
            return true;
        }

        
        bool GetTuple(uint64_t key, Tuple *tuple){
            
            
            std::vector<RID> rids;
            bool treeGet = GetRidsByKey(key, &rids);
            if(!treeGet) return false;
            bool tableGet = tableHeap_->GetTuple(rids[0], tuple);
            if(!tableGet) return false;

         //   auto schema = tableSchema_;
           /* printf("get begin\n");
			auto v1 = tuple->GetValue(schema, 0);
        	auto v2 = tuple->GetValue(schema, 1);
        	auto v3 = tuple->GetValue(schema, 2);
        	std::cout<<"v1:"<<v1.ToString()<<"\n";
        	std::cout<<"v2:"<<Value::GetCString(v2)<<"\n";
        	std::cout<<"v3:"<<Value::GetCString(v3)<<"\n";*/
            return true;
        }

        bool GetRidsByKey(uint64_t key, std::vector<RID> *rids){
            GenericKey<64> index_key;
            index_key.SetFromInteger(key);
            bool treeGet = tree_->GetValue(index_key, rids);
            return treeGet;
        }

        void RemoveRIdByKey(uint64_t key){
            GenericKey<64> index_key;
            index_key.SetFromInteger(key);
            tree_->Remove(index_key);
        }

        
        bool Delete(uint64_t key){
          //  printf("delete key:%uld begin \n", key);
            std::vector<RID> rids;
            bool treeGet = GetRidsByKey(key, &rids);
            if(!treeGet){
                return false;
            //    printf("why not delete\n");
            }
            assert(rids.size() == 1);
            RemoveRIdByKey(key);
            tableHeap_->ApplyDelete(rids[0]);
            return true;
        }

        bool GetValue(uint64_t key, const std::string &colName, std::string &v){
            Tuple tuple;
            bool getTupleSuccess = GetTuple(key, &tuple);
            if(!getTupleSuccess) return false;
            int colIdx = tableSchema_->GetColIdx(colName);
		    if(colIdx == -1) return false;
            auto value = tuple.GetValue(tableSchema_, colIdx);
            
            v = value.ToString();
          //  printf("str:%s", str);
           // printf("table getvalue %s\n", v.c_str());
            return true;
        }
       
        Schema *GetTableSchema() const{
            return tableSchema_;

        }
        void Flush(){
            indexBPM_->FlushAllPages();
            tableBPM_->FlushAllPages();
        }
    private:
        
        bool isClosed_;
        DiskManager *tableDiskManager_;
        DiskManager *indexDiskManager_;
        BufferPoolManager *indexBPM_;
        BufferPoolManager *tableBPM_;
        Schema *tableSchema_;
        TableHeap *tableHeap_;
        BPlusTree<GenericKey<64>, RID, GenericComparator<64>> *tree_;
        page_id_t indexRootPageId_;
        page_id_t tableLastPageId_;
    };

}
