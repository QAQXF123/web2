//lock
#include"../a-include/common/rwlock.h"
//disk
#include"../storage/disk/disk_manager.cpp"
//buffer
#include"../buffer/buffer_pool_manager.cpp"
#include"../buffer/lru_replacer.cpp"

//index
#include"../storage/index/b_plus_tree.cpp"
#include"../storage/index/index_iterator.cpp"

//page
#include"../storage/page/b_plus_tree_internal_page.cpp"
#include"../storage/page/b_plus_tree_leaf_page.cpp"
#include"../storage/page/b_plus_tree_page.cpp"
#include"../storage/page/header_page.cpp"
#include"../storage/page/table_page.cpp"

//table
#include"../storage/table/tuple.cpp"
#include"../storage/table/table_heap.cpp"
#include"../storage/table/table_iterator.cpp"
#include"../storage/table/table_generator.cpp"
//catalog
#include"../catalog/column.cpp"
#include"../catalog/schema.cpp"
#include"../a-include/catalog/catalog.h"

//type
#include"../type/type.cpp"
#include"../type/value.cpp"
#include"../type/integer_parent_type.cpp"
#include"../type/integer_type.cpp"
#include"../type/bigint_type.cpp"
#include"../type/varlen_type.cpp"

#include"table.cpp"
#include"table_manager.h"
namespace bustub{
	TableManager::TableManager(){
		auto src = getcwd(nullptr, 256);
		printf("src%s ======\n", src);
		//tablename:indxepath:tablepath:colnums: col1Name:col1Type:col1Len     :create
		//user:url/a:url/b:3:usernum:2:0:username:3:20:pwd:3:20:0
		std::ifstream ifs("data/tablelist.txt", std::ios::in);
		std::string line;
		if(!ifs){
			std::cout<<"fail to open table/tablelist\n";
			return;
		}
		while(ifs >> line){
			tableInfos.push_back(line);
			//printf("%s\n\n\n\n\n\n", line.c_str());
			line += ':';
			std::vector<std::string> info;
			std::string t;
			for(char i : line){
				if(i == ':'){
					info.push_back(t);
					t.clear();
				}else{
					t += i;	
				}
			}
			int infoSzie = info.size() - 1;
			std::vector<std::tuple<std::string, int, int>> tableInfoTuples;
			for(int i = 4; i < infoSzie; i += 3){
				//colname:coltype:colsize
				//std::cout<<info[i] << "-" << info[i+1] << "-" << info[i+2]<<"\n";
				tableInfoTuples.push_back(make_tuple(info[i], stoi(info[i+1]), stoi(info[i+2])));
			}
			
			
			bool isCreate = info.back() == "0";
			
			Table *table = new Table(isCreate, info[1], info[2], tableInfoTuples);
			getTable_[info[0]] = table;
		}	
		ifs.close();

		std::ofstream ofs("data/tablelist.txt", std::ios::out);
		if(!ofs){
			std::cout<<"fail to open table/tablelist in out";
		}
		//printf("write back to info\n\n\n");
		for(auto &info : tableInfos){
			info.back() = '0';
			info += '\n';
			ofs<<info;
		}
		ofs.close();

		//创建一个线程把内存池中脏页持久化
		flushThread = new std::thread([&](){
			while(1){
				for(auto t : getTable_){
					auto table = t.second;
					table->Flush();
				}
				std::this_thread::sleep_for(std::chrono::seconds(5));
			}
		});
	}
	
	bool TableManager::Insert(const std::string &tableName, const std::vector<std::string> &values){
		auto table = GetTable(tableName);
		bool insertSuccess = table->Insert(values);
		return insertSuccess;
	}

	bool TableManager::GetTuple(const std::string &tableName, uint64_t key, Tuple *tuple){
		auto table = getTable_[tableName];
		bool getSuccess = table->GetTuple(key, tuple);
		return getSuccess;
	}

	bool TableManager::GetValue(const std::string& tableName, uint64_t key, const std::string &colName, std::string &value){
		auto table = getTable_[tableName];
		bool getSuccess = table->GetValue(key, colName, value);
		//printf("tm getvalue %s\n", value.c_str());
		return getSuccess;

	}

