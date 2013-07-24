pdes: Parallel & Distributed Simulation (discrete event)

This repository contains the software that my students and I are creating during our investigations of Time Warp 
synchronized parallel simulation.  The directory structure of this archive is:

warped: containing the source code and documentation for the warped simulation kernel.  This kernel is written in C++
  and has been designed so that most of the options can be configured in a runtime configuration file.  The kernel
  supports both sequential and parallel (time warp synchronized) simulations.  The time warp kernel is designed 
  primarily for efficient execution on a Beowulf cluster.  A threaded configuration is available that causes the
  kernel to spawn multiple threads on each node of the cluster.  The kernel supports numerous options within the
  time warp runtime space (e.g., periodic checkpointing; aggressive, lazy, or dynamic cancellation; optimistic fossil 
  collection; and so on).  Additional details on the software architecture and configuration options are available
  in the documentation subdirectory.

utils: contains a collection of utility functions used by the warped kernel.  This needs to be built before the warped
  subtree.
  
ns3towarped: (forthcoming) contains a llvm based tool to translate NS-3 network simulation models for execution on
  the warped kernel.  This is a source-to-source translator.  This translator is currently under development and not
  yet ready for external consumption/use.
  
questions, email me: wilseypa@gmail.com

thank you.
