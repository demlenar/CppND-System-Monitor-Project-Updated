#include "processor.h"
#include "linux_parser.h"
#include <thread>
#include <chrono>

using namespace std::this_thread;
using namespace std::chrono;


// TODO: Return the aggregate CPU utilization
// Resource https://stackoverflow.com/questions/158585/how-do-you-add-a-timed-delay-to-a-c-program
float Processor::Utilization() { 
    use_ = float(LinuxParser::ActiveJiffies())/float(LinuxParser::Jiffies());
    return use_; }