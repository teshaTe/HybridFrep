project(HFRep_interior_df)
cmake_minimum_required(VERSION 3.0)

add_executable(diffMapTest diffMapsTest.cpp )
target_include_directories(diffMapTest PRIVATE ${Python_INCLUDE_DIRS} ${Python_NumPy_INCLUDE_DIRS})
target_link_libraries(diffMapTest hybridIDF)

add_executable(idfClassTest idfClassTest.cpp )
target_include_directories(idfClassTest PRIVATE ${Python_INCLUDE_DIRS} ${Python_NumPy_INCLUDE_DIRS})
target_link_libraries(idfClassTest hybridIDF)
