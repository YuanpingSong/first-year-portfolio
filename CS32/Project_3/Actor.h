#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
    Actor(StudentWorld* world, int imageID, double startX, double startY, int dir = 0, double size = 1.0, int depth = 0, bool isAlive = true);
    virtual ~Actor() { }
    // Derived - Class Defined Functions
    virtual bool willActOn(Actor * target) const = 0; // can this object collide with target?
    virtual void actOn(Actor * target) = 0; // collide with target and calculate the consequences
    virtual void doSomething() = 0; // let this object move in space according to its own algorithm
    // Identity Querying Functions
    virtual bool isProjectile() const = 0;
    virtual bool isShip() const = 0;
    virtual bool isGoodie() const  = 0;
    virtual bool isFriendly() const  = 0;
    virtual bool isInteractive() const = 0;
    // State Querying Functions
    bool isAlive() const;
    bool isOnScreen() const;
    // Mutator
    void markForDestruction();
    // Accessor
    StudentWorld* getWorld() const;
    // Helper Function
    bool isOnScreen(int x, int y) const;
private:
    bool m_isAlive; // wether an actor is alive cannot always be computed from its current state (i.e. position) therefore I use a data member to record whether it need be marked dead for some other reason.
    StudentWorld * m_world;
};

class Ship : public Actor {
public:
    Ship(StudentWorld* world, int imageID, double startX, double startY, double size, int depth, int health);
    virtual ~Ship() { }
    // Game Logic
    void sufferDamage(int amount);
    // Trivial Functions
    virtual bool isProjectile() const;
    virtual bool isShip() const;
    virtual bool isGoodie() const;
    virtual bool isInteractive() const;
    int getHealth() const;
    void setHealth(int value);
private:
    double m_hp;
};

class NachenBlaster : public Ship {
public:
    NachenBlaster(StudentWorld* world);
    virtual ~NachenBlaster() {}
    // Game Logic
    virtual void doSomething();
    virtual void actOn(Actor * target);
    virtual bool willActOn(Actor* target) const ;
    // Trivial Functions
    virtual bool isFriendly() const;
    int getTorpedoCount() const;
    void incTorpedoCount();
    int getEnergy() const;
private:
    int m_energyPoints;
    int m_nTorpedos;
};

class Alien : public Ship {
public:
    Alien(StudentWorld* world, int imageID, double startX, double startY, int health, int planLength, double speed, int planDirection, int score, int damage);
    virtual ~Alien() {}
    // Game Logic
    virtual void doSomething();
    virtual bool willActOn(Actor* target) const;
    virtual void actOn(Actor * target);
    // Trivial Functions
    virtual bool isFriendly() const;
    int getReward() const;
    void setFlightPlanLength(int length);
    void setFlightSpeed(double speed);
    void setFlightPlanDirection(int direction);
    // Derived Class Specific Public Functions
    virtual void attemptToDispenseGoodie() const = 0;
protected:
    static constexpr int DIR_UP_AND_LEFT = 135;
    static constexpr int DIR_DUE_LEFT = 180;
    static constexpr int DIR_DOWN_AND_LEFT = 225;
private:
    // Private member functions
    // Derived class specific functions
    virtual bool AttemptToFire() = 0;
    virtual void determineFlightPlanLength() = 0;
    // Helpter Functions
    void newFlightPlan();
    bool needNewPlan() const;
    bool isNachenBlasterInRange() const;
    void flyOneTick();
    // Private data members
    // Flight Plan Related
    int m_flightPlanLength;
    double m_flightSpeed;
    int m_flightPlanDirection;
    // Combat Related
    int m_reward;
    int m_damage;
};

class Smallgon : public Alien {
public:
    Smallgon(StudentWorld* world, double startX, double startY);
    virtual ~Smallgon() {}
    virtual void attemptToDispenseGoodie() const;
private:
    virtual bool AttemptToFire();
    virtual void determineFlightPlanLength();
};

