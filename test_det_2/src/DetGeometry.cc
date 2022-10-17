#include "DetGeometry.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RotationMatrix.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "CommandMessenger.hh"
#include "G4GeometryManager.hh"
#include "CADMesh.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "vector"

using namespace std;

DetGeometry::DetGeometry()
{
    geometryMessenger = new GeometryMessenger(this);
}

DetGeometry::~DetGeometry()
{
    delete geometryMessenger;
}

G4VPhysicalVolume* DetGeometry::Construct(){

    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    G4NistManager* nist = G4NistManager::Instance();

    G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");


    G4Box* solidWorld = new G4Box("World", 0.5*m, 0.5*m, 0.5*m);
    G4LogicalVolume*logicWorld = new G4LogicalVolume(solidWorld, world_mat,"World");
    G4VisAttributes*nva = new G4VisAttributes();
    nva->SetVisibility(false);
    logicWorld->SetVisAttributes(nva);
    G4VPhysicalVolume* physWorld =
            new G4PVPlacement(0,
                              G4ThreeVector(),
                              logicWorld,
                              "World",
                              0,
                              false,
                              0);

    //define materials

    G4Material*aluminium_mat = nist->FindOrBuildMaterial("G4_Al");
//    G4Material*det_mat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    G4Material*det_mat = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    G4Material*reflector_mat = nist->FindOrBuildMaterial("G4_MYLAR");
    G4Material*counter_mat = nist->FindOrBuildMaterial("G4_AIR");


    G4Material*mat_5 = nist->FindOrBuildMaterial(material_name);

    //define distances

    d_cont = 0.3*um;
    d_det = 0.03*cm;
    l_det = 0.3*cm;
    l_cont = 0.3*cm;

    G4double tube_R = R_size;

    det_X = 0;
    det_Y = 0;
    det_Z = 0;

    G4ThreeVector Body_No_9_pos = G4ThreeVector(det_X*cm, det_Y*cm, (det_Z)*cm);
    G4Tubs*Body_No_9 = new G4Tubs("Body_No_9", 0*cm, 3.7*cm, 3.7*cm, 0*deg, 360*deg);
    G4LogicalVolume*logicBody_No_9 = new G4LogicalVolume(Body_No_9, det_mat, "Body_No_9");
    G4VisAttributes*logicVisBody_No_9 = new G4VisAttributes(G4Colour(1, 0, 0));
    logicBody_No_9->SetVisAttributes(logicVisBody_No_9);
    G4PVPlacement*Body_No_9_phys = new G4PVPlacement(0,
                                                     Body_No_9_pos,
                                                     logicBody_No_9,
                                                     "Body_No_9",
                                                     logicWorld,
                                                     false,
                                                     0);

    G4ThreeVector Body_No_10_pos = G4ThreeVector(det_X*cm, det_Y*cm, (det_Z+0.05)*cm);
    G4Tubs*Body_No_10_1 = new G4Tubs("Body_No_10_1", 0*cm, 3.7*cm, 3.75*cm, 0*deg, 360*deg);
    G4Tubs*Body_No_10_2 = new G4Tubs("Body_No_10_2", 0*cm, 3.8*cm, 3.75*cm, 0*deg, 360*deg);
    G4SubtractionSolid*Body_No_10 = new G4SubtractionSolid("Body_No_10", Body_No_10_2, Body_No_10_1, 0, G4ThreeVector(0,0,-1));
    G4LogicalVolume*logicBody_No_10 = new G4LogicalVolume(Body_No_10, reflector_mat, "Body_No_10");
    G4VisAttributes*logicVisBody_No_10 = new G4VisAttributes(G4Colour(1, 1, 0));
    logicBody_No_10->SetVisAttributes(logicVisBody_No_10);
    G4PVPlacement*Body_No_10_phys = new G4PVPlacement(0,
                                                      Body_No_10_pos,
                                                      logicBody_No_10,
                                                      "Body_No_10",
                                                      logicWorld,
                                                      false,
                                                      0);

    G4ThreeVector Body_No_11_pos = G4ThreeVector(det_X*cm, det_Y*cm, (det_Z-3.8)*cm);
    G4Tubs*Body_No_11 = new G4Tubs("Body_No_11", 0*cm, 3.8*cm, 0.1*cm, 0*deg, 360*deg);
    G4LogicalVolume*logicBody_No_11 = new G4LogicalVolume(Body_No_11, counter_mat, "Body_No_11");
    G4VisAttributes*logicVisBody_No_11 = new G4VisAttributes(G4Colour(1, 0, 1));
    logicBody_No_11->SetVisAttributes(logicVisBody_No_11);
    G4PVPlacement*Body_No_11_phys = new G4PVPlacement(0,
                                                      Body_No_11_pos,
                                                      logicBody_No_11,
                                                      "Body_No_11",
                                                      logicWorld,
                                                      false,
                                                      0);

    //set scint properties

    G4double photonEnergy[] =
            { 2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV,
              2.177*eV, 2.216*eV, 2.256*eV, 2.298*eV,
              2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV,
              2.532*eV, 2.585*eV, 2.640*eV, 2.697*eV,
              2.757*eV, 2.820*eV, 2.885*eV, 2.954*eV,
              3.026*eV, 3.102*eV, 3.181*eV, 3.265*eV,
              3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
              3.760*eV, 3.877*eV, 4.002*eV, 4.136*eV };

    const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);

