#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <ctime>
 using namespace std;

void main()
{
  pid_t pid;
  int rv;
  string otp = "Дата: ";
  time_t seconds = time(NULL);
  tm* timeinfo;
  switch(pid=fork()) {
  case -1:
          perror("fork"); /* произошла ошибка */
          exit(1); /*выход из родительского процесса*/
  case 0:otp = "Дата: ";
          timeinfo = localtime(&seconds);
          otp+= asctime(timeinfo);
          cout << " CHILD: "<< otp << endl;
          cout << " CHILD: Это процесс-потомок!" << endl;
          cout << " CHILD: Мой PID -- " << getpid() << endl;
          cout << " CHILD: PID моего родителя -- " << getppid() << endl;
          cout << " CHILD: Выход!" << endl;
          exit(rv);
  default:
          otp = "Дата: ";
          timeinfo = localtime(&seconds);
          otp+= asctime(timeinfo);
          cout << "PARENT: "<< otp << endl;
          cout << "PARENT: Это процесс-родитель!" << endl;
          cout << "PARENT: Мой PID -- " << getpid() << endl;
          cout << "PARENT: PID моего потомка " << pid << endl;
          cout << "PARENT: Я жду, пока потомок не вызовет exit()..." << endl;
          wait(&pid);
          cout << "PARENT: Код возврата потомка: " << WEXITSTATUS(rv) << endl;
          cout << "PARENT: Выход!" << endl;
  }
}

