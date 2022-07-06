#------------------------------------------------------
#
# DobotLinkPro created by LiuYufei 2019-01-29T17:00:00
#
# DPluginInterface -- plugins' base class
# Plugins          -- user plugins
# DLMaintenance    -- plugins upgrade
#
# plugins:
# MagicDevicePlugin -- Magician(lite), MagicBox, M1
# IndustrialRobotPlugin -- CR5, IR
# ArduinoPlugin     -- AIStarter, MobilePlatform
#
#------------------------------------------------------

TEMPLATE = subdirs

HEADERS += \
    version.h

win32 {
SUBDIRS = \
    DobotLink \
#    DLMaintenance \
    test \
    Plugins/DPluginInterface \
    Plugins/IndustrialRobotPlugin \
    Plugins/MagicDevicePlugin \
    Plugins/ArduinoPlugin \
#    Plugins/MicrobitPlugin \
    Plugins/DownloadPlugin \
#    Plugins/DemoPlugin \
    Plugins/DynamicAlgorithmRPCPlugin \
    Plugins/DebuggerPlugin \
    Plugins/PyImageOMPlugin \
    Plugins/CodingAgentPlugin \
    Plugins/DebuggerlitePlugin \
    Plugins/MagicianProJoystickPlugin \
    Plugins/RunCmdPlugin \
}

macx {
SUBDIRS = \
    DobotLink \
    test \
    Plugins/DPluginInterface \
    Plugins/MagicDevicePlugin \
    Plugins/ArduinoPlugin \
    Plugins/DownloadPlugin \
    Plugins/PyImageOMPlugin \
    Plugins/DebuggerlitePlugin \
    Plugins/RunCmdPlugin
}

android {
SUBDIRS = \
    DobotLink \
    Plugins/DPluginInterface \
    Plugins/IndustrialRobotPlugin \
    LuaMobile
}

linux {
SUBDIRS = \
    DobotLink \
    Plugins/DPluginInterface \
    Plugins/MagicDevicePlugin \
    Plugins/PyImageOMPlugin \
#    Plugins/DownloadPlugin \
    Plugins/DebuggerlitePlugin \
    Plugins/RunCmdPlugin
}

edu-mac {
    SUBDIRS = \
        DobotLink \
        DLMaintenance \
        Plugins/DPluginInterface \
        Plugins/MagicDevicePlugin \
        Plugins/DownloadPlugin \
        Plugins/PyImageOMPlugin \
        Plugins/DebuggerlitePlugin \
        Plugins/RunCmdPlugin
}

edu-win {
    SUBDIRS = \
        DobotLink \
        DLMaintenance \
        Plugins/DPluginInterface \
        Plugins/MagicDevicePlugin \
        Plugins/ArduinoPlugin \
        Plugins/DownloadPlugin \
        Plugins/PyImageOMPlugin \
        Plugins/DebuggerlitePlugin \
        Plugins/RunCmdPlugin \
        Plugins/CodingAgentPlugin
}

coding-win {
    SUBDIRS = \
        DobotLink \
        DLMaintenance \
        Plugins/DPluginInterface \
        Plugins/MagicDevicePlugin \
        Plugins/ArduinoPlugin \
        Plugins/DownloadPlugin \
        Plugins/PyImageOMPlugin \
        Plugins/DebuggerlitePlugin \
        Plugins/RunCmdPlugin \
        Plugins/CodingAgentPlugin
}

#ind-mac {
#    SUBDIRS = \
#        DobotLink \
#        Plugins/DPluginInterface \
#        Plugins/IndustrialRobotPlugin \
#        Plugins/DebuggerPlugin \
#        Plugins/MagicianProJoystickPlugin \
#        Plugins/MagicDevicePlugin \
#        Plugins/DynamicAlgorithmRPCPlugin
#}

ind-win {
    SUBDIRS = \
        DobotLink \
        Plugins/DPluginInterface \
        Plugins/IndustrialRobotPlugin
}

ind-linux {
    SUBDIRS = \
        DobotLink \
        Plugins/DPluginInterface \
        Plugins/IndustrialRobotPlugin
}

RC_FILE=uac.rc
