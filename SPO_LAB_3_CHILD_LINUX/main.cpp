#include <iostream>
#include <limits>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <cstdio>


using namespace std;

double *size;
char* mess;
int memorykey;                      // передаваемый текст
int memorykey_size;                 // размер передаваемого текста

void term_handler(int i){
    shmdt(mess);
    shmdt(size);
    shmctl(memorykey, IPC_RMID, 0);
    shmctl(memorykey_size, IPC_RMID, 0);
    cout << "ERROR IN PARENT " << endl;
    exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
    // устанавливаю обработчик для SIGTERM
    struct sigaction sa;
    sa.sa_handler = term_handler;
    sigaction(SIGTERM, &sa, 0);


    key_t key1 = 9998;
    key_t key2 = 9999;

    if( (memorykey_size = shmget(key1, sizeof(double),IPC_CREAT | IPC_EXCL | 0660)) == -1){ // получение дескриптора разделяемой памяти
        memorykey_size = shmget(key1, sizeof(double),0);

    }


    // открытие семафора.
    sem_t *sem;
    if((sem = sem_open("pSem", O_RDWR, 0666, 0)) == SEM_FAILED)
    {
        // выход в случае неудачи
        cout << "CHILD: OPEN SEMAPHORE ERROR" << endl;
        perror("CODE:");
        kill(getppid(),SIGTERM);
        exit(EXIT_FAILURE);
    }

    string message;
    while(true){
        cout << "wait" << endl;
        sem_wait(sem);                                                                   // ждем семафор
        
        message.clear();
        message.resize(0);

        cout << "Input message: ";
        char c;
        c = getchar();
        while(c!=EOF){
            message+=c;
            c = getchar();
        }
        cout << "Click ENTER" << endl;
        cin.ignore();

        double len = message.size();
        size = (double*)::shmat(memorykey_size,0,0);                                  // связывание адресного пространства
        memcpy(size, &len, sizeof(double));                                           // копируем байтики
        cout << size[0] << endl;

        if( (memorykey = shmget(key2, len,IPC_CREAT | IPC_EXCL | 0660)) == -1){       // получение дескриптора разделяемой памяти
            memorykey = shmget(key2, len,0);

        }
        mess = (char*)::shmat(memorykey,0,0);                                         // связывание адресного пространства
        const char* str = message.c_str();
        memcpy(mess, str, message.size());                                            // копируем байтики


        shmdt(mess);                                                                   // отвязывание адресного пространства
        sem_post(sem);                                                                 // освободить семафор
        sleep(1);

    }
    exit(EXIT_SUCCESS);
    return 0;
}