class Smoregon : public Alien {
public:
    Smoregon(StudentWorld* world, double startX, double startY);
    virtual ~Smoregon() { }
    virtual void attemptToDispenseGoodie() const;
private:
    virtual bool AttemptToFire();
    virtual void determineFlightPlanLength();
};

class Snagglegon : public Alien {
public:
    Snagglegon(StudentWorld* world, double startX, double startY);
    virtual ~Snagglegon() {}
    virtual void attemptToDispenseGoodie() const;
private:
    virtual bool AttemptToFire();
    virtual void determineFlightPlanLength();
};

class Projectile : public Actor {
public:
    Projectile(StudentWorld* world, int imageID, double startX, double startY, int direction, int damage, int speed);
    virtual ~Projectile() { }
    // Game Logic
    virtual void doSomething();
    virtual bool willActOn(Actor* target) const;
    virtual void actOn(Actor* target);
    // Trivial Functions
    virtual bool isShip() const;
    virtual bool isProjectile() const;
    virtual bool isGoodie() const;
    virtual bool isInteractive() const;
    // derived class defined function
    
private:
    virtual void attemptToRotate() = 0;
    int m_damage;
    int m_speed;
};

class Cabbage : public Projectile {
public:
    Cabbage(StudentWorld* world, int x, int y);
    virtual ~Cabbage() { }
    virtual bool isFriendly() const;
private:
    virtual void attemptToRotate();
};

class Turnip : public Projectile {
public:
    Turnip(StudentWorld* world, double startX, double startY);
    virtual ~Turnip() {}
    virtual bool isFriendly() const;
private:
    virtual void attemptToRotate();
};

class Torpedo : public Projectile {
public:
    Torpedo(StudentWorld* world, double startX, double startY, int direction);
    virtual ~Torpedo() { }
    virtual bool isFriendly() const;
private:
    virtual void attemptToRotate();
};

class Goodie : public Actor {
public:
    Goodie(StudentWorld* world, int imageID, double startX, double startY);
    virtual ~Goodie() { }
    // Game Logic
    virtual void doSomething();
    virtual bool willActOn(Actor * target) const;
    virtual void actOn(Actor* target);
    // Trivial Functions
    virtual bool isProjectile() const;
    virtual bool isShip() const;
    virtual bool isGoodie() const;
    virtual bool isFriendly() const;
    virtual bool isInteractive() const;
private:
    virtual void dispenseReward() = 0;
};

class ExtraLifeGoodie : public Goodie  {
public:
    ExtraLifeGoodie(StudentWorld* world, double startX, double startY);
    virtual ~ExtraLifeGoodie() { }
private:
    virtual void dispenseReward();
};

class RepairGoodie : public Goodie {
public:
    RepairGoodie(StudentWorld* world, double startX, double startY);
    virtual ~RepairGoodie() { }
private:
     virtual void dispenseReward();
};

class TorpedoGoodie : public Goodie  {
public:
    TorpedoGoodie(StudentWorld* world, double startX, double startY);
    virtual ~TorpedoGoodie() { }
private:
     virtual void dispenseReward();
};

class NonInteractive : public Actor {
public:
    NonInteractive(StudentWorld* world, int imageID, double startX, double startY, double size, int depth);
    virtual ~NonInteractive() { }
    // Trivial Functions
    virtual bool willActOn(Actor* target) const;
    virtual void actOn(Actor* target);
    virtual bool isShip() const;
    virtual bool isProjectile() const;
    virtual bool isGoodie() const;
    virtual bool isFriendly() const;
    virtual bool isInteractive() const;
};

class Star : public NonInteractive {
public:
    Star(StudentWorld* world, double startX, double startY);
    virtual ~Star() { }
    virtual void doSomething();
};

class Explosion : public NonInteractive {
public:
    Explosion(StudentWorld* world, double startX, double startY);
    virtual ~Explosion() { }
    virtual void doSomething();
private:
    int m_age;
};

#endif // ACTOR_H_

