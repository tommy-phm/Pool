#include <thread>
#include "simulator.h"

bool endGame = false;
bool active = false;

int main() {
    std::cout << "[Pool Simulator] Staring Program\n";
    setup();
    std::thread thread_obj(render);
    
    run();

    closeWindow();
    thread_obj.join();
    return 0;
}