	Table* TableManager::GetTable(const std::string &name){
		if(!getTable_[name]){
			std::string msg = "no table named" + name;
			throw msg.c_str();
			//return nullptr;
		}
		return getTable_[name];
		
	}

	


	/* test  */

	//help fun

	//test fun
	int test_n = 1e2;

	/*void TableManager::PrintTupleValue(const Tuple &tuple, const Schema *schema){
		auto v1 = tuple.GetValue(schema, 0);
        auto v2 = tuple.GetValue(schema, 1);
        auto v3 = tuple.GetValue(schema, 2);
        std::cout<<"v1:"<<v1.ToString()<<"\n";
        std::cout<<"v2:"<<Value::GetCString(v2)<<"\n";
        std::cout<<"v3:"<<Value::GetCString(v3)<<"\n";
	}*/

	void TableManager::test1(){
		auto userTable = GetTable("user");   
        for(int i = 0; i < test_n; i++){
			std::vector<std::string> values;
			std::string a = "aaaaabbbbbaaaaabbbbaaaa";
			std::string b = "aaaaabbbbbaaaa";
			values.push_back(std::to_string(i));
			values.push_back(a);
			values.push_back(b);
			userTable->Insert(values);	
		} 
        userTable->Flush();
        /*std::vector<int> notGet;
        Tuple tuple;
		for(int i = 0; i < test_n; i++){
			auto t = userTable->GetTuple(i, &tuple);
			if(!t){
				notGet.push_back(i);
				continue;	
			}
			
		}
		for(auto i : notGet){
			throw std::runtime_error("get fail after inser\n");
			std::cout<<i<<"\n";
		}*/
	}

	void TableManager::test2(){
		std::cout<<"table test2 begi\n";
		auto userTable = GetTable("user");   
		std::vector<int> notGet;
		Tuple tuple;
		for(int i = 0; i < test_n; i++){
			bool t = userTable->GetTuple(i, &tuple);
			if(!t){
				notGet.push_back(i);
				continue;	
			}
			auto schema = userTable->GetTableSchema();
			auto v1 = tuple.GetValue(schema, 0);
        	auto v2 = tuple.GetValue(schema, 1);
        	auto v3 = tuple.GetValue(schema, 2);
       	 	std::cout<<"v1:"<<v1.ToString()<<"\n";
        	std::cout<<"v2:"<<Value::GetCString(v2)<<"\n";
        	std::cout<<"v3:"<<Value::GetCString(v3)<<"\n";
		}
	
		if(!notGet.empty()){
			for(auto i : notGet){
				printf("not get %d\n", i);
			}
			throw std::runtime_error("fail in test2\n");
		}
		std::cout<<"table tes2 end\n\n\n\n";
	}

	void TableManager::test3(){
		std::cout<<"delete test begin\n";
		auto table = GetTable("user");
		std::vector<int> notDelete;
		test1();
		for(int i = 0; i < test_n; i+=3){
			table->Delete(i);
		}
		test2();
		test1();
		for(int i = 0; i < test_n; i++){
			table->Delete(i);
		}
		test1();
		test2();
		assert(notDelete.empty());
	}

	void TableManager::test4(const std::vector<int>& rands){
		std::cout<<"rand insert test begin\n";
		int n = rands.size();
		auto userTable = GetTable("user");   
        for(int i = 0; i < n; i++){
			std::vector<std::string> values;
			std::string a = "abc";
			std::string b = "hhh";
			values.push_back(std::to_string(rands[i]));
			values.push_back(a);
			values.push_back(b);
			userTable->Insert(values);	
		} 
        userTable->Flush();
        std::vector<int> notGet;
        Tuple tuple;
		for(int i = 0; i < test_n; i++){
			auto t = userTable->GetTuple(i, &tuple);
			if(!t){
				notGet.push_back(i);
				continue;	
			}
			
		}
		for(auto i : notGet){
			std::cout<<i<<"\n";
			throw std::runtime_error("get fail after inser in test4\n");	
		}

	}

	
}


