# CHANGELOG

Historical detailed commit information of this repository **getting2knowDace**.

### Abbreviations

- TBP: Two Body Problem
- FTM: Free Torque Motion

## v1.0

### Major upgrades
- TBD

### Commit detail (chronological)
- 61c075f0 (2023-04-04): 
  - Generalized the standard deviation set up in _Deltas_ class: standard deviation (_stddev_) has 
  to be set for every state variable.
  - The enhancement above allows realistic simulations (i.e., FTM problem), taken some initial
  conditions from paper: _Francesco Cavanego et al., "On-board spacecraft relative pose estimation with high-order 
  extended Kalman filter"_ (https://doi.org/10.1016/j.actaastro.2018.11.020) that can be found in Tables 3, 4 and 5.
- c6e794e8 (2023-04-04): 
  - Integrator RK78 was only able for TBP, generalized to _N_ number of state variables. It can
  be used in FTM problem as well.
  - Added verbosity in RK78. Updated state at each integration step is dumped into terminal.