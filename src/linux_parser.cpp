#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float MemTotal;
  float MemFree;
  float mu = 0;
  string line;
  string key;
  string value;
  string kb;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> kb) {
        // collect Mem Total
        if (key == "MemTotal") {
          std::replace(value.begin(), value.end(), '_', ' ');
          MemTotal = std::stof(value);
        }
        // collect MemFree, calculate memory utilization, return and exit
        // function
        else if (key == "MemFree") {
          MemFree = std::stof(value);
          mu = (MemTotal - MemFree) / MemTotal;
          return mu;
        };
      }
    }
  }
  return mu;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long ut;
  long idle;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> ut >> idle;
  }
  return ut;
}

// TODO: Read and return the [total] number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cpu = CpuUtilization();

  long total;
  long non_idle = ActiveJiffies();
  long idle = IdleJiffies();

  total = non_idle + idle;

  return total;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
float LinuxParser::ActiveJiffies(int pid) { 
  string sutime;  //user time 
  string sstime;  //kernel mode time 
  string scutime;
  string scstime;
  string start_time;  // measured in seconds
  long uptime = UpTime();
  float total_time;
  float seconds;
  float use = 0;
  float total_time_sec;
  string line;
  string key;
  string a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k >> l >> m >> n >> sutime >> sstime >> scutime >> scstime >> s >> t >> u >> start_time >> w;
      break;
      }     
    }

  if (a != to_string(pid)){
    return use;
  }
                          
  long utime = stol(sutime);
  long stime = stol(sstime);
  long cutime = stol(scutime);
  long cstime = stol(scstime);
  long st = stol(start_time);
  
  total_time = (utime + stime + cutime + cstime);
  seconds = uptime - (st/sysconf(_SC_CLK_TCK));
  total_time_sec = total_time/sysconf(_SC_CLK_TCK);
  if (seconds > 0) {
    use = total_time_sec/seconds;
  }

  return use;
}


// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> cpu = CpuUtilization();

  long non_idle;

  non_idle = stol(cpu[0]) + stol(cpu[1]) + stol(cpu[2]) + stol(cpu[5]) + stol(cpu[6]) + stol(cpu[7]);

  return non_idle;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu = CpuUtilization();

  long idle;

  idle = stol(cpu[3]) + stol(cpu[4]);

  return idle;
}


// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string key;
  string line;

  string user;    //0
  string nice;    //1
  string system;  //2
  string idle;    //3
  string iowait;  //4
  string irq;     //5
  string softirq; //6
  string steal;   //7
  string guest;   //8
  string guest_nice;  //9

  vector<string> cpu; 

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
      if (key == "cpu") {
        cpu.push_back(user);  
        cpu.push_back(nice);    
        cpu.push_back(system);  
        cpu.push_back(idle);   
        cpu.push_back(iowait);  
        cpu.push_back(irq);     
        cpu.push_back(softirq);
        cpu.push_back(steal);   
        cpu.push_back(guest);  
        cpu.push_back(guest_nice);

        return cpu;
      }
    }
  }
}


// TODO: Read and return the total number of processes
// Resource: https://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-stdstring-starts-with-a-certain-string-and-convert-a
int LinuxParser::TotalProcesses() {
  int tp;
  string line;
  string key;
  string key2;
  string key3;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ' ', ',');
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key.rfind("processes",0) == 0){
          std::replace(key.begin(), key.end(), ',', ' ');
          std::istringstream ss(key);
          ss >> key2 >> key3;
          tp = stoi(key3);
          return tp;
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int rp;
  string line;
  string key;
  string key2;
  string key3;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ' ', ',');
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key.rfind("procs_running",0) == 0){
          std::replace(key.begin(), key.end(), ',', ' ');
          std::istringstream ss(key);
          ss >> key2 >> key3;
          rp = stoi(key3);
          return rp;
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string c;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> c;
    return c + "                                                                           ";
    }
  return c;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string r;
  string line;
  string key;
  string key2;
  int mb;
  std::ifstream filestream(kProcDirectory + to_string(pid)+ kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), ' ', ' ');
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key.rfind("VmSize",0) == 0){
          std::istringstream ss(line);
          ss >> key2 >> r;
          mb = stoi(r)/1000;
          return to_string(mb);
        }
      }
    }
  }
  return string();
}


// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
    string uid;
    string line;
    string key;
    string key2;
    string key3;
    std::ifstream filestream(kProcDirectory + to_string(pid)+ kStatusFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)){
        std::replace(line.begin(), line.end(), ':', ' ');
        std::replace(line.begin(), line.end(), ' ', ' ');
        std::istringstream linestream(line);
        while (linestream >> key) {
          if (key.rfind("Uid",0) == 0){
            std::istringstream ss(line);
            ss >> key2 >> uid;
            return uid;
          }
        }
      }
    }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string u;
  string line;
  string key;
  string vvv = ":x:" + uid + ":";
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while  (linestream >> key){
        if(key.find(vvv)!= string::npos){
          size_t pos = line.find(vvv);
          long pos2 = (long)pos;
          for (long i = 0; i < pos2; i++) {
            u += key[i];
          }
          
          return u + "     ";
        }
      }
    }
  }
  return "back up"; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid){
  string ut;
  long start_time_raw;
  long start_time;
  string line;
  string key;
  string a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k >> l >> m >> n >> o >> p >> q >> r >> s >> t >> u >> ut;
      start_time_raw = stol(ut);
      break;
      }     
    }
  start_time = start_time_raw/sysconf(_SC_CLK_TCK);
  
  return start_time;
}
