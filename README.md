# BtB-MCR

BtB-MCR is a repository dedicated to fast digital material reconstruction techniques. The repository will serve as a collection of already existing algorithms for digital microstructure reconstruction (MCR) while also serving as testing ground for novel implementations and novel algorithms, with the mission of increased robustness and faster computational speeds.

I will also include documentation and examples as more of this code becomes public. Everything that is currently included in this package has been peer-reviewed and published already. Novel material will be indicated.

## Table of Contents

1. [Requirements](#requirements)
2. [Compiling](#compiling)
3. [Main Capabilities](#main-capabilities)
4. [Authors](#authors)
5. [Documentation](#documentation)
6. [Publications](#publications)
7. [Upcoming Changes](#upcoming-changes)
8. [Acknowledgements](#acknowledgements)


## Requirements

Linux requires MPI, while the Windows distribution relies on OpenMP for parallel computing. Below is a general list of requirements:

- GNU C/C++ compiler prefered.
- C++ 17 or later
- CMake 3.10 or later

GPU support will come to Linux at some point in the future. There are currently no plans to support Mac users, the package focuses on HPC applications.

### GUI Requirements

A GUI is coming in the future. There are different requirements for compiling the GUI.

## Compiling

Use the CMake files for building the application.


## Main Capabilities

Current capabilities:

- CPU accelerated material reconstruction via correlation functions (Y-T algorithm).

## Authors

- Main developer: Andre Adam (The University of Kansas)
    - [ResearchGate](https://www.researchgate.net/profile/Andre-Adam-2)
    - [GoogleScholar](https://scholar.google.com/citations?hl=en&user=aP_rDkMAAAAJ)
    - [GitHub](https://github.com/adama-wzr)
- Advisor: Dr. Xianglin Li (Washingtion University in St. Louis)
    - [Website](https://xianglinli.wixsite.com/mysite)
    - [GoogleScholar](https://scholar.google.com/citations?user=8y0Vd8cAAAAJ&hl=en)
 
## Documentation

At this moment, no documentation exists beyond the publications.

## Publications

If any material here is useful to you, please consider citing one of the following articles.

**Yeong Torquato Algorithm:**
- Adam, A., Wang, F., & Li, X. (2022). Efficient reconstruction and validation of heterogeneous microstructures for energy applications. International Journal of Energy Research. https://doi.org/10.1002/er.8578

**Stochastic Reconstruction**
- Adam, A., Fang, H., & Li, X. (2024). Effective thermal conductivity estimation using a convolutional neural network and its application in topology optimization. Energy and AI, 15, 2666–5468. https://doi.org/10.1016/j.egyai.2023.100310
- Adam, A., Stallard, S.L., Fang, H. et al. A General Framework for Predicting Permeability in Porous Structures Using Convolutional Neural Networks with Error Estimation. Transp Porous Med 152, 100 (2025). https://doi.org/10.1007/s11242-025-02239-4

## Upcoming Changes

- (ongoing) Refactor like TPS, makes more sense for larger projects to have linked objects.
    - Might drop Windows support for a bit
- 2D and 3D YTA code, enabled for multi-phase reconstruction.
- Parallel programming with MPI (UNIX exclusive, no Windows).
- Stochastic reconstruction methods will be implemented to the main execution loop.

- A genConfig GUI will be added soon. I learned my lesson with larger projects, it is definitely necessary.

## Acknowledgements

This package is not sponsored or supported by any grants. The original methods were developed as part of other publications, and the publications have their acknowledgements.
