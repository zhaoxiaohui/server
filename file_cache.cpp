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
			int maxcount = 0;
			for(it=m_cache.begin(); it!=m_cache.end(); it++){
				if(it->second->count > maxcount){
					maxcount = it->second->count;
					rit = it;
				}
			}
			if(rit != m_cache.end()){
				//重新确定size大小
				size_ -= rit->second->reply_->content.size();
				delete rit->second->reply_;
				delete rit->second;
				m_cache.erase(rit);
			}
		}

		void file_cache::insert(std::string key, reply *value){
			
			std::map<std::string, m_content*>::iterator it;
            std::map<std::string, m_content*>::iterator iit;
			it = m_cache.find(key);
			if(it != m_cache.end()){
                it->second->count = 0;
				for(iit = m_cache.begin(); iit != m_cache.end(); iit++)
                    iit->second->count++;
				return;
			}
            int st = value->content.size();
			while(size_ + st >= CACHE_MAXSIZE)
				remove();
			
			m_content *mc = new m_content();//(m_content *)malloc(sizeof(m_content));
			if(!mc){
				std::cerr<<"insert error for malloc failed\n";
				return;
			}
			mc->count = 0;
			mc->reply_ = value;
            size_ += st;
			m_cache.insert(std::pair<std::string, m_content*>(key,mc));
            for(iit = m_cache.begin(); iit != m_cache.end(); iit++)
                    iit->second->count++;
		}
	
	}
}
