#include "StepAct.hh"
#include "DetGeometry.hh"


StepAct::StepAct(DetGeometry*det, EventAct *eventAct):event(eventAct), fDetector(det)
{

}

void StepAct::UserSteppingAction(const G4Step *aStep)

{

    if (aStep->GetTrack()->GetVolume()->GetName()=="Body_No_11"
        && aStep->GetTrack()->GetParticleDefinition()->GetParticleName()=="opticalphoton"
//            && aStep->GetTrack()->GetCurrentStepNumber()==1
        && aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary
        && aStep->IsFirstStepInVolume()
            )

    {
        event->AddPhoton(1);
    };

    if (aStep->GetTrack()->GetVolume()->GetName()=="Body_No_9")

    {
        event->AddEnDep(aStep->GetTotalEnergyDeposit());
    };


}

