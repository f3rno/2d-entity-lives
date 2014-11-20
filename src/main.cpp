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

  sf::RenderWindow window(sf::VideoMode(800, 800), "NNetworks");
  window.setVerticalSyncEnabled(false);
  window.setFramerateLimit(0);
  bool graphics_enabled = true;

  CWorld* world = new CWorld(800, 800, 10, 30);

  int loops = 0;
  sf::Clock loopClock;

  sf::Event event;
  while(window.isOpen()) {
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::G) {
          graphics_enabled = !graphics_enabled;

          if (!graphics_enabled) {
            window.clear();
          }
        } else if (event.key.code == sf::Keyboard::S) {
          window.setFramerateLimit(30);
        } else if (event.key.code == sf::Keyboard::F) {
          window.setFramerateLimit(60);
        } else if (event.key.code == sf::Keyboard::U) {
          window.setFramerateLimit(0);
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
