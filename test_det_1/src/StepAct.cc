#include "StepAct.hh"
#include "DetGeometry.hh"


StepAct::StepAct(DetGeometry*det, EventAct *eventAct):event(eventAct), fDetector(det)
{

}

void StepAct::UserSteppingAction(const G4Step *aStep)

{

//    if (aStep->GetTrack()->GetVolume()->GetName()=="Body_No_1"
//    &&aStep->GetTrack()->GetParticleDefinition()->GetParticleName()=="geantino"
//    &&aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary
//    &&aStep->IsFirstStepInVolume())
//
//    {
//        event->AddEnDep(aStep->GetTrack()->GetKineticEnergy());
//    };

    if (aStep->GetTrack()->GetVolume()->GetName()=="Body_No_1"
//    &&aStep->GetTrack()->GetGlobalTime()>=10*s
    )

    {
        event->AddEnDep(aStep->GetTotalEnergyDeposit());
    };


}

