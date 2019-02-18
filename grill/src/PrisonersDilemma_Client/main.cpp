/*
 *  Autor: Matthias Grill
 *  Class: 5BHIF
 *  Date: 11.02.2019
 *  File: PrisonersDilemma_Client/main.cpp
 *   ____ _ _            _   
 *  / ___| (_) ___ _ __ | |_ 
 * | |   | | |/ _ \ '_ \| __|
 * | |___| | |  __/ | | | |_ 
 *  \____|_|_|\___|_| |_|\__|
 */
#include <iostream>
#include <thread>
#include "../includes.cpp"

using namespace std;

//int main(int argc, char* argv[]) {
int main() {
    thread t{[]{ cout << "Hello"; }};
    t.join();
    cout << " I am the client!" << endl;
    return 0;
}
