#include <csignal>
#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <thread>
#include "simulator.h"

bool running = true;
bool active = false;
int gameMode = -1;

void signalHandler(int signal) {
    running = false;
    std::exit(signal);
}

int main(int argc, char* argv[]) {
    if (argc == 35) {
        // ./pool_simulator.exe 24 0 25.948557 -1.125 25.948557 1.125 27.897114 -2.25 27.897114 0 27.897114 2.25 29.84567 -3.375 29.84567 -1.125 29.84567 1.125 29.84567 3.375 31.79423 -4.5 31.79423 -2.25 31.79423 0 31.79423 2.25 31.79423 4.5 1 2 3 4
        // ./pool_simulator.exe 31.213 -12.9567 40.5841 -23.4996 41.1107 2.76752 33.5685 -9.12049 35.1899 -6.23481 31.4919 16.4001 37.5773 -11.7111 39.6964 -8.63141 31.2216 -0.548955 39.6093 14.7927 41.7601 -9.97258 44.2546 -5.39355 38.3861 -1.86874 31.6491 2.35859 40.9087 17.1636 32.7772 8.98929 0 0
        for (int n = 0, i = 1; n < ball_n; n++) {
            balls[n].x = std::strtof(argv[i++], nullptr);
            balls[n].y = std::strtof(argv[i++], nullptr);
        }
        balls[ball_n - 1].i = std::strtof(argv[33], nullptr) * maxValocity;
        balls[ball_n - 1].j = std::strtof(argv[34], nullptr) * maxValocity;
        balls[ball_n - 1].moving = true;

        //std::thread thread_obj(render);active = true;simulateGame();closeWindow();thread_obj.join();
        simulateRound();
        
        for (int n = 0; n < ball_n; n++) {
            std::cout << balls[n].x << std::endl;
            std::cout << balls[n].y << std::endl;
           
        }
        std::cout << 0 << std::endl;
        std::cout << 0 << std::endl;
        return 0;
    }

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::cout << "======================\n"
              << "   POOL SIMULATOR\n"
              << "======================\n";
    while (gameMode < 0 || gameMode > 4) {
        std::cout << "Select a game mode:\n"
                  << "  [0] Single Player\n"
                  << "  [1] Host a Server\n"
                  << "  [2] Join as a Client\n"
                  << "  [3] Quit\n"
                  << "----------------------\n"
                  << "Option: ";
        std::cin >> gameMode;
        if (std::cin.fail() || gameMode < 0 || gameMode > 3) {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "[ERROR] Invalid option. Please try again.\n"
                      << "----------------------\n";
            gameMode = -1; 
        }
    }
    std::cout << "----------------------\n";

    if(gameMode == 3)
        return 0;

    setup();

    std::thread networkThread;

    if (gameMode == 1) {
        hostSocket();
        networkThread = std::thread(receiveGameState);
    } else if (gameMode == 2) {
        joinSocket();
        networkThread = std::thread(receiveGameState);
    }
    
    std::thread renderThread(render);
    
    simulateGame();

    if (gameMode == 1 || gameMode == 2) {
        closeSocket();
        networkThread.join(); 
    }

    renderThread.join();

    return 0;
}