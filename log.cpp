#include "log.hpp"

namespace http{
    namespace server3{
        
        Log::Log(){
            /*如果当前文件目录下 不存在logs目录 则创建*/
            cur_path = boost::filesystem::current_path();
            boost::filesystem::path bfp = cur_path / DIRECTORY;
            cur_path = bfp;
            if(!boost::filesystem::exists(bfp) || !boost::filesystem::is_directory(bfp)){
                    boost::filesystem::create_directory(bfp);
            }
			req_num = 0;
            //std::cout<<"create log\n";
        }

        Log::~Log(){
            /*关闭文件流*/
            if(fout)fout.close();
        }
            
        Log* Log::getInstance(){
            static Log clog;
            //if(!clog)
              //  clog = new Log();
            return &clog;
        }
        void Log::record(string message){
            std::cout << message <<"\n";
            req_num++;
			if(req_num == 100){
				req_num = 0;
				if(fout)fout.close();
			}
            string filename = getFileName();
            if(checkOrCreate(filename)){
				fout << getCurTime() << " " << message << "\n";
                //std::cout<<message<<"\n";
            }else{
                std:cerr<<"Error:record failed\n";
            }

        }

        bool Log::checkOrCreate(string filename){
            boost::filesystem::path bfp = cur_path / filename;
            if(boost::filesystem::exists(bfp)){
                if(!fout){/*文件还没有打开*/
                    fout.open(bfp.string().c_str(), ios::app);
                    //std::cout << "open file" <<"\n";
                }
                return true;
            }else{
                //boost::filesystem::create_directory(bfp);
				if(fout){
					fout.close();//关闭上一个文件
				}
                /*创建新的文件的时候 就设置输出流*/
                fout.open(bfp.string().c_str(), ios::app);
                return true;
            }
        }

		string Log::getCurTime(){
			struct tm *now_t;         //实例化tm结构指针
			time_t cur_t;
            cur_t = time(NULL);
			now_t = localtime(&cur_t);
			char cur[20];
			sprintf(cur,"%04d-%02d-%02d %02d:%02d:%02d",now_t->tm_year+1900, now_t->tm_mon+1, now_t->tm_mday, now_t->tm_hour,\
							now_t->tm_min, now_t->tm_sec);
			string res = cur;
			return res;
		}

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
