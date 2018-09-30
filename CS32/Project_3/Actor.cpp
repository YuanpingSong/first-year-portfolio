#include "Actor.h"
#include <string>
#include <iostream>
#include <climits>
using namespace std;
// class Actor

Actor::Actor(StudentWorld* world, int imageID, double startX, double startY, int dir, double size, int depth, bool isAlive) : GraphObject(imageID, startX, startY, dir, size, depth) , m_isAlive(isAlive), m_world(world) {
    
}

bool Actor::isAlive() const  {
    return m_isAlive;
}

bool Actor::isOnScreen() const {
    return isOnScreen(getX(), getY());
}

void Actor::markForDestruction() { // even though isAlive() can check whether an actor is still on Screen. I require calling this function explicitly in order to discard an object so as to make the game logic more obvious.
    m_isAlive = false;
}

StudentWorld* Actor::getWorld() const {
    return m_world;
}

bool Actor::isOnScreen(int x, int y) const {
    return 0 <= x && x < VIEW_WIDTH && 0 <= y && y < VIEW_HEIGHT;
}

// end class Actor

// class Ship

Ship::Ship(StudentWorld* world, int imageID, double startX, double startY, double size, int depth, int health) : Actor(world, imageID, startX, startY, 0, size, depth), m_hp(health) {
}

void Ship::sufferDamage(int amount) {
    m_hp -= amount;
    if (m_hp <= 0) {
        markForDestruction();
        Actor* ap = new Explosion(getWorld(), getX(), getY());
        getWorld() -> addActor(ap);
        if (!isFriendly()) {
            getWorld() -> playSound(SOUND_DEATH);
            getWorld() -> destroyedAlienShip();
            getWorld() -> increaseScore(dynamic_cast<Alien*>(this) -> getReward());
            dynamic_cast<Alien*>(this) -> attemptToDispenseGoodie();
        }
    } else {
        getWorld() -> playSound(SOUND_BLAST);
    }
}

bool Ship::isProjectile() const {
    return false;
}

bool Ship::isShip() const {
    return true;
}

bool Ship::isGoodie() const {
    return false;
}

bool Ship::isInteractive() const {
    return true;
}

int Ship::getHealth() const {
    return m_hp;
}

void Ship::setHealth(int value) {
    m_hp = value;
}

// end class Ship

// class NachenBlaster

NachenBlaster::NachenBlaster(StudentWorld* world) : Ship(world, IID_NACHENBLASTER, 0 , 128, 1.0, 0, 50), m_energyPoints(30), m_nTorpedos(0) {
}

void NachenBlaster::doSomething() {
    // if NachenBlaster is not currently alive, return immediately
    if (!isAlive()) {
        return;
    }
    
    int ch;
    if (getWorld() -> getKey(ch)) {
        switch (ch) {
            case KEY_PRESS_LEFT:
                if (isOnScreen(getX() - 6, getY())) {
                    moveTo(getX() - 6, getY());
                } else {
                    moveTo(0 , getY());
                }
                break;
            case KEY_PRESS_RIGHT:
                if (isOnScreen(getX() + 6, getY())) {
                    moveTo(getX() + 6, getY());
                } else {
                    moveTo(VIEW_WIDTH - 1, getY());
                }
                break;
            case KEY_PRESS_UP:
                if (isOnScreen(getX(), getY() + 6)) {
                    moveTo(getX(), getY() + 6);
                } else {
                    moveTo(getX(), VIEW_HEIGHT - 1 );
                }
                break;
            case KEY_PRESS_DOWN:
                if (isOnScreen(getX(), getY() - 6)) {
                    moveTo(getX(), getY() - 6);
                } else {
                    moveTo(getX(), 0 );
                }
                break;
            case KEY_PRESS_SPACE:
                if (m_energyPoints >= 5) {
                    m_energyPoints -= 5;
                    getWorld() -> playSound(SOUND_PLAYER_SHOOT);
                    Actor * ap = new Cabbage(getWorld(), getX() + 12, getY()); // what if the cabbage is immediately out of bounds?
                    getWorld() -> addActor(ap);
                }
                break;
            case KEY_PRESS_TAB:
                if(m_nTorpedos > 0) {
                    m_nTorpedos--;
                    getWorld() -> playSound(SOUND_TORPEDO);
                    Actor * ap = new Torpedo(getWorld(), getX() + 12, getY(), 0);
                    getWorld() -> addActor(ap);
                }
                break;
            default:
                break;
            // Check for collision pg 24
        } // end switch
    } // end if
    if (m_energyPoints < 30) {
        m_energyPoints++;
    }
}

bool NachenBlaster::willActOn(Actor* target) const {
    return false; // Generally, Nachenblaster is not consumed in any interaction, therefore it is always an target and never an actor.
}

