#include "file_cache.hpp"

namespace http{
	namespace server3{

		file_cache::file_cache(){
			size_ = 0;
		}

		file_cache* file_cache::getInstance(){
			static file_cache fc;
			return &fc;
		}
		
		file_cache::~file_cache(){
			m_cache.clear();
		}	
		
		reply* file_cache::hasKey(std::string key){
			std::map<std::string, m_content*>::iterator it;
			it = m_cache.find(key);
			if(it != m_cache.end()){
				return it->second->reply_;
			}
			return NULL;
		}

		void file_cache::remove(){
			std::map<std::string, m_content*>::iterator it;
			std::map<std::string, m_content*>::iterator rit;
			int mincount = 0x7fffffff;
			for(it=m_cache.begin(); it!=m_cache.end(); it++){
				if(it->second->count < mincount){
					mincount = it->second->count;
					rit = it;
				}else{//如果其他引用大于当前最小值 那么重新赋值为1
					it->second->count = 1;
				}
			}
			if(rit != m_cache.end()){
				//重新确定size大小
				size_ -= rit->second->reply_->content.size();
				free(rit->second->reply_);
				free(rit->second);
				m_cache.erase(rit);
			}
		}

		void file_cache::insert(std::string key, reply *value){
			
			std::map<std::string, m_content*>::iterator it;
			it = m_cache.find(key);
			if(it != m_cache.end()){
				it->second->count++;
				return;
			}

			while(size_ >= CACHE_MAXSIZE)
				remove();
			
			m_content *mc = (m_content *)malloc(sizeof(m_content));
			if(!mc){
				std::cerr<<"insert error for malloc failed\n";
				return;
			}
			mc->count = 1;
			mc->reply_ = value;

			m_cache.insert(std::pair<std::string, m_content*>(key,mc));
		}
	
	}
}
