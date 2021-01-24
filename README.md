Hybrid Function Representation for Heterogeneous Objects  
(https://www.sciencedirect.com/science/article/abs/pii/S1524070321000035)  
Authors: A.Tereshin, A.Pasko, O.Fryazinov, V.Adzhiev  
The NCCA Bournemouth University  

Abstract  
Heterogeneous object modelling is an emerging area where geometric shapes are considered in concert with their internal physically-based attributes. This paper describes a novel theoretical and practical framework for modelling volumetric heterogeneous objects on the basis of a novel unifying functionally-based hybrid representation called HFRep. This new representation allows for obtaining a continuous smooth distance field in Euclidean space and preserves the advantages of the conventional representations based on scalar fields of different kinds without their drawbacks. We systematically describe the mathematical and algorithmic basics of HFRep. The steps of the basic algorithm are presented in detail for both geometry and attributes. To solve some problematic issues, we have suggested several practical solutions, including a new algorithm for solving the eikonal equation on hierarchical grids. Finally, we show the practicality of the approach by modelling several representative heterogeneous objects, including those of a time-variant nature.  


Description:  
This project is an implementation of Hybrid Function Representation for defining heterogeneous volumetric objects; This implementation covers HFRep based on FRep and SDF, FRep and IDF.  

Dependencies:  
Cmake  (https://www.cmake.org/)  
SFML   (https://www.sfml-dev.org/)  
OpenGL (https://www.opengl.org/)  
OpenMP (https://www.openmp.org/)  
Eigen3 (https://www.eigen.tuxfamily.org/)  
GLFW   (https://www.glfw.org/)  
GTS    (http://www.gts.sourceforge.net/)  
Python NumPy   (https://www.numpy.org/)  
matplotlib-cpp (https://www.github.com/lava/matplotlib-cpp)  

Custom builds of these libraries:  
libIgl (https://www.libigl.github.io/)  
dipLib (https://www.diplib.org/)  
Geometric tools (https://www.geometrictools.com/)  

are presented in the repositores;  

This library was tested under linux (only) and built up using Qt creator and Cmake.



