#pragma once
#include<fstream>
#include<unordered_map>
#include<vector>
#include<iostream>
#include <thread>
#include <chrono>
#include <stdlib.h>



namespace bustub{
class Tuple;
class Table;
class TableManager{
	public:
		TableManager();
		~TableManager() = default;
		Table *GetTable(const std::string &tableName);
		bool Insert(const std::string& tableName, const std::vector<std::string> &values);
		bool GetTuple(const std::string& tableName, uint64_t key, Tuple *tuple);
		bool GetValue(const std::string& tableName, uint64_t key, const std::string &colName, std::string &value);
		//void PrintTupleValue(const Tuple &tuple, const Schema *schema);

		//test
		void test1();
		void test2();
		void test3();
		void test4(const std::vector<int>& rands);

		//help fun
		std::vector<int> GetRands(int n){
			std::vector<int> res(n);
			for(int i = 0; i < n; i++){
				res[i] = i;
			}
			for(int i = n - 1; i >= 0; i--){
				int t = rand() % (i + 1);
				std::swap(res[i], res[t]);
			}
			return res;

		}
	
	
	private:
		std::unordered_map<std::string, Table*> getTable_;
		std::vector<std::string> tableInfos;
		std::thread *flushThread;

};

}