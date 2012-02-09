#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>

namespace Langmuir
{

class World;
class Grid;
class Agent;
class Potential;
class DrainAgent;
class SourceAgent;
class ChargeAgent;
struct SimulationParameters;

class Simulation : public QObject
{
private:
    Q_OBJECT
    Q_DISABLE_COPY(Simulation)

public:
    Simulation(World &world, QObject *parent = 0);
    virtual ~Simulation();
    virtual void performIterations(int nIterations);
    void equilibrated();

protected:

    virtual void initializeSites();
    virtual void placeDefects(const QList<int>& siteIDs = QList<int>());
    virtual void placeElectrons(const QList<int>& siteIDs = QList<int>());
    virtual void placeHoles(const QList<int>& siteIDs = QList<int>());

    void performInjections();
    void nextTick();
    static void chargeAgentIterate(ChargeAgent * chargeAgent);
    static void chargeAgentChooseFuture(ChargeAgent * chargeAgent);
    static void chargeAgentDecideFuture(ChargeAgent * chargeAgent);
    World &m_world;
};

}

#endif
