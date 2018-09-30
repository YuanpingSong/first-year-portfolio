#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <cmath>
#include "Actor.h"
#include "GraphObject.h"
#include <iostream>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{
    // these code are, in reality, unneccessary, because the init function has the same role.
    m_NachenBlaster = nullptr;
    shipsDestroyed = 0;
    shipsPresent = 0;
    // m_actors is default initialized to an empty list
}

StudentWorld::~StudentWorld(){
    delete m_NachenBlaster;
    m_NachenBlaster = nullptr;
    for (Actor * ap : m_actors) {
        delete ap;
        ap = nullptr;
    }
    m_actors.clear(); // the same code destroys dynamically allocated objects when a level finishes and when the studentWorld obj. is destructed
}

int StudentWorld::init()
{
    shipsPresent = 0; // we need to set those numbers to 0 at the start of each round.
    shipsDestroyed = 0;
    m_NachenBlaster = new NachenBlaster(this);
    // fill the background with 30 stars
    for (int i = 0; i < 30; i++) {
        Star * ap = new Star(this, randInt(0, 255), randInt(0, 255));
        m_actors.push_back(ap);
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // give each object a chance to do something
    m_NachenBlaster -> doSomething();
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        attemptToActWith(*it);
        if (m_NachenBlaster -> getHealth() <= 0) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
            
        }
        (*it) -> doSomething();
        attemptToActWith(*it); // check wheter a collision has occured both before and after the actor moved, as required by the spec.
        
        // check if NachenBlaster has died
        if (m_NachenBlaster -> getHealth() <= 0) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
            
        } // check if life is decremented
        // player finished level. Check if level, life, scores are modified correctly
        if (shipsRemainingToDestroy() == 0) {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    // delete dead flying objects from the game
    list<Actor*>::iterator it = m_actors.begin();
    while ( it != m_actors.end()) {
        if ( (*it) -> isAlive()) {
            it++;
        } else {
            delete *it;
            it = m_actors.erase(it);
        }
    }
    
    // Update Game Statistics
    string banner = "Lives: " + to_string(getLives()) + "  Health: " + to_string(m_NachenBlaster -> getHealth() * 2) + "%  Score: " + to_string(getScore()) + "  Level: " + to_string(getLevel()) + "  Cabbages: " + to_string(m_NachenBlaster -> getEnergy() * 100 / 30) + "%  Torpedoes: " + to_string(m_NachenBlaster -> getTorpedoCount());
    setGameStatText(banner);
    
    // new star
    if (randInt(1, 15) == 1) {
        Star * ap = new Star(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT - 1));
        m_actors.push_back(ap);
    }
    
    // new ship
    if (shipsPresent < min(maxShipsPresent(), shipsRemainingToDestroy())) {
        // introduce a new ship
        shipsPresent++;
        int determinant = randInt(0, totalProbability() - 1);
        if (determinant < SmallgonProbability()) { // smallgon
            Actor * ap = new Smallgon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT -1));
            addActor(ap);
        } else if (determinant < SmallgonProbability() + SmoregonProbability()) { // smoregon
            Actor * ap = new Smoregon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT -1));
            addActor(ap);
        } else { // snagglegon
            Actor * ap = new Snagglegon(this, VIEW_WIDTH - 1, randInt(0, VIEW_HEIGHT -1));
            addActor(ap);
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_NachenBlaster;
    m_NachenBlaster = nullptr;
    for (Actor * ap : m_actors) {
        delete ap;
        ap = nullptr;
    }
    m_actors.clear();
}

bool StudentWorld::isCollided(GraphObject *a, GraphObject *b) const {
    return euclidean_dist(a -> getX(), a -> getY(), b -> getX(), b -> getY()) < 0.75 * ( a -> getRadius() + b -> getRadius() );
}

double StudentWorld::euclidean_dist(int x1, int y1, int x2, int y2) const {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

bool StudentWorld::willInteract(Actor *a, Actor *b) const {
    if (!isCollided(a, b)) {
        return false;
    }
    if ( !(a -> isAlive() && b -> isAlive()) ) {
        return false;
    }
    if (a == b){
        return false; // no self-interaction
    }
    return a -> willActOn(b) || b -> willActOn(a);
}


void StudentWorld::attemptToActWith(Actor *a) {
    // dead objects cannot act
    if (! (a -> isAlive()) ) {
        return;
    }
    // immediately finish the level if Nachenblaster is no longer alive.
    if (!m_NachenBlaster -> isAlive()) {
        return;
    }
    // possible collision with NachenBlaster
    if (willInteract(a, m_NachenBlaster)) {
        a -> actOn(m_NachenBlaster);
        return;
    }
    // possible collision among other actors
    for (auto it = m_actors.begin(); it != m_actors.end(); it++) {
        if (*it != a) {
            if (willInteract(a, *it)) {
                if (a ->willActOn(*it)) {
                    a -> actOn(*it);
                    break; // since ap is consumed, proceed to the next item.
                } else {
                    (*it) -> actOn(a);
                }
            }
        }
    }
}

void StudentWorld::addActor(Actor* actor) {
    m_actors.push_front(actor); // new actors are not processed during the current tick
}

int StudentWorld::shipsToDestoryThisLevel() const {
    return 6 + 4 * getLevel();
}

int StudentWorld::shipsRemainingToDestroy() const {
    return shipsToDestoryThisLevel() - shipsDestroyed;
}
void StudentWorld::destroyedAlienShip() {
    shipsDestroyed++;
    shipsPresent--;
}

void StudentWorld::escapedShip() {
    shipsPresent--;
}

NachenBlaster* StudentWorld::getNachenBlaster() const {
    return m_NachenBlaster;
}

int StudentWorld::maxShipsPresent() const {
    return 4 + 0.5 * getLevel();
}

int StudentWorld::SmallgonProbability() const {
    return 60;
}

int StudentWorld::SmoregonProbability() const {
    return 20 + getLevel() * 5;
}

int StudentWorld::SnagglegonProbability() const {
    return 5 + getLevel() * 10;
}

int StudentWorld::totalProbability() const {
    return SmallgonProbability() + SmoregonProbability() + SnagglegonProbability();
}



