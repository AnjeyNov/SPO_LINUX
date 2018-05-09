#include <iostream>
#include <pthread.h>
#include <list>
#include <csignal>
#include <unistd.h>
#include <stdlib.h>
using namespace std;


pthread_mutex_t mp;

void term_handler(int i){
    cout << "Terminating " << pthread_self() << endl;
    pthread_exit(NULL);
}

void* newthread(void* arg){
    // устанавливаю обработчик для SIGTERM
    struct sigaction sa;
    sa.sa_handler = term_handler;
    sigaction(SIGTERM, &sa, 0);
    cout << "Create thread " << pthread_self() << endl;
    while (true)
    {
        pthread_mutex_lock(&mp);
        cout << "THREAD " << pthread_self() << endl;
        pthread_mutex_unlock(&mp);
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    list<pthread_t> threadIDlist;
    if(pthread_mutex_init(&mp, NULL)!=0){
        cout << "Mutex create error" << endl;
    }
    while(true) {
        pthread_mutex_lock(&mp);
        char symbol;
        cout << "Print symbol" << endl;
        //сичтать символ
        cin.get(symbol);

        switch(symbol) {
            //добавить процесс-шестерку
            case '+': {
                //создание нового потока
                pthread_t id;
                if(pthread_create(&id,NULL, newthread, NULL)!=0){
                    cout << "Thread create error" << endl;
                    break;
                }
                //добавить ID потока-шестерки в лист потоков
                threadIDlist.push_back(id);
                sleep(1);

            }break;

            //удалить процесс-шестерку
            case '-': {
                if (!threadIDlist.empty()) {

                    // отправляем процессу-шестерке сигнал вежливого завершиния
                    pthread_kill(threadIDlist.back(), SIGTERM);
                    sleep(1);
                    //удалить PID процесса-шестерки из листа процессов
                    threadIDlist.pop_back();
                } else {
                    cout <<  "List is empty." << endl;
                }
            } break;
            //выйти удалив все процессы-шестерки сигнал убийствасигнал убийства
            case 'q':
                if(!threadIDlist.empty())
                {
                    for(pthread_t &childID: threadIDlist) {

                        //послать в последний созданный процесс-шестерку сигнал вежливого завершиния
                        pthread_kill(childID, SIGTERM);
                    }
                    //очистить лист
                    threadIDlist.clear();
                } exit(EXIT_SUCCESS);   // завершить программу с кодом 0
            default:
                continue;
        }
        cin.ignore();
        pthread_mutex_unlock(&mp);
        sleep(1);
    }

    return 0;
}
