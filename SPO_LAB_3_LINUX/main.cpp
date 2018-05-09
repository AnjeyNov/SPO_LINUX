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
    cout << "ERROR IN CHILD " << endl;
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


    // инициализация семафора
    sem_t *sem;
    sem_unlink("pSem");
    if((sem = sem_open ("pSem", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
    {
        // выход в случае неудачи
        cout << "PARENT: CREATE SEMAPHORE ERROR" << endl;
        perror("CODE:");
        exit(EXIT_FAILURE);
    }

    // создание нового процесса
    pid_t ID;
    ID = fork();
    switch (ID) {

        // ошибка создания
        case -1:
            cout << "Create process error" << endl;
            exit(EXIT_FAILURE);
            break;

        // дочерний процесс
        case 0:
            execv("/home/anjey/qtprojects/build-SPO_LAB_3_CHILD_LINUX-Desktop-Debug/SPO_LAB_3_CHILD_LINUX", argv);
            cout << "Chiled error" << endl;
            kill(ID,SIGTERM);
            exit(EXIT_FAILURE);
            break;

        // родительский процесс
        default:
            break;
    }
    sleep(1);
    memorykey_size = shmget(key1, sizeof(double),0);            // получение дескриптора разделяемой памяти
    while(true){
        sem_wait(sem);                                          // ждем и занимаем семафор

        size = (double*)::shmat(memorykey_size,0,0);            // связывание адресного пространства
        cout << size[0] << endl;
        memorykey = shmget(key2, size[0],0);                    // получение дескриптора разделяемой памяти

        //if(size[0]<4095){
            mess = (char*)::shmat(memorykey,0,0);               // связывание адресного пространства
            string message = mess;
            cout << message << endl;
            shmdt(mess);                                        // отвязывание адресного пространства

       //}
       //else
       //     cout << "Извините, слишком большое сообщение" << endl;
       shmctl(memorykey, IPC_RMID, 0);                        // удаление разделяемого пространства
       sem_post(sem);                                         // освобождение семафора
       sleep(2);
    }
    kill(ID, SIGKILL);
    return 0;
}
