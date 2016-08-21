#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>
#include "world.h"
#include "neural/neural_network.h"

using namespace std;

int main(int argc, char *argv[]) {

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

  return 0;
}