//
// Diamond
//
    G4double refractiveIndex1[] =
            { 1.3435, 1.344,  1.3445, 1.345,  1.3455,
              1.346,  1.3465, 1.347,  1.3475, 1.348,
              1.3485, 1.3492, 1.35,   1.3505, 1.351,
              1.3518, 1.3522, 1.3530, 1.3535, 1.354,
              1.3545, 1.355,  1.3555, 1.356,  1.3568,
              1.3572, 1.358,  1.3585, 1.359,  1.3595,
              1.36,   1.3608};

    assert(sizeof(refractiveIndex1) == sizeof(photonEnergy));

    G4double absorption[] =
            {3.448*m,  4.082*m,  6.329*m,  9.174*m, 12.346*m, 13.889*m,
             15.152*m, 17.241*m, 18.868*m, 20.000*m, 26.316*m, 35.714*m,
             45.455*m, 47.619*m, 52.632*m, 52.632*m, 55.556*m, 52.632*m,
             52.632*m, 47.619*m, 45.455*m, 41.667*m, 37.037*m, 33.333*m,
             30.000*m, 28.500*m, 27.000*m, 24.500*m, 22.000*m, 19.500*m,
             17.500*m, 14.500*m };

    assert(sizeof(absorption) == sizeof(photonEnergy));

    G4double scintilFast[] =
            { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00 };

    assert(sizeof(scintilFast) == sizeof(photonEnergy));

    G4double scintilSlow[] =
            { 0.01, 1.00, 2.00, 3.00, 4.00, 5.00, 6.00,
              7.00, 8.00, 9.00, 8.00, 7.00, 6.00, 4.00,
              3.00, 2.00, 1.00, 0.01, 1.00, 2.00, 3.00,
              4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 8.00,
              7.00, 6.00, 5.00, 4.00 };

    assert(sizeof(scintilSlow) == sizeof(photonEnergy));

    G4MaterialPropertiesTable* myMPT1 = new G4MaterialPropertiesTable();

    myMPT1->AddProperty("RINDEX",       photonEnergy, refractiveIndex1, nEntries)->SetSpline(true);
    myMPT1->AddProperty("ABSLENGTH",    photonEnergy, absorption,       nEntries)->SetSpline(true);
    myMPT1->AddProperty("FASTCOMPONENT",photonEnergy, scintilFast,      nEntries)->SetSpline(true);
