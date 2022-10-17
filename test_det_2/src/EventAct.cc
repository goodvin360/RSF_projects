#include "EventAct.hh"

EventAct::EventAct(RunAct *runAct):run(runAct)
{

}

EventAct::~EventAct()
{

}

void EventAct::BeginOfEventAction(const G4Event *anEvent)
{
    EnergyDep = 0;
    PhotonNum = 0;
}

void EventAct::EndOfEventAction(const G4Event *anEvent)
{
    if (EnergyDep>0)
        run->AddEvent(EnergyDep);

    if (PhotonNum>0)
        run->AddEvent2(PhotonNum);
}


void EventAct::AddEnDep(G4double en)
{
    EnergyDep += en;
}

void EventAct::AddPhoton(G4double num)
{
    PhotonNum += num;
}

