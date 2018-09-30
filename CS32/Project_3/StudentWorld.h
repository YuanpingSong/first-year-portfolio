#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_


#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class NachenBlaster;
class GraphObject;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    // Game Logic
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    // Mutator
    void addActor(Actor* actor);
    void destroyedAlienShip();
    void escapedShip();
    // Accessor
    NachenBlaster* getNachenBlaster() const;
private:
    // Alien Traffic Control
    int shipsToDestoryThisLevel() const;
    int shipsRemainingToDestroy() const;
    int maxShipsPresent() const;
    int SmallgonProbability() const;
    int SmoregonProbability() const;
    int SnagglegonProbability() const;
    int totalProbability() const;
    int shipsDestroyed;
    int shipsPresent;
    // Helpter Functions
    bool isCollided(GraphObject *a, GraphObject *b) const;
    double euclidean_dist(int x1, int y1, int x2, int y2) const;
    bool willInteract(Actor* a, Actor* b) const;
    void attemptToActWith(Actor* a);
    // Data Member
    NachenBlaster* m_NachenBlaster;
    std::list<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
