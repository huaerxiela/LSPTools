#include "dlfcn.h"
#include "logging.h"
#include <string>
#include <unwind.h>
#include "sstream"

#define MAX_STACK_TRACE_DEPTH 10

static void GetStackTraceInfoByBuiltin(){
    std::stringstream ss;
    void *curAddr;
    for (int i = 0; i < MAX_STACK_TRACE_DEPTH; i++) {
        Dl_info info;
        void *base = nullptr;
        const char *file ="", *symbol ="";
        switch (i) {
            case 0:
                curAddr = __builtin_return_address(0);
                break;
            case 1:
                curAddr = __builtin_return_address(1);
                break;
            case 2:
                curAddr = __builtin_return_address(2);
                break;
            case 3:
                curAddr = __builtin_return_address(3);
                break;
            case 4:
                curAddr = __builtin_return_address(4);
                break;
            case 5:
                curAddr = __builtin_return_address(5);
                break;
            case 6:
                curAddr = __builtin_return_address(6);
                break;
            case 7:
                curAddr = __builtin_return_address(7);
                break;
            case 8:
                curAddr = __builtin_return_address(8);
                break;
            case 9:
                curAddr = __builtin_return_address(9);
                break;
            case 10:
                curAddr = __builtin_return_address(10);
                break;
        }
        if (curAddr == 0x0){
            break;
        }
        if(dladdr(curAddr, &info)){
            if(info.dli_sname){
                symbol =info.dli_sname;
            }
            if(info.dli_fname){
                file = info.dli_fname;
            }
            if(info.dli_fbase != nullptr){
                base = info.dli_fbase;
            }
        }
        ss << "#" << curAddr << "\t";
        ss << hex << ((uint64_t)curAddr-(uint64_t) base) << "\t";
        ss << file << "\t";
        ss << symbol << "\t";
        ss << "\n";
    }
    LOG(INFO) << "GetStackTraceInfoByBuiltin\n" << ss.str();
}