void NachenBlaster::actOn(Actor * target) {
    return; // this function is included just for completeness. It will never be called.
}



bool NachenBlaster::isFriendly() const {
    return true;
}

int NachenBlaster::getTorpedoCount() const {
    return m_nTorpedos;
}

void NachenBlaster::incTorpedoCount() {
    m_nTorpedos += 5;
}

int NachenBlaster::getEnergy() const {
    return m_energyPoints;
}

// end NachenBlaster

// class Alien

Alien::Alien(StudentWorld* world, int imageID, double startX, double startY, int health, int planLength, double speed, int planDirection, int score, int damage) : Ship(world, imageID, startX, startY, 1.5, 1, health) , m_flightPlanLength(planLength), m_flightSpeed(speed) , m_flightPlanDirection(planDirection), m_reward(score), m_damage(damage) {
    
}

void Alien::doSomething() {
    // destroyed
    if (!isAlive()) {
        return;
    }
    // flown off the left side of the screen
    if (getX() < 0) {
        markForDestruction();
        getWorld() -> escapedShip();
        return;
    }
    
    // did collision happen? This part I will handle in studentWorld
    
    // do we need new flight plan?
    if (needNewPlan()) {
        newFlightPlan();
    }
    
    // can we shoot?
    if (isNachenBlasterInRange()) {
        if(AttemptToFire()) {
            return;
        }
    }
    
    // move
    flyOneTick();
    
    // check again for possible collision
}

bool Alien::needNewPlan() const {
    return m_flightPlanLength == 0 || getY() <= 0 || getY() >= VIEW_HEIGHT - 1;
}

bool Alien::isNachenBlasterInRange() const {
    Actor* NachenBlaster = getWorld() -> getNachenBlaster();
    return NachenBlaster -> getX() < getX() && abs(NachenBlaster -> getY() - getY()) <= 4;
}

void Alien::flyOneTick() {
    switch (m_flightPlanDirection) {
        case DIR_UP_AND_LEFT:
            moveTo(getX() - m_flightSpeed, getY() + m_flightSpeed);
            m_flightPlanLength--;
            break;
        case DIR_DUE_LEFT:
            moveTo(getX() - m_flightSpeed, getY());
            m_flightPlanLength--;
            break;
        case DIR_DOWN_AND_LEFT:
            moveTo(getX() - m_flightSpeed, getY() - m_flightSpeed);
            m_flightPlanLength--;
            break;
        default:
            std::cerr << "Unknown Flight Direction";
            break;
    }
}

void Alien::newFlightPlan() {
    if (getY() >= VIEW_HEIGHT - 1) {
        m_flightPlanDirection = DIR_DOWN_AND_LEFT;
    } else if (getY() <= 0){
        m_flightPlanDirection = DIR_UP_AND_LEFT;
    } else { // snagglegon will not come into this branch
        int discriminant = randInt(1, 3);
        if (discriminant == 1) {
            m_flightPlanDirection = DIR_UP_AND_LEFT;
        } else if (discriminant == 2) {
            m_flightPlanDirection = DIR_DUE_LEFT;
        } else {
            m_flightPlanDirection = DIR_DOWN_AND_LEFT;
        } // end flight direction selection
    } // end if
    determineFlightPlanLength();
}

bool Alien::willActOn(Actor* target) const {
    return target -> isShip() && target -> isFriendly(); // alien ships can collide with and only with Nachenblaster
}

void Alien::actOn(Actor* target) {
    NachenBlaster * nbp = getWorld() -> getNachenBlaster();
    for (int i = 0; i <= getHealth() / 100; i++) {
        nbp -> sufferDamage(m_damage);
        if (!(nbp -> isAlive())) {
            return;
        }
        
    }
    markForDestruction();
    getWorld() -> increaseScore(m_reward);
    getWorld() -> playSound(SOUND_DEATH);
    getWorld() -> destroyedAlienShip();
    Actor * ap = new Explosion(getWorld(), getX(), getY());
    getWorld() -> addActor(ap);
    attemptToDispenseGoodie();
}

bool Alien::isFriendly() const {
    return false;
}

int Alien::getReward() const {
    return m_reward;
}

void Alien::setFlightPlanLength(int length) {
    m_flightPlanLength = length;
}
void Alien::setFlightSpeed(double speed) {
    m_flightSpeed = speed;
}

void Alien::setFlightPlanDirection(int direction) {
    m_flightPlanDirection = direction;
}

// End class NachenBlaster

// class Smallgon

Smallgon::Smallgon(StudentWorld* world, double startX, double startY) : Alien(world, IID_SMALLGON, startX, startY, 0, 0, 2.0, DIR_DUE_LEFT, 250, 5 ) {
    setHealth(5 * (1 + (getWorld() -> getLevel() - 1) * 0.1 ));
}

