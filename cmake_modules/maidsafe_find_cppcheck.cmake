#==============================================================================#
#                                                                              #
#  Copyright (c) 2011 maidsafe.net limited                                     #
#  All rights reserved.                                                        #
#                                                                              #
#  Redistribution and use in source and binary forms, with or without          #
#  modification, are permitted provided that the following conditions are met: #
#                                                                              #
#      * Redistributions of source code must retain the above copyright        #
#        notice, this list of conditions and the following disclaimer.         #
#      * Redistributions in binary form must reproduce the above copyright     #
#        notice, this list of conditions and the following disclaimer in the   #
#        documentation and/or other materials provided with the distribution.  #
#      * Neither the name of the maidsafe.net limited nor the names of its     #
#        contributors may be used to endorse or promote products derived from  #
#        this software without specific prior written permission.              #
#                                                                              #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  #
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         #
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        #
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    #
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     #
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     #
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  #
#  POSSIBILITY OF SUCH DAMAGE.                                                 #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Written by maidsafe.net team                                                #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Module used to locate Cppcheck.                                             #
#                                                                              #
#  Settable variables to aid with finding Cppcheck                             #
#    CPPCHECK_ROOT_DIR                                                         #
#                                                                              #
#  Variables set and cached by this module are:                                #
#    Cppcheck_EXECUTABLE                                                       #
#                                                                              #
#==============================================================================#


UNSET(Cppcheck_EXECUTABLE CACHE)

IF(CPPCHECK_ROOT_DIR)
  SET(CPPCHECK_ROOT_DIR ${CPPCHECK_ROOT_DIR} CACHE PATH "Path to Cppcheck directory" FORCE)
ELSE()
  SET(CPPCHECK_ROOT_DIR
        "C:/Program Files/Cppcheck"
        "D:/Program Files/Cppcheck"
        "E:/Program Files/Cppcheck"
        "C:/Program Files (x86)/Cppcheck"
        "D:/Program Files (x86)/Cppcheck"
        "E:/Program Files (x86)/Cppcheck"
        usr
        usr/local)
ENDIF()

FIND_PROGRAM(Cppcheck_EXECUTABLE NAMES cppcheck PATHS ${CPPCHECK_ROOT_DIR} PATH_SUFFIXES ${CPPCHECK_PATH_SUFFIXES})

IF(NOT Cppcheck_EXECUTABLE)
  IF(WIN32)
    MESSAGE("-- Didn't find Cppcheck. Specify path to Cppcheck root as e.g. -DCPPCHECK_ROOT_DIR=\"C:/Program Files/Cppcheck\"")
  ELSE()
    MESSAGE("-- Didn't find Cppcheck. Specify path to Cppcheck root as e.g. -DCPPCHECK_ROOT_DIR=/usr/local/bin")
  ENDIF()
ELSE()
  MESSAGE("-- Found Cppcheck - static code analysis enabled.")
ENDIF()
