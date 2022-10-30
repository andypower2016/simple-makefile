// C 
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

// C++
#include <fstream>
#include <chrono>
#include <ctime> 
#include <sstream>
#include <iomanip>
#include <thread>


std::ofstream outfile;


void func(const uint8_t *Data, size_t Size) {
  struct data{
    int x;
  };
  outfile << Size << std::endl;
  if(Size > 0)
  {
    outfile << "Data:" << std::endl;
    for (int i = 0 ; i < Size ; ++i)
    {
      char buffer[256];
      sprintf(buffer, "0x%02x\n", Data[i]);
      outfile << buffer;

      //struct data *ptr_data = NULL;
      //printf("ptr_data->x=%d\n", ptr_data->x);
    }
  }
}

void CreateCurLogFile()
{
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");

  char filename[256];
  sprintf(filename, "./myout/fout_%d_%d_%s", getpid(), std::hash<std::thread::id>{}(std::this_thread::get_id()), oss.str().c_str());
  outfile.open(filename, std::ofstream::out);
}

void CloseCurLogFile()
{
  outfile.close();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  CreateCurLogFile();
  func(Data, Size);
  CloseCurLogFile();
  return 0;
}
/*
int main()
{
	printf("main\n");
	return 0;
}*/