void Smallgon::attemptToDispenseGoodie() const {
    return; // Smallgon does not yield goodies.
}

bool Smallgon::AttemptToFire() {
    if (randInt(1, (20 / getWorld() -> getLevel()) + 5) == 1 ) {
        Actor * ap = new Turnip(getWorld(), getX() - 14, getY());
        getWorld() -> addActor(ap);
        getWorld() -> playSound(SOUND_ALIEN_SHOOT);
        return true;
    }
    return false;
}

void Smallgon::determineFlightPlanLength() {
    setFlightPlanLength(randInt(1, 32));
}

// End Smallgon

// class Smoregon

Smoregon::Smoregon(StudentWorld* world, double startX, double startY) : Alien(world, IID_SMOREGON, startX, startY, 0, 0, 2.0, DIR_DUE_LEFT, 250, 5 ) {
    setHealth(5 * (1 + (getWorld() -> getLevel() - 1) * 0.1) );
}

void Smoregon::attemptToDispenseGoodie() const {
    if (randInt(1, 3) == 1) {
        if (randInt(1, 2) == 1) {
            Actor * ap = new RepairGoodie(getWorld(), getX(), getY());
            getWorld() -> addActor(ap);
        } else {
            Actor * ap = new TorpedoGoodie(getWorld(), getX(), getY());
            getWorld() -> addActor(ap);
        }
    }
}

bool Smoregon::AttemptToFire() {
    if (randInt(1, (20 / getWorld() -> getLevel()) + 5) == 1 ) {
        Actor * ap = new Turnip(getWorld(), getX() - 14, getY());
        getWorld() -> addActor(ap);
        getWorld() -> playSound(SOUND_ALIEN_SHOOT);
        return true;
    }
    
    if (randInt(1, (20 / getWorld() -> getLevel()) + 5) == 1) {
        setFlightPlanDirection(DIR_DUE_LEFT);
        setFlightPlanLength(VIEW_WIDTH);
        setFlightSpeed(5);
    }
    return false;
}
void Smoregon::determineFlightPlanLength() {
     setFlightPlanLength(randInt(1, 32));
}

// End class Smoregon

// class Snagglegon

Snagglegon::Snagglegon(StudentWorld* world, double startX, double startY) : Alien(world, IID_SNAGGLEGON, startX, startY, 0, INT_MAX, 1.75, DIR_DOWN_AND_LEFT, 1000, 15 ) { // is this down and Left?
    setHealth(10 * (1 + (getWorld() -> getLevel() - 1) * 0.1 ));
}

void Snagglegon::attemptToDispenseGoodie() const {
    if (randInt(1, 6) == 1) {
        Actor * ap = new ExtraLifeGoodie(getWorld(), getX(), getY());
        getWorld() -> addActor(ap);
    }
}

bool Snagglegon::AttemptToFire() {
    if (randInt(1, (15 / getWorld() -> getLevel()) + 10) == 1 ) {
        Actor * ap = new Torpedo(getWorld(), getX() - 14, getY(), 180);
        getWorld() -> addActor(ap);
        getWorld() -> playSound(SOUND_TORPEDO);
        return true;
    }
    return false;
}

void Snagglegon::determineFlightPlanLength() {
    setFlightPlanLength(INT_MAX);
}

// End class Snagglegon

// class Projectile

Projectile::Projectile(StudentWorld* world, int imageID, double startX, double startY, int direction, int damage, int speed) : Actor(world, imageID, startX, startY, direction, 0.5, 1), m_damage(damage), m_speed(speed) {
    
}
void Projectile::doSomething(){
    if (!isAlive()) {
        return;
    }
    if (!isOnScreen()) {
        markForDestruction();
        return;
    }
    // interaction with ships, which will be handled in student world;
    
    if (isFriendly()) {
        moveTo(getX() + m_speed, getY());
    } else {
        moveTo(getX() - m_speed, getY());
    }
    attemptToRotate();
}

bool Projectile::willActOn(Actor* target) const {
    return target -> isShip() && (target -> isFriendly() != isFriendly()); // a projectile will collide with a ship that is not on its side.
}

void Projectile::actOn(Actor* target) {
    Ship * sp = dynamic_cast<Ship*>(target);
    sp -> sufferDamage(m_damage);
    markForDestruction();
}
bool Projectile::isShip() const {
    return false;
}
bool Projectile::isProjectile() const {
    return true;
}
bool Projectile::isGoodie() const {
    return false;
}
bool Projectile::isInteractive() const {
    return true;
}

// end class Projectile

// class Cabbage

