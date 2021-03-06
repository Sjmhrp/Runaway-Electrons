#include "G4RunManager.hh"
#include "G4VisExecutive.hh"
#include "G4VVisManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Electron.hh"
#include "G4MuonMinus.hh"
#include "G4ParticleGun.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4SystemOfUnits.hh"
#include "QBBC.hh"

class BasicDetectorConstruction : public G4VUserDetectorConstruction {
public:
	G4VPhysicalVolume * Construct() {
		G4double worldSize = 20*cm;
		G4double targetSize = 5*cm;
		G4Box* solidWorld = new G4Box("World",worldSize,worldSize,worldSize);
		G4Box* solidTarget = new G4Box("Target",targetSize,targetSize,targetSize);
		G4NistManager* nist = G4NistManager::Instance();
		G4Material* Pb = nist->FindOrBuildMaterial("G4_Pb");
		G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
		G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, air, "World");
		G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, Pb, "Target");
		G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);
		G4VPhysicalVolume* physTarget = new G4PVPlacement(0, G4ThreeVector(10*cm,0,0), logicTarget, "Target", logicWorld, false, 0, true);
		return physWorld;
	}
};

class BasicGenerator : public G4VUserPrimaryGeneratorAction {
public:
	void GeneratePrimaries(G4Event* event) {
		G4ParticleGun* gun = new G4ParticleGun;
		gun->SetParticleDefinition(G4Electron::ElectronDefinition());
		gun->SetParticleEnergy(100*MeV);
		gun->SetParticleMomentumDirection(G4ThreeVector(1,0,0));
		gun->GeneratePrimaryVertex(event);
	}
};

int main(int argc,char** argv) {
	G4RunManager* runManager = new G4RunManager;
	runManager->SetUserInitialization(new BasicDetectorConstruction);
	runManager->SetUserInitialization(new QBBC);
	runManager->SetUserAction(new BasicGenerator);
	runManager->Initialize();
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
	G4UImanager* ui = G4UImanager::GetUIpointer();
	G4VVisManager* vis = G4VVisManager::GetConcreteInstance();
	ui->ApplyCommand("/run/verbose 1");
	ui->ApplyCommand("/event/verbose 1");
	ui->ApplyCommand("/tracking/verbose 1");
	ui->ApplyCommand("/vis/open OGL");
	if(vis) {
		ui->ApplyCommand("/vis/drawVolume");
		ui->ApplyCommand("/vis/scene/add/trajectories smooth");
		ui->ApplyCommand("/vis/modeling/trajectories/create/drawByCharge");
		ui->ApplyCommand("/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true");
		ui->ApplyCommand("/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2");
		ui->ApplyCommand("/vis/viewer/flush");
	}
	runManager->BeamOn(1);
	G4UIExecutive* uiExec = new G4UIExecutive(argc,argv);
	uiExec->SessionStart();
	delete uiExec;
	delete runManager;
	delete visManager;
	return 0;
}