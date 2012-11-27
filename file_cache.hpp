/**
 *文件缓存
 *
 *对于访问的文件进行缓存处理，缓存大小限定为300M
 *当缓存满时 采用LRU算法移除
 *@author:firebird
 *@date:20121127
 */

#ifndef __HTTP_SERVER3_FILE_CACHE_HPP__
#define __HTTP_SERVER3_FILE_CACHE_HPP__

#include <iostream>
#include <map>
#include "reply.hpp"

#define CACHE_MAXSIZE 300*1024

namespace http{
	namespace server3{
		///存储的内容结构
		//包括计数 和 reply等
		typedef struct M_CONTENT{
			int   count;           //计数器
			reply *reply_;        //服务器回复的内容
		}m_content;
		
		///缓存类 单例模式
		class file_cache{
			public:
				static file_cache* getInstance();
				~file_cache();
				void insert(std::string key, reply *value);
				reply* hasKey(std::string);
			private:
				file_cache();
				void remove();//采用LRU删除
				int size_;
				std::map<std::string, m_content*> m_cache;
		};
	}
}

#endif //end ifndef
