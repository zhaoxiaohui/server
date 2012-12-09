/**
*系统日记记录文件
*@date:20121126
*@author:firebird
*/
#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <boost/asio.hpp>
#include <boost/bind.hpp>
//#include <boost/enable_shared_from_this.hpp>
//#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <pthread.h>
#define DIRECTORY "logs"
#define PERWRITE 30
#define MESSAGESIZE 2000
using namespace std;
using boost::system::error_code;
namespace http{
    namespace server3{
	class Log{
        public:
            /*记录日志*/
            void record(string message);
			static Log* getInstance(boost::asio::io_service& io_service_, std::string& log_fullname);
            //static Log* clog;
            ~Log();
        private:
            /*单例模式*/
            //Log(boost::asio::io_service& io_service);
			Log(boost::asio::io_service& io_service_, std::string& log_fullname);
			/*获得系统当前时间 年-月-日 时:分:秒*/
			string getCurTime();
            /*取得当前要往哪个文件写入内容 文件名字以当天的日期为准*/
            string getFileName();
			///记录缓存
			void record_();
            /*检查当前要写入的文件是否已经存在*/
            int checkOrCreate();
            /*当前工作路径*/
            boost::filesystem::path cur_path;
            std::string log_fullname_;
			boost::asio::strand strand_;
            /*当前的输出流*/
            ofstream fout;
			/*每当100次请求之后 主动更新文件*/
			//int req_num;
			/*设定自动回写时间 60S*/
			boost::asio::deadline_timer write_timer;
			void write_back(const error_code& e);

            //写入锁
            pthread_mutex_t write_mutex;
			///缓存机制
			vector<std::string> messages;
    };//end class
    }//end namespace server3
}//end namespace http
#endif// __LOG_HP__
