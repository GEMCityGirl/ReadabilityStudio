# Generate the Globalization dashboard + local badges 

# L10n status inputs
set(G11N_STATUS_DIR ${CMAKE_SOURCE_DIR}/project-management/g11n/status)

# I18n status inputs
set(G11N_STATUS_I18N_DIR ${CMAKE_SOURCE_DIR}/project-management/g11n/status/i18n)

# Outputs
set(G11N_OUT_MD     ${CMAKE_SOURCE_DIR}/project-management/g11n/DASHBOARD.md)
set(G11N_OUT_BADGES ${CMAKE_SOURCE_DIR}/project-management/g11n/badges)

# Roll-up weights
set(G11N_WEIGHTS ui=10,docs=5,installer=1,art=1,license=1)

# Inputs that trigger regeneration (l10n + i18n + script)
set(G11N_INPUTS
  ${G11N_STATUS_DIR}/ui.json
  ${G11N_STATUS_DIR}/docs.json
  ${G11N_STATUS_DIR}/windows-installer.json
  ${G11N_STATUS_DIR}/art.json
  ${G11N_STATUS_DIR}/license.json
  ${G11N_STATUS_I18N_DIR}/ui.json
  ${G11N_STATUS_I18N_DIR}/docs.json
  ${G11N_STATUS_I18N_DIR}/windows-installer.json
  ${G11N_STATUS_I18N_DIR}/art.json
  ${G11N_STATUS_I18N_DIR}/license.json
  ${CMAKE_SOURCE_DIR}/cmake/modules/BuildG11NDashboard.cmake)

# Pretty project name
set(G11N_PROJECT_NAME ${readstudio_DESCRIPTION})

# Stamp so the badges dir participates in up-to-date checks
set(G11N_BADGES_STAMP ${G11N_OUT_BADGES}/.stamp)

add_custom_command(
  OUTPUT ${G11N_OUT_MD} ${G11N_BADGES_STAMP}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${G11N_OUT_BADGES}
  COMMAND ${CMAKE_COMMAND}
          -DROOT=${CMAKE_SOURCE_DIR}
          -DSTATUS_DIR=${G11N_STATUS_DIR}
          -DSTATUS_DIR_I18N=${G11N_STATUS_I18N_DIR}
          -DOUT_MD=${G11N_OUT_MD}
          -DOUT_BADGES=${G11N_OUT_BADGES}
          -DWEIGHTS=${G11N_WEIGHTS}
          -DPROJECT_NAME=${G11N_PROJECT_NAME}
          -P ${CMAKE_SOURCE_DIR}/cmake/modules/BuildG11NDashboard.cmake
  COMMAND ${CMAKE_COMMAND} -E touch ${G11N_BADGES_STAMP}
  DEPENDS ${G11N_INPUTS}
  COMMENT "Generating Globalization dashboard and badges"
  VERBATIM)

add_custom_target(g11n-dashboard ALL
  DEPENDS ${G11N_OUT_MD} ${G11N_BADGES_STAMP})

message(STATUS "[G11N] rules loaded → ${G11N_OUT_MD}")
