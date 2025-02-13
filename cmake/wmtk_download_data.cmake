function(wmtk_download_data)



     CPMAddPackage(
         NAME wmtk_data
         GIT_REPOSITORY ${WMTK_DATA_REPOSITORY}
         GIT_TAG ${WMTK_DATA_TAG}
         GIT_SHALLOW TRUE
         DOWNLOAD_ONLY TRUE
     )

     set(DATA_DIR "${wmtk_data_SOURCE_DIR}" PARENT_SCOPE)
     message(STATUS "${wmtk_data_SOURCE_DIR}======================")


endfunction()
