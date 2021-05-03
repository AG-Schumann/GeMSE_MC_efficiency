# GeMSE_MC

Geant4 code to simulate the GeMSE detector efficiency

[![CodeFactor](https://www.codefactor.io/repository/github/ag-schumann/gemse_mc/badge)](https://www.codefactor.io/repository/github/ag-schumann/gemse_mc)

## Compilation

If you are sitting on top of the [GeMSE_environment](https://github.com/AG-Schumann/GeMSE_environment), run:
```
git clone https://github.com/AG-Schumann/GeMSE_MC.git
cd GeMSE_MC
source /opt/GeMSE/setup_sims.sh
make -j # "-j" for parallel compilation using all available CPU
```
and your executables will be built under `./bin/Linux-g++/GeMSE_MC`, where `.` is the default working directory (`$G4WORKDIR`).

If running locally, make sure you have ROOT and Geant4 (10.3 recommended) installed. Compilation errors will arise because of the nonexistent paths that the `CADMesh.hh` class needs to execute. You can either install these extra libraries (ask Diego) or just run the code without CADMesh, assuming you do not want to simulate a 3D sample. In order to do that:
+ remove `include/CADMesh.hh` files from its directory,
+ comment out the line `#include <CADMesh.hh>` from the top of the `src/GeMSE_DetectorConstruction.cc` file,
+ comment out these two lines from the `Makefile`
```yaml
EXTRALIBS +=/opt/GeMSE/assimp/bin/libassimp.so
EXTRALIBS +=/opt/GeMSE/tetgen/tetlib.so
```
and you're ready to go:
```
git clone https://github.com/AG-Schumann/GeMSE_MC.git
cd GeMSE_MC
export G4WORKDIR=${PWD} # optional, otherwise defaulted to /home/<username>/geant4workdir/
make -j # "-j" for parallel compilation using all available CPU
```

## Execution

Run the executable with:
```
$G4WORKDIR/bin/Linux-g++/GeMSE_MC -m <macrofile.mac> -o <results_folder>
```

### macrofile.mac
* Contains the information which isotopes or gamma lines are simulated
* See example, `macros/example_various_isotopes_G4103p3.mac`.
* For visualization only use `macros/visualization_vrml.mac` (no need for `-o` argument in this case).

### results_folder
* Folder where result files are written.

### Output
* `simulated_efficiencies.root`
	* TTree `tree`: Tree with information about detection efficiency and branching ratio (emission probability) for every gamma line
		* TBranch `energy`: energy of gamma line
		* TBranch `efficiency`: detection efficiency
        * TBranch `efficiency_err`: statistical uncertainty of detection efficiency
        * TBranch `eff_BR`: product of detection efficiency and branching ratio

## Import CAD samples

The [CADMesh](https://github.com/christopherpoole/CADMesh) interface, specifically [v2.0.3](https://github.com/christopherpoole/CADMesh/releases/tag/v2.0.3) is integrated into this code. This means that one can import CAD samples in STL, PLY or OBJ format into Geant4 for their construction. See an example in `src/GeMSE_DetectorConstruction.cc`, under `CAD SAMPLE`.

*Note*: Contrary to the CADMesh version used so far, present in GeMSE_MC release [1.2.0](https://github.com/AG-Schumann/GeMSE_MC/releases/tag/v1.2.0), this one does not support binary STL files. These can be converted to ASCII using commercial CAD software (see `sample_geometries/scans_3d/banana_LRT_talk_binary.stl`).
