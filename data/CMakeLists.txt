
FILE(GLOB ICON "dynamind-mine-icon.png")
INSTALL(FILES ${ICON} DESTINATION "/usr/share/pixmaps/")
INSTALL(FILES ${ICON} DESTINATION "/usr/share/icons/crystalsvg/128x128/mimetypes/")

FILE(GLOB ICONXPM "images/dynamind-icon.xpm")
INSTALL(FILES ${ICONXPM} DESTINATION "/usr/share/pixmaps/")

FILE(GLOB DESKTOP "dynamind.desktop")
INSTALL(FILES ${DESKTOP} DESTINATION "/usr/share/applications")
