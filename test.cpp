
#include "./log.h"
using namespace std;
#define out(x) std::cout<<x<<std::endl;

Logger log_obj("1.txt");

int main()
{
   write_log(log_obj,"this info",WARNING);
   return 0;
}