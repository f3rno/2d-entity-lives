#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>
#include "world.h"
#include "neural_network.h"

using namespace std;

int main() {
  random_device rd;
  srand(rd());

  sf::RenderWindow window(sf::VideoMode(400, 400), "NNetworks");
  window.setVerticalSyncEnabled(false);
  window.setFramerateLimit(0);
  bool graphics_enabled = true;

  CWorld* world = new CWorld(400, 400, 50, 60);
  // world->setBreeding(false);

  int loops = 0;
  sf::Clock loopClock;

  sf::Event event;
  while(window.isOpen()) {
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::G) {
        graphics_enabled = !graphics_enabled;

        if (!graphics_enabled) {
          window.clear();
        }
       }
    }

    world->step();

    if (graphics_enabled) {
      window.clear();
      world->draw(&window);
      window.display();
    }

    loops++;
    if (loopClock.getElapsedTime().asSeconds() >= 1.0f) {
      cout << loops << " l/s\n";
      loops = 0;
      loopClock.restart();
    }
  }

  delete world;
  return 0;
}