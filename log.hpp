/**
*系统日记记录文件
*@date:20121126
*@author:firebird
*/
#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define DIRECTORY "logs"
using namespace std;

namespace http{
    namespace server3{
	class Log{
        public:
            /*记录日志*/
            void record(string message);
			static Log* getInstance();
            //static Log* clog;
            ~Log();
        private:
            /*单例模式*/
            Log();
			/*获得系统当前时间 年-月-日 时:分:秒*/
			string getCurTime();
            /*取得当前要往哪个文件写入内容 文件名字以当天的日期为准*/
            string getFileName();
            /*检查当前要写入的文件是否已经存在*/
            bool checkOrCreate(string fileName);
            /*当前工作路径*/
            boost::filesystem::path cur_path;
            /*当前的输出流*/
            ofstream fout;
    };//end class
    }//end namespace server3
}//end namespace http
#endif// __LOG_HP__
