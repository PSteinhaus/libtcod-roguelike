// Dependency-management-model: "one to rule them all"
const int MAX_NAME_LENGTH = 40;

#include "libtcod.hpp"
#include "helpers.hpp"
#include <tuple>
#include <map>
#include <list>
#include <vector>
#include <limits>
typedef std::pair<int,int> Point;
class Actor;
class Tile;
#include "Persistent.hpp"
#include "Liquid.hpp"
#include "LiquidContainer.hpp"
#include "Body.hpp"
#include "Destructible.hpp"
#include "Attacker.hpp"
#include "Ai.hpp"
#include "Effect.hpp"
#include "Equipable.hpp"
#include "TargetSelector.hpp"
#include "Digestor.hpp"
#include "Useable.hpp"
#include "Pickable.hpp"
#include "Container.hpp"
#include "ActorRepository.hpp"
#include "Actor.hpp"
class Chunk;
#include "Tile.hpp"
#include "Map.hpp"
#include "Chunk.hpp"
#include "Camera.hpp"
#include "Gui.hpp"
#include "Engine.hpp"