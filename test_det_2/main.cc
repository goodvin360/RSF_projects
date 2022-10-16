#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else

#include <G4ParticleHPManager.hh>
#include "G4RunManager.hh"
#endif
#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "DetGeometry.hh"
#include "ActionInit.hh"
#include "PrimaryGen.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "iostream"
#include "QGSP_BERT_HP.hh"
#include "ShieldingLEND.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "QBBC.hh"

using namespace std;

int main(int argc, char** argv) {

    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    CLHEP::HepRandom::setTheSeed(time(NULL));

    #ifdef G4MULTITHREADED
    G4MTRunManager* runManager;
    #else
    G4RunManager* runManager;
    #endif
    DetGeometry *detGeom;

#ifdef G4MULTITHREADED
    runManager = new G4MTRunManager;
    G4int nThreads = G4Threading::G4GetNumberOfCores();
    if (argc==3) nThreads = G4UIcommand::ConvertToInt(argv[2]);
    runManager->SetNumberOfThreads(nThreads);
#else
    runManager = new G4RunManager;
#endif

//Set mandatory initialization classes
    detGeom = new DetGeometry();
    runManager->SetUserInitialization(detGeom);
    auto Phys = new QBBC;
    Phys->RegisterPhysics(new G4RadioactiveDecayPhysics);
    Phys->ReplacePhysics(new G4EmStandardPhysics_option4);
    G4ParticleHPManager::GetInstance()->SetUseNRESP71Model( true );
    G4ParticleHPManager::GetInstance()->SetNeglectDoppler(true);
//    G4ParticleHPManager::GetInstance()->SetSkipMissingIsotopes( true );
//    G4ParticleHPManager::GetInstance()->SetDoNotAdjustFinalState( true );
//    G4ParticleHPManager::GetInstance()->SetUseOnlyPhotoEvaporation( true );
//    G4ParticleHPManager::GetInstance()->SetProduceFissionFragments( true );
//    G4ParticleHPManager::GetInstance()->SetUseWendtFissionModel( true );
    runManager->SetUserInitialization(Phys);
    runManager->SetUserInitialization(new ActionInit(detGeom));
    runManager->Initialize();

    // Initialize visualization
    G4VisManager*visManager = new G4VisExecutive;
    visManager->Initialize();

    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    G4UIExecutive* ui = 0;
    if ( argc == 1 ) {
        ui = new G4UIExecutive(argc, argv);
    }

    if (! ui) {
        // batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }
    else {
        // interactive mode : define UI session
        UImanager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
    }

    delete ui;
    delete visManager;
    delete runManager;

    return 0;
}