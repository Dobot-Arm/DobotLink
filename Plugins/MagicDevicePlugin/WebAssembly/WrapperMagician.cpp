#include "WebBase.h"
#include "Marcs.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>

// std::string Magician_SearchDobot(bool comSync)
// {
//     // ![0]
//     FUNCTION_PREPARE();

//     // ![1]
//     FUNCTION_UNUSE_PARAMS();

//     // ![2]
//     FUNCTION_SEND(QString(__FUNCTION__), comSync);
// }

// std::string Magician_ConnectDobot(std::string portName, bool comSync)
// {
//     // ![0]
//     FUNCTION_PREPARE();

//     // ![1]
//     sendObj.insert("portName", QString::fromStdString(portName));

//     // ![2]
//     FUNCTION_SEND(QString(__FUNCTION__), comSync);
// }

// std::string Magician_DisconnectDobot(std::string portName, bool comSync)
// {
//     // ![0]
//     FUNCTION_PREPARE();

//     // ![1]
//     sendObj.insert("portName", QString::fromStdString(portName));

//     // ![2]
//     FUNCTION_SEND(QString(__FUNCTION__), comSync);
// }

// EMSCRIPTEN_BINDINGS(Magician)
// {
//     emscripten::function("Magician_SearchDobot", Magician_SearchDobot);
//     emscripten::function("Magician_ConnectDobot", Magician_ConnectDobot);
//     emscripten::function("Magician_DisconnectDobot", Magician_DisconnectDobot);
// }