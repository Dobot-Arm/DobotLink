#ifndef MARCS_H
#define MARCS_H

#define FUNCTION_PREPARE() \
    QJsonObject sendObj;   \
    QJsonValue recvValue;

#define FUNCTION_UNUSE_PARAMS() \
    ;

#define FUNCTION_SEND(funcname, comSync)                            \
    QStringList temp;                                               \
    if (funcname.contains("_"))                                     \
    {                                                               \
        temp = funcname.split("_");                                 \
    }                                                               \
    else                                                            \
    {                                                               \
        temp = funcname.split(".");                                 \
    }                                                               \
    QString type = temp[0];                                         \
    QString method = temp[1];                                       \
    QJsonValue *pRecvValue = comSync ? &recvValue : nullptr;        \
    WebBase::instance()->send(type, method, sendObj, pRecvValue);   \
                                                                    \
    if (!comSync)                                                   \
        return "";                                                  \
                                                                    \
    if (recvValue.isBool())                                         \
    {                                                               \
        return recvValue.toBool() ? "true" : "false";               \
    }                                                               \
                                                                    \
    QJsonDocument document;                                         \
    if (recvValue.isArray())                                        \
    {                                                               \
        document.setArray(recvValue.toArray());                     \
    }                                                               \
    else if (recvValue.isObject())                                  \
    {                                                               \
        document.setObject(recvValue.toObject());                   \
    }                                                               \
    QByteArray byteArray = document.toJson(QJsonDocument::Compact); \
    std::string str(byteArray.constData(), byteArray.length());     \
    return str;
#endif // MARCS_H