//    myMPT1->AddProperty("SLOWCOMPONENT",photonEnergy, scintilSlow,      nEntries)->SetSpline(true);


    std::vector<G4double> particleEnergy     = {0.0001*MeV, 10*MeV};

    std::vector<G4double> scintYieldElectron = {0., 10000.};

    std::vector<G4double> scintYieldProton   = {0., 10000.};

    std::vector<G4double> scintYieldDeuteron = {0., 10000.};

    std::vector<G4double> scintYieldTriton   = {0., 10000.};

    std::vector<G4double> scintYieldAlpha    = {0., 10000.};

    std::vector<G4double> scintYieldCarbon   = {0., 10000.};


    myMPT1->AddProperty("ELECTRONSCINTILLATIONYIELD", particleEnergy, scintYieldElectron);
    myMPT1->AddProperty("PROTONSCINTILLATIONYIELD", particleEnergy, scintYieldProton);
    myMPT1->AddProperty("DEUTERONSCINTILLATIONYIELD", particleEnergy, scintYieldDeuteron);
    myMPT1->AddProperty("TRITONSCINTILLATIONYIELD", particleEnergy, scintYieldTriton);
    myMPT1->AddProperty("ALPHASCINTILLATIONYIELD", particleEnergy, scintYieldAlpha);
    myMPT1->AddProperty("IONSCINTILLATIONYIELD", particleEnergy, scintYieldCarbon);

    myMPT1->AddConstProperty("ELECTRONSCINTILLATIONYIELD1",0.87); //Relative yield of component 1 for electrons
    myMPT1->AddConstProperty("ELECTRONSCINTILLATIONYIELD2",0.13); //Relative yield of component 2
    myMPT1->AddConstProperty("ELECTRONSCINTILLATIONYIELD3",0.001); //Relative yield of component 3

    myMPT1->AddConstProperty("PROTONSCINTILLATIONYIELD1",0.80); //Relative yield of component 1 for protons (estimate from literature)
    myMPT1->AddConstProperty("PROTONSCINTILLATIONYIELD2",0.20); //Relative yield of component 2
    myMPT1->AddConstProperty("PROTONSCINTILLATIONYIELD3",0.001); //Relative yield of component 3

    myMPT1->AddConstProperty("DEUTERONSCINTILLATIONYIELD1",0.80); //Relative yield of component 1 for deuterons (use proton value)
    myMPT1->AddConstProperty("DEUTERONSCINTILLATIONYIELD2",0.20); //Relative yield of component 2
    myMPT1->AddConstProperty("DEUTERONSCINTILLATIONYIELD3",0.001); //Relative yield of component 3

    myMPT1->AddConstProperty("TRITONSCINTILLATIONYIELD1",0.80); //Relative yield of component 1 for tritons (use proton value)
    myMPT1->AddConstProperty("TRITONSCINTILLATIONYIELD2",0.20); //Relative yield of component 2
    myMPT1->AddConstProperty("TRITONSCINTILLATIONYIELD3",0.001); //Relative yield of component 3

    myMPT1->AddConstProperty("ALPHASCINTILLATIONYIELD1",0.74); //Relative yield of component 1 for alphas (estimate from literature)
    myMPT1->AddConstProperty("ALPHASCINTILLATIONYIELD2",0.26); //Relative yield of component 2
    myMPT1->AddConstProperty("ALPHASCINTILLATIONYIELD3",0.001); //Relative yield of component 3

    myMPT1->AddConstProperty("IONSCINTILLATIONYIELD1",0.74); //Relative yield of component 1 for carbon ions(use alpha value)
    myMPT1->AddConstProperty("IONSCINTILLATIONYIELD2",0.26); //Relative yield of component 2
    myMPT1->AddConstProperty("IONSCINTILLATIONYIELD3",0.001); //Relative yield of component 3

//    myMPT1->AddConstProperty("SCINTILLATIONYIELD",1000/MeV);
    myMPT1->AddConstProperty("RESOLUTIONSCALE",2.5);
    myMPT1->AddConstProperty("FASTTIMECONSTANT", 1.*ns);
//    myMPT1->AddConstProperty("SLOWTIMECONSTANT",10.*ns);
    myMPT1->AddConstProperty("YIELDRATIO",0.8);

    det_mat->SetMaterialPropertiesTable(myMPT1);

    // Set the Birks Constant for the Water scintillator
//    diam_mat->GetIonisation()->SetBirksConstant(0.001*mm/MeV);

//
// World
//
    G4double refractiveIndex2[] =
            { 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
              1.00, 1.00, 1.00, 1.00 };

    G4double absorption2[] =
            {100*m,  100*m,  100*m,  100*m, 100*m, 100*m,
             100*m, 100*m, 100*m, 100*m, 100*m, 100*m,
             100*m, 100*m, 100*m, 100*m, 100*m, 100*m,
             100*m, 100*m, 100*m, 100*m, 100*m, 100*m,
             100*m, 100*m, 100*m, 100*m, 100*m, 100*m,
             100*m, 100*m };

    G4MaterialPropertiesTable* myMPT2 = new G4MaterialPropertiesTable();
    myMPT2->AddProperty("RINDEX", photonEnergy, refractiveIndex2, nEntries)->SetSpline(true);
    myMPT2->AddProperty("ABSLENGTH", photonEnergy, absorption2, nEntries)->SetSpline(true);

    world_mat->SetMaterialPropertiesTable(myMPT2);

//
// reflector
//
    G4double refractiveIndex3[] =
            { 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
              1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
              1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
              1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
              1.5, 1.5, 1.5, 1.5 };

    G4double absorption3[] =
            {3.448*mm,  4.082*mm,  6.329*mm,  9.174*mm, 12.346*mm, 13.889*mm,
             15.152*mm, 17.241*mm, 18.868*mm, 20.000*mm, 26.316*mm, 35.714*mm,
             45.455*mm, 47.619*mm, 52.632*mm, 52.632*mm, 55.556*mm, 52.632*mm,
             52.632*mm, 47.619*mm, 45.455*mm, 41.667*mm, 37.037*mm, 33.333*mm,
             30.000*mm, 28.500*mm, 27.000*mm, 24.500*mm, 22.000*mm, 19.500*mm,
             17.500*mm, 14.500*mm };

    G4MaterialPropertiesTable* myMPT3 = new G4MaterialPropertiesTable();
    myMPT3->AddProperty("RINDEX", photonEnergy, refractiveIndex3, nEntries)->SetSpline(true);
    myMPT3->AddProperty("ABSLENGTH", photonEnergy, absorption3, nEntries)->SetSpline(true);

    reflector_mat->SetMaterialPropertiesTable(myMPT3);

