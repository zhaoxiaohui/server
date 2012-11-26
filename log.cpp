#include "log.hpp"

namespace http{
    namespace server3{
        
        Log::Log(){
            /*如果当前文件目录下 不存在logs目录 则创建*/
            cur_path = boost::filesystem::current_path();
            boost::filesystem::path bfp = cur_path / DIRECTORY;
            cur_path = bfp;
            fout = NULL;
            if(!boost::filesystem::exist(bfp) || !boost::filesystem::is_directory(bfp)){
                try{
                    boost::filesystem::create_directory(bfp);
                }catch(Exception e){
                    std::cerr<<"Error:"<<e.what()<<"\n";
                }
            }
        }

        Log::~Log(){
            /*关闭文件流*/
            if(fout)fout.close();
        }

        void Log::record(string message){
            string filename = getFileName();
            if(checkOrCreate(filename)){
                fout<<message<<"\n";
            }else{
                std:cerr<<"Error:record failed\n";
            }

        }

        bool Log::checkOrCreate(string filename){
            boost::filesystem::path bfp = cur_path / filename;
            if(boost::filesystem::exist(bfp)){
                if(fout == NULL){/*文件还没有打开*/
                    fout.open(bfp.file_string().c_str(),ios::app);
                }
                return true;
            }else{
                try{
                    boost::filesystem::create_directory(bfp);
                    /*创建新的文件的时候 就设置输出流*/
                    fout.open(bfp.file_string().c_str(),ios::app);
                    return true;
                }catch(Exception e){
                    std::cerr<<"Error:"<<e.what()<<"\n";
                    return false;
                }
            }
        }

        string Log::getFileName(){
            CTime now_t=CTime::GetCurrentTime();
            char cur[15]
            sprintf(cur,"%04d%02d%02d.txt", now_t.getYear(), now_t.getMonth(), now_t.getDay());
            string res = cur;
            return res;
        }


    }//end namespace server3
}//end namespace http
