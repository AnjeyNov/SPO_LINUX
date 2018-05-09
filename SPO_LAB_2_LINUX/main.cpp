#include <iostream>
#include <csignal>
#include <unistd.h>
#include <list>

const int WAITING_TIME = 1;


const std::string DELETE_ERROR =
        "List is empty.";

const char* PATH = "/home/anjey/qtprojects/SPO_LAB_2_CHILD_LINUX/child";

int main()
{
    int status = 0;
    char symbol;
    pid_t tempProcess;
    std::list<pid_t> processesIDList;

    sigset_t waitSet;                            // переменная для хранения набора сигналов
    sigemptyset(&waitSet);                       // очистка набора сигналов
    sigaddset(&waitSet, SIGUSR2);                // добавление SIGUSR2 в набор сигналов
    sigprocmask(SIG_BLOCK, &waitSet, nullptr);   // блокируем набор сигналов для возможности использовать sigwait()

    std::cout << "Main control process : " << getpid() << std::endl;

    while(true) {
        //сичтать символ
        std::cin.get(symbol);

        switch(symbol) {
            //добавить процесс-шестерку
            case '+': {
                //создание нового процесса
                tempProcess = fork();

                switch (tempProcess) {

                    //ошибка создания процесса
                    case -1: {
                        std::cout << "Error with fork operation." << std::endl;
                    } exit(EXIT_FAILURE); // завершение с ошибкой

                    //дочерний процесс
                    case 0: {
                        //заменяем дочерний процесс на процесс-шестерку
                        execv(PATH, nullptr);

                        //если не удалось заменить дочерний процесс на процесс-шестерку
                        std::cout << "Error with load operation, please check filePath" << std::endl;
                    } exit(EXIT_FAILURE);

                    //родительский процесс
                    default: {
                        //добавить PID процесса-шестерки в лист просессов
                        processesIDList.push_back(tempProcess);
                        sleep(WAITING_TIME);
                    } break;

                }

            } break;

            //удалить процесс-шестерку
            case '-': {
                if (!processesIDList.empty()) {

                    // отправляем процессу-шестерке сигнал вежливого завершиния
                    kill(processesIDList.back(), SIGTERM);

                    //удалить PID процесса-шестерки из листа процессов
                    processesIDList.pop_back();
                } else {
                    std::cout <<  "List is empty." << std::endl;
                }
            } break;
            //выйти удалив все процессы-шестерки сигнал убийствасигнал убийства
            case 'q':
                if(!processesIDList.empty())
                {
                    for(pid_t &childPid : processesIDList) {

                        //послать в последний созданный процесс-шестерку сигнал вежливого завершиния
                        kill(childPid, SIGTERM);
                    }
                    //очистить лист
                    processesIDList.clear();
                } exit(EXIT_SUCCESS);   // завершить программу с кодом 0
            default:
                continue;
        }
        std::cin.ignore();

        for(pid_t &childPid: processesIDList) {
            //послать в процесс-шестерку сигнал который он ожидает
            kill(childPid, SIGUSR1);
            //ждать один из заблокированных сигналов
            sigwait(&waitSet, &status);
        }
    }
}
