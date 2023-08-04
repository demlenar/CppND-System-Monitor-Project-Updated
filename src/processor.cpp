#include "processor.h"
#include "linux_parser.h"
#include <thread>
#include <chrono>

using namespace std::this_thread;
using namespace std::chrono;


// TODO: Return the aggregate CPU utilization
// Resource https://stackoverflow.com/questions/158585/how-do-you-add-a-timed-delay-to-a-c-program
float Processor::Utilization() { 
    long PrevNonIdle = LinuxParser::Jiffies();  
    long PrevIdle = LinuxParser::IdleJiffies();
    long PrevTotal = PrevNonIdle + PrevIdle;
    
    sleep_until(system_clock::now() + seconds(2));

    long NonIdle = LinuxParser::Jiffies();  
    long Idle = LinuxParser::IdleJiffies();
    long Total = NonIdle + Idle;

    float totald = Total - PrevTotal;
    float idled = Idle - PrevIdle;

    use_ = (totald - idled)/totald;

    return use_; }