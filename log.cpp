#include <sys/timeb.h>
#include "log.hpp"

namespace http{
    namespace server3{
        
        Log::Log()/*:
			write_timer(io_service),
			strand_(io_service)*/{
            /*如果当前文件目录下 不存在logs目录 则创建*/
            cur_path = boost::filesystem::current_path();
            boost::filesystem::path bfp = cur_path / DIRECTORY;
            cur_path = bfp;
            if(!boost::filesystem::exists(bfp) || !boost::filesystem::is_directory(bfp)){
                    boost::filesystem::create_directory(bfp);
            }
			//req_num = 0;
            //std::cout<<"create log\n";
			/**
			write_timer.expires_from_now(boost::posix_time::seconds(60));
			write_timer.async_wait(
				strand_.wrap(boost::bind(&Log::write_back,
					this, boost::asio::placeholders::error)));
			
			*/
        }
		/**
		void Log::write_back(const error_code& e){
			if(fout){
				//fout.close();
				std::cout << "record message\n"; 
			}
			write_timer.expires_from_now(boost::posix_time::seconds(60));
			write_timer.async_wait(
				strand_.wrap(boost::bind(&Log::write_back,
					this, boost::asio::placeholders::error)));
		}
		*/
        Log::~Log(){
            /*关闭文件流*/
			if(messages.size() > 0)record_();
            if(fout)fout.close();
        }
            
        Log* Log::getInstance(){
            static Log clog;// = new Log(io_service);
            //if(!clog)
              //  clog = new Log();
            return &clog;
        }
        void Log::record(string message){
            std::cout << "@@@@@" << message << "\n";
            //req_num++;
			//if(req_num == 50){
			//	req_num = 0;
			//	strand_.wrap(boost::bind(&Log::write_back, this, boost::asio::placeholders::error));
			//}
			std::string filename = getFileName();
			int coc = checkOrCreate(filename);
            if(coc){
				messages.push_back(getCurTime() + " " + message);
				if(messages.size() >= 50){
					std::cout<<"record\n";
					record_();
				}
                //std::cout<<message<<"\n";
            }else{
                std:cerr<<"Error:record failed\n";
            }

        }
		void Log::record_(){
			vector<string>::iterator m_it;
			for(m_it=messages.begin(); m_it!=messages.end(); m_it++){
				fout << *m_it << "\n";
			}
			fout << flush;
			messages.clear();
		}
		/**
		int getNum(std::string filename){
			int res = 0;
			//&&.txt
			int len = filename.length() - 4;
			if(len == 0)return 0;
			while(len &&filename[len] != '_'){
				res = res*10 + filename[len] - '0';
				len--;
			}
			return res;
		}
		int findTheLastFile(boost::filesystem::path curp){
			int res = 0;
			boost::filesystem::directory_iterator dir_iter(curp),dir_end;
			for(; dir_iter!=dir_end; dir_iter++){
				res = getNum(dir_iter->leaf().string());
			}
			return res;

		}
		*/
       	int Log::checkOrCreate(string filename){
            boost::filesystem::path bfp = cur_path / filename;
            if(boost::filesystem::exists(bfp)){
                if(!fout){/*文件还没有打开*/
                    fout.open(bfp.string().c_str(), ios::app);
                    //std::cout << "open file" <<"\n";
                }
                return 1;//exist
            }else{
                //boost::filesystem::create_directory(bfp);
				if(fout){
					if(messages.size() > 0)record_();
					fout.close();
				}
                /*创建新的文件的时候 就设置输出流*/
                fout.open(bfp.string().c_str(), ios::app);
                return 2;//new
            }
        }

		string Log::getCurTime(){
			struct tm now_t;         //实例化tm结构指针
			struct timeb tp;
            ftime(&tp);
			localtime_r(&tp.time, &now_t);
			char cur[25];
			sprintf(cur,"%04d-%02d-%02d %02d:%02d:%02d.%03d",now_t.tm_year+1900, now_t.tm_mon+1, now_t.tm_mday, now_t.tm_hour,\
							now_t.tm_min, now_t.tm_sec, tp.millitm);
			string res = cur;
			return res;
		}
		/**
		char *gettime(char *msg)
		{
			struct timeb tp;
			struct tm ltime;
			ftime(&tp);
			localtime_r(&tp.time,&ltime);
			sprintf(msg,"%04d-%02d-%02d %02d:%02d:%02d.%03d %d",
			ltime.tm_year+1900,
			ltime.tm_mon+1,
			ltime.tm_mday,
			ltime.tm_hour,
			ltime.tm_min,
			ltime.tm_sec,
			tp.millitm,
			ltime.tm_wday);
			return msg;
		}*/
        string Log::getFileName(){
            struct tm *now_t;
			time_t cur_t;
            cur_t = time(NULL);
			now_t = localtime(&cur_t);
            char cur[15];
            sprintf(cur, "%04d%02d%02d.txt", now_t->tm_year+1900, now_t->tm_mon+1, now_t->tm_mday);
            string res = cur;
            return res;
        }


    }//end namespace server3
}//end namespace http