Cabbage::Cabbage(StudentWorld* world, int x, int y) : Projectile(world, IID_CABBAGE, x, y, 0, 2, 8) {
    
}

bool Cabbage::isFriendly() const {
    return true;
}

void Cabbage::attemptToRotate() {
    setDirection(getDirection() + 20);
}

// end class Cabbage

// class Turnip

Turnip::Turnip(StudentWorld* world, double startX, double startY) : Projectile(world, IID_TURNIP, startX, startY, 180, 2, 6){
    
}

bool Turnip::isFriendly() const {
    return false;
}

void Turnip::attemptToRotate() {
    setDirection(getDirection() + 20);
}

// end Turnip

// class Torpedo

Torpedo::Torpedo(StudentWorld* world, double startX, double startY, int direction) : Projectile(world, IID_TORPEDO, startX, startY, direction, 8, 8) {
    
}

bool Torpedo::isFriendly() const {
    return getDirection() == 0;
}

void Torpedo::attemptToRotate() {
    return;
}

// end Torpedo

// class Goodie

Goodie::Goodie(StudentWorld* world, int imageID, double startX, double startY) : Actor(world, imageID, startX, startY, 0, 0.5, 1, true)  {
    
}

void Goodie::doSomething() {
    if (!isAlive()) {
        return;
    }
    
    // collision handled by student world
    
    moveTo(getX() - 0.75, getY() - 0.75);
    
    // collision handled again.
}

bool Goodie::willActOn(Actor* target) const {
    return target -> isShip() && target -> isFriendly(); // the only friendly ship is Nachenblaster, as desired
}

void Goodie::actOn(Actor* target) {
    getWorld() -> increaseScore(100);
    markForDestruction();
    getWorld() -> playSound(SOUND_GOODIE);
    dispenseReward();
}

bool Goodie::isProjectile() const {
    return false;
}
bool Goodie::isShip() const {
    return false;
}
bool Goodie::isGoodie() const  {
    return true;
}
bool Goodie::isFriendly() const {
    return false; // isGoodie returning true is sufficient for identifying all sorts of goodies.
}
bool Goodie::isInteractive() const {
    return true;
}

// End class Goodie

// class ExtraLifeGoodie

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* world, double startX, double startY) : Goodie(world, IID_LIFE_GOODIE, startX, startY) {
    
}

void ExtraLifeGoodie::dispenseReward() {
    getWorld() -> incLives();
}

// End ExtraLifeGoodie


// class RepairGoodie

RepairGoodie::RepairGoodie(StudentWorld* world, double startX, double startY) : Goodie(world, IID_REPAIR_GOODIE, startX, startY) {
    
}

void RepairGoodie::dispenseReward() {
    NachenBlaster * nbp = getWorld() -> getNachenBlaster();
    int currHealth = nbp -> getHealth();
    if (currHealth < 40) {
        nbp -> setHealth(currHealth + 10);
    } else {
        nbp -> setHealth(50);
    }
}

// end class RepairGoodie

// class TorpedoGoodie

TorpedoGoodie::TorpedoGoodie(StudentWorld* world, double startX, double startY) : Goodie(world, IID_TORPEDO_GOODIE, startX, startY) {
    
}

void TorpedoGoodie::dispenseReward() {
    getWorld() -> getNachenBlaster() -> incTorpedoCount();
}

// end class TorpedoGoodie


// class NonInteractive

NonInteractive::NonInteractive(StudentWorld* world, int imageID, double startX, double startY, double size, int depth) : Actor(world, imageID, startX, startY, 0, size, depth) {
    
}

bool NonInteractive::isShip() const {
    return false;
}
bool NonInteractive::isProjectile() const {
    return false;
}
bool NonInteractive::isGoodie() const {
    return false;
}
bool NonInteractive::isFriendly() const {
    return false;
}
bool NonInteractive::isInteractive() const {
    return false;
}
bool NonInteractive::willActOn(Actor* target) const {
    return false;
}
void NonInteractive::actOn(Actor* target) {
    return; // this funciton will never be called.
}

// end class NonInteractive

// class Star

Star::Star(StudentWorld* world, double startX, double startY) : NonInteractive(world, IID_STAR, startX, startY, randInt(5,50) / 100.0, 3  ) {
}

void Star::doSomething() {
    moveTo(getX() - 1, getY());
    if (getX() < 0) {
        markForDestruction();
    }
}

// end class Star

// class Explosion

Explosion::Explosion(StudentWorld* world, double startX, double startY) : NonInteractive(world, IID_EXPLOSION, startX, startY, 1, 0) {
    m_age = 0;
}

void Explosion::doSomething() {
    setSize(getSize() * 1.5);
    m_age++;
    if (m_age == 4) {
        markForDestruction();
    }
}

// end class Explosion
