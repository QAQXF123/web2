#include<map>
#include<unordered_map>
#include<set>
#include <assert.h> 
#include <chrono>
#include<functional>
#include<iostream>
using TimeOutCallBack = std::function<void()>;
using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::milliseconds;
using TimeStamp = Clock::time_point;

struct TimerNode{
	int id;
	TimeStamp time;
	TimeOutCallBack cb;
	bool operator<(const TimerNode &rhs) const {
		return time < rhs.time || (time == rhs.time && id < rhs.id);
	}
	TimerNode(int id, TimeStamp time, TimeOutCallBack &cb):id(id), time(time), cb(cb){}
	~TimerNode(){
	//	std::cout<<id<<"~\n"; 
	}
};
class Timer{
	public:
		Timer() = default;
		~Timer(){
			
		}
		
		bool size() const {
			return set_.size();
		}
		bool empty() const { 
			return set_.empty();
		}
		
		void add(int id, int timeout, TimeOutCallBack &&cb){
			if(getTimer_.count(id)){
				auto it = getTimer_.find(id)->second;
				TimerNode node = std::move(*it);
				set_.erase(getTimer_[id]);
				node.time = Clock::now() + MS(timeout);
				node.cb = cb;
				set_.insert(node);
				it = set_.find(node);
				getTimer_[id] = it;
			}else{
				TimerNode *node = new TimerNode(id, Clock::now() + MS(timeout), cb);
				set_.insert(*node);
				auto it = set_.find(*node);
				getTimer_[id] = it;	 
			}
		}
		void adjust(int id, int timeout){
			auto it = getTimer_.find(id)->second;
			TimerNode node = std::move(*it);
			set_.erase(it);
			node.time = Clock::now() + MS(timeout);
			set_.insert(node);
			auto nodeIt = set_.find(node);
			getTimer_[id] = nodeIt;
		}
	
		void tick(){
			while(!set_.empty()){
				TimerNode node = *set_.begin();
				if(std::chrono::duration_cast<MS>(node.time - Clock::now()).count() > 0) { 
            		break; 
        		}
        		node.cb();
        		set_.erase(set_.begin());
        		getTimer_.erase(node.id);
        		//delete &node;
			}	
		}
		int GetNextTick(){
			tick();
			int res = -1;
			if(!set_.empty()){
				res = std::chrono::duration_cast<MS>((*set_.begin()).time - Clock::now()).count();
			}
			return res > 0 ? res : 0;
		}
		
	private:
		std::set<TimerNode> set_;
		std::unordered_map<int, std::set<TimerNode>::iterator> getTimer_;
		
		
}; 
