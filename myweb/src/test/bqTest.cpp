#include"blockdeque.h"
#include<iostream>
#include<thread>
int main(){
	BlockDeque<int> q(100000);
	std::thread t1([&q]{
		while(1){
			while(!q.empty()){
		
			int b; 
			q.pop(b);
			std::cout<<b<<"\n";
			}
		}
	});
	for(int i = 0; i < 100000; i++){
		std::cout<<"push"<<i<<"\n";
		q.push_back(i);
	}
	
	t1.join();
	for(int i = 0; i < 100000; i++){
		std::cout<<"push"<<i<<"\n";
		q.push_back(i);
	}
}