//
// counter
//
    G4double refractiveIndex4[] =
            { 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
              1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
              1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
              1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
              1.5, 1.5, 1.5, 1.5 };

    G4double absorption4[] =
            {3.448*m,  4.082*m,  6.329*m,  9.174*m, 12.346*m, 13.889*m,
             15.152*m, 17.241*m, 18.868*m, 20.000*m, 26.316*m, 35.714*m,
             45.455*m, 47.619*m, 52.632*m, 52.632*m, 55.556*m, 52.632*m,
             52.632*m, 47.619*m, 45.455*m, 41.667*m, 37.037*m, 33.333*m,
             30.000*m, 28.500*m, 27.000*m, 24.500*m, 22.000*m, 19.500*m,
             17.500*m, 14.500*m };

    G4MaterialPropertiesTable* myMPT4 = new G4MaterialPropertiesTable();
    myMPT4->AddProperty("RINDEX", photonEnergy, refractiveIndex4, nEntries)->SetSpline(true);
    myMPT4->AddProperty("ABSLENGTH", photonEnergy, absorption4, nEntries)->SetSpline(true);

    counter_mat->SetMaterialPropertiesTable(myMPT4);


//
// Contact surface with detector
//
    G4double ref = 0.0;
    G4double refs[] =
            { ref, ref, ref, ref, ref, ref, ref,
              ref, ref, ref, ref, ref, ref, ref,
              ref, ref, ref, ref, ref, ref, ref,
              ref, ref, ref, ref, ref, ref, ref,
              ref, ref, ref, ref };

    G4OpticalSurface*fWorldContSurface_1;
    fWorldContSurface_1 = new G4OpticalSurface( "Cont_1_Surface" );
    G4LogicalBorderSurface* WorldContSurface_1 = new G4LogicalBorderSurface( "Cont_1_Surface",
                                                                             Body_No_11_phys,
                                                                             physWorld,
                                                                             fWorldContSurface_1);
    G4double sigma_alpha_1 = 0.1;
    fWorldContSurface_1->SetType( G4SurfaceType::dielectric_dielectric);
    fWorldContSurface_1->SetModel( G4OpticalSurfaceModel::unified);
    fWorldContSurface_1->SetFinish(ground);
//    fWorldContSurface_1->SetSigmaAlpha(sigma_alpha_1);

    G4MaterialPropertiesTable* sur_1 = new G4MaterialPropertiesTable();
    sur_1->AddProperty( "REFLECTIVITY", photonEnergy, refs, nEntries )->SetSpline( true );
    fWorldContSurface_1->SetMaterialPropertiesTable(sur_1);



    G4OpticalSurface* fDetRefSurface = new G4OpticalSurface("Det_Surface");
    G4LogicalBorderSurface* WorldDetSurface = new G4LogicalBorderSurface("Det_Surface",
                                                                         Body_No_9_phys,
                                                                         Body_No_10_phys,
                                                                         fDetRefSurface);
    G4double ref3 = 0.99;
    G4double refs3[] =
            { ref3, ref3, ref3, ref3, ref3, ref3, ref3,
              ref3, ref3, ref3, ref3, ref3, ref3, ref3,
              ref3, ref3, ref3, ref3, ref3, ref3, ref3,
              ref3, ref3, ref3, ref3, ref3, ref3, ref3,
              ref3, ref3, ref3, ref3 };

    G4double sigma_alpha_3 = 0.1;
    fDetRefSurface->SetType(dielectric_metal);
    fDetRefSurface->SetModel(G4OpticalSurfaceModel::unified);
    fDetRefSurface->SetFinish(polished);
//    fWorldDetSurface->SetSigmaAlpha(sigma_alpha_3);

    G4MaterialPropertiesTable* sur_3 = new G4MaterialPropertiesTable();
    sur_3->AddProperty( "REFLECTIVITY", photonEnergy, refs3, nEntries )->SetSpline( true );
    fDetRefSurface->SetMaterialPropertiesTable(sur_3);

    return physWorld;
}

void DetGeometry::SetNewMat(G4String newName) {
    material_name = newName;
    G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetGeometry::SetNewSize(G4double newSize) {
    R_size = newSize;
    G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}
