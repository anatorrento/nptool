add_library(NPSSSSD SHARED  SSSD.cc)
target_link_libraries(NPSSSSD NPSCore ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} ${NPLib_LIBRARIES} -lNPSSSD)
