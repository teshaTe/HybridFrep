project(HFRep_interior_df)
cmake_minimum_required(VERSION 3.0)

if(BUILD_2D_HFREP_SDF)
    add_subdirectory(HFRep2D)
endif()

if(BUILD_3D_HFREP_SDF)
    add_subdirectory(HFRep3D)
endif()
