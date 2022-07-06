#include "WebBase.h"
#include "Marcs.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <emscripten.h>
#include <QDebug>

void OnRecv(const qint64 &id, const QJsonValue &recvValue)
{                    
    QByteArray byteArray;
    if (recvValue.isBool()) {
        QString temp = recvValue.toBool() ? "true" : "false";
        byteArray = temp.toLatin1();
    } else {
        QJsonDocument document;
        if (recvValue.isArray()) {
            document.setArray(recvValue.toArray());
        } else if (recvValue.isObject()) {
            document.setObject(recvValue.toObject());
        }
        byteArray = document.toJson(QJsonDocument::Compact);
    }

    EM_ASM({
        if (Module._onRecv !== undefined) {
            Module._onRecv($0, $1, $2);
        }
    }, (int)id, byteArray.data(), byteArray.length());
}

void Common_RequestPorts()
{
    EM_ASM({
        navigator.serial.requestPort();
    });
}

void Common_Init() 
{
    static bool isInited(false);
    if (!isInited) {
        isInited = true;
    } else {
        return;
    }

    WebBase::instance()->setRecvCallback(OnRecv);

    EM_ASM({
        Module._onRecv = function(id, bytesPrt, len) {
            if (Module.requests === undefined || Module.requests[id] === undefined) return;

            const str = UTF8ToString(bytesPrt, len);
            const res = JSON.parse(str);
            const resolve = Module.requests[id];
            resolve(res);
            delete Module.requests[id];
        };

        Module.Common_Send = async function(method, params) {
            return new Promise((resolve) => {
                if (Module.requests === undefined) Module.requests = {};
                const id = Module._Common_Send(method, JSON.stringify(params));
                Module.requests[id] = resolve;
            });
        };
    });
}


int _Common_Send(std::string funcname, std::string sendStr)
{
    QJsonObject sendObj;
    QByteArray bytes(sendStr.c_str(), sendStr.length());
    QJsonDocument sendDoc = QJsonDocument::fromJson(bytes);
    sendObj = sendDoc.object();

    /*以前老的method写法是全称:dobotlink.MagicianLite.XXX，但是我看现在的代码，明明就是
    **MagicianLite.XXX的，不知道为啥。为了兼容老的全称写法，这里做个兼容
    */
    QString type;
    QString method;
    QStringList temp = QString::fromStdString(funcname).split(".");
    if (temp.size() == 2)
    {
        type = temp[0];
        method = temp[1];
    }
    else if (temp.size() == 3)
    {
        type = temp[1];
        method = temp[2];
    }
    return WebBase::instance()->send(type, method, sendObj);
}

EMSCRIPTEN_BINDINGS(Common)
{
    emscripten::function("Common_RequestPorts", Common_RequestPorts);
    emscripten::function("Common_Init", Common_Init);
    emscripten::function("_Common_Send", _Common_Send);
}
