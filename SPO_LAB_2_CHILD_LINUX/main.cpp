#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sstream>

const static std::string MESSAGE =
        "PID: ";

const int WAIT_TIME = 1;

template <typename T>
std::string toString(T val)
{
    std::ostringstream oss;
    oss<< val;
    return oss.str();
}

void term_handler(int i){
    std::cout << "Terminating " << getpid() << std::endl;
    exit(EXIT_SUCCESS);
}

int main()
{
    int status = 0;
    int thisPID = getpid();

    std::string childMessage = MESSAGE;
    childMessage += toString(thisPID);

    // устанавливаю обработчик для SIGTERM
    struct sigaction sa;
    sa.sa_handler = term_handler;
    sigaction(SIGTERM, &sa, 0);

    sigset_t set;                            // переменная для хранения набора сигналов
    sigemptyset(&set);                       // очистка набора сигналов
    sigaddset(&set, SIGUSR1);                //добавление SIGUSR1 в набор сигналов

    sigprocmask(SIG_BLOCK, &set, NULL);      //блокируем набор сигналов для возможности использовать sigwait()

    while (true)
    {
        sigwait(&set, &status);                         // ждем один из заблокированных сигналов
        std::cout << childMessage.c_str() << std::endl;;
        kill(getppid(), SIGUSR2);                       // посылаем родителю сигнал
    }
    return 0;
}
