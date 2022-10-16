#include "DetGeometry.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RotationMatrix.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "CommandMessenger.hh"
#include "G4GeometryManager.hh"
#include "CADMesh.hh"


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

    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");


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

    G4Material*mat_1 = nist->FindOrBuildMaterial("G4_Al");

    G4Material*mat_2 = nist->FindOrBuildMaterial("G4_WATER");

    G4Material*mat_3 = nist->FindOrBuildMaterial("G4_Cu");
    
    G4Material*mat_4 = new G4Material("Compound", 5*g/cm3, 2);
    mat_4->AddMaterial(mat_1, 21*perCent);
    mat_4->AddMaterial(mat_3, 79*perCent);

    G4Material*mat_5 = nist->FindOrBuildMaterial(material_name);

    G4Element*el_La = new G4Element("Lanthanum", "La", 57, 138.9*g/mole);
    G4Element*el_Br = new G4Element("Bromum", "Br", 35, 79.904*g/mole);
    G4Material*LaBr3_mat = new G4Material("LaBr3_mat", 5.057*g/cm3, 2);
    LaBr3_mat->AddElement(el_La, 1);
    LaBr3_mat->AddElement(el_Br, 3);

    G4Isotope*B10 = new G4Isotope("B10", 5, 10, 10.012*g/mole);
    G4Isotope*B11 = new G4Isotope("B11", 5, 11, 11.009*g/mole);
    G4Element* elBoron = new G4Element("Boron", "B", 2);
    elBoron->AddIsotope(B10, 18.5*perCent);
    elBoron->AddIsotope(B11, 81.5*perCent);
    G4Material*Boron_mat = new G4Material("Boron_mat", 2.34*g/cm3, 1, kStateSolid, 300*kelvin, 1*atmosphere);
    Boron_mat->AddElement(elBoron, 1);


    //define distances

    G4double tube_R = R_size;

    //define body number 1
    G4ThreeVector Body_No_1_pos = G4ThreeVector(0, 0, 0);
    G4Tubs*Body_No_1 = new G4Tubs("Body_No_1", 0*cm, 2.5*cm, 1.5*cm, 0*deg, 360*deg);
    G4LogicalVolume*logicBody_No_1 = new G4LogicalVolume(Body_No_1, nist->FindOrBuildMaterial("G4_Ge"), "Body_No_1");
    G4VisAttributes*logicVisBody_No_1 = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
    logicBody_No_1->SetVisAttributes(logicVisBody_No_1);
    new G4PVPlacement(0,
                      Body_No_1_pos,
                      logicBody_No_1,
                      "Body_No_1",
                      logicWorld,
                      false,
                      0);



    G4ThreeVector Body_No_2_pos = G4ThreeVector(0, 0*cm, 0);
    G4Tubs*Body_No_2_0 = new G4Tubs("Body_No_2_0", 0*cm, 2.5*cm, 1.5*cm, 0*deg, 360*deg);
    G4Tubs*Body_No_2_1 = new G4Tubs("Body_No_2_1", 0*cm, 2.7*cm, 1.7*cm, 0*deg, 360*deg);
    G4SubtractionSolid*Body_No_2 = new G4SubtractionSolid("Body_No_2", Body_No_2_1, Body_No_2_0);
    G4LogicalVolume*logicBody_No_2 = new G4LogicalVolume(Body_No_2, nist->FindOrBuildMaterial("G4_Al"), "Body_No_2");
    G4VisAttributes*logicVisBody_No_2 = new G4VisAttributes(G4Colour(1.0, 0.0, 1.0));
    logicBody_No_2->SetVisAttributes(logicVisBody_No_2);
    new G4PVPlacement(0,
                      Body_No_2_pos,
                      logicBody_No_2,
                      "Body_No_2",
                      logicWorld,
                      false,
                      0);

    G4ThreeVector Body_No_3_pos = G4ThreeVector(0, 0*cm, 13.7*cm);
    G4Tubs*Body_No_3 = new G4Tubs("Body_No_3", 0*cm, 1*cm, 0.05*cm, 0*deg, 360*deg);
    G4LogicalVolume*logicBody_No_3 = new G4LogicalVolume(Body_No_3, nist->FindOrBuildMaterial("G4_NYLON-6-6"), "Body_No_3");
    G4VisAttributes*logicVisBody_No_3 = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
    logicBody_No_3->SetVisAttributes(logicVisBody_No_3);
    new G4PVPlacement(0,
                      Body_No_3_pos,
                      logicBody_No_3,
                      "Body_No_3",
                      logicWorld,
                      false,
                      0);

//    G4RotationMatrix*my_rot = new G4RotationMatrix ();
//    my_rot->rotateX(90*deg);
//    my_rot->rotateY(0*deg);
//    my_rot->rotateZ(0*deg);
//    G4ThreeVector Body_No_4_pos = G4ThreeVector(0, 0*cm, 15*cm);
//    G4Tubs*Body_No_4 = new G4Tubs("Body_No_4", 0*cm, 5*cm, 5*cm, 0*deg, 360*deg);
//    G4LogicalVolume*logicBody_No_4 = new G4LogicalVolume(Body_No_4, nist->FindOrBuildMaterial("G4_Al"), "Body_No_4");
//    G4VisAttributes*logicVisBody_No_4 = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0));
//    logicBody_No_4->SetVisAttributes(logicVisBody_No_4);
//    new G4PVPlacement(my_rot,
//                      Body_No_4_pos,
//                      logicBody_No_4,
//                      "Body_No_4",
//                      logicWorld,
//                      false,
//                      0);

    /* G4ThreeVector Body_No_5_pos = G4ThreeVector(0, 0*cm, 0);
     G4Tubs*Body_No_5 = new G4Tubs("Body_No_5", 10*cm, 10.1*cm, 10*cm, 0*deg, 360*deg);
     G4LogicalVolume*logicBody_No_5 = new G4LogicalVolume(Body_No_5, nist->FindOrBuildMaterial("G4_Al"), "Body_No_5");
     G4VisAttributes*logicVisBody_No_5 = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
     logicBody_No_5->SetVisAttributes(logicVisBody_No_5);
     new G4PVPlacement(0,
                       Body_No_5_pos,
                       logicBody_No_5,
                       "Body_No_5",
                       logicWorld,
                       false,
                       0);*/


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
