#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>
#include <math.h>
#include "world.h"
#include "neural/neural_network.h"

using namespace std;

#ifdef __MACH__
  #include <mach/mach_time.h>
#else
  #include <time.h>

  void get_current_time_ns(struct timespec &spec, uint64_t &time) {
    clock_gettime(CLOCK_MONOTONIC, &spec);
    time = (spec.tv_sec * 1.0e9) + spec.tv_nsec;
  }
#endif

int main(int argc, char *argv[]) {
  random_device rd;
  srand(rd());

  /*
    Usually I'd break this up into two seperate applications, but as this is
    more of a toy a simple if/then will suffice
  */
  if(argc == 2 && strcmp(argv[1], "--test") == 0) {

    // Test mode, used for performance benchmarks.
    CWorld* world = new CWorld(800, 800, 10, 30, false);

    double average_time = 0;

    uint64_t start_time;
    uint64_t end_time;
    uint64_t run_time;

    #ifdef __MACH__
      mach_timebase_info_data_t timebase_info;
      mach_timebase_info(&timebase_info);
    #else
      struct timespec current_time;
    #endif

    cout << "Running 1M steps..." << endl;

    for(int i = 0; i < 10; i++) {

      #ifdef __MACH__
        start_time = mach_absolute_time();
      #else
        get_current_time_ns(current_time, start_time);
      #endif

      for(double i = 0; i < 100000; i++) {
        world->step();
      }

      #ifdef __MACH__
        end_time = mach_absolute_time(); // ns
      #else
        get_current_time_ns(current_time, end_time);
      #endif

      run_time = end_time - start_time;
      average_time += run_time / 10;

      #ifdef __MACH__
        run_time *= timebase_info.numer;
        run_time /= timebase_info.denom;
      #endif

      cout << "100k steps in " << run_time / 1000000 << "ms" << endl;
    }

    #ifdef __MACH__
      average_time *= timebase_info.numer;
      average_time /= timebase_info.denom;
    #endif

    cout << "Average step time: " << average_time / 100000 << "ns" << endl;

  } else {

    // Normal execution mode, graphical output
    sf::RenderWindow window(sf::VideoMode(800, 800), "NNetworks");
    window.setVerticalSyncEnabled(false);
    window.setFramerateLimit(0);

    CWorld* world = new CWorld(800, 800, 10, 30, true);

    sf::Event event;
    while(window.isOpen()) {
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
          window.close();
        if (event.type == sf::Event::KeyReleased) {
          if (event.key.code == sf::Keyboard::S) {
            window.setFramerateLimit(30);
          } else if (event.key.code == sf::Keyboard::F) {
            window.setFramerateLimit(60);
          } else if (event.key.code == sf::Keyboard::U) {
            window.setFramerateLimit(0);
          }
        }
      }

      world->step();
      window.clear();
      world->draw(&window);
      window.display();
    }

    delete world;
  }

  return 0;
}
