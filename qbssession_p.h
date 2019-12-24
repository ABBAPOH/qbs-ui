#ifndef QBSSESSION_P_H
#define QBSSESSION_P_H

#include "qbssession.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#include <string_view>

class Packet
{
    static constexpr std::string_view packetStart{"qbsmsg:", 7};
public:
    enum class Status { Incomplete, Complete, Invalid };
    Status parseInput(QByteArray &input)
    {
        if (m_expectedPayloadLength == -1) {
            const int packetStartOffset = input.indexOf(packetStart.data());
            if (packetStartOffset == -1)
                return Status::Incomplete;
            const int numberOffset = packetStartOffset + packetStart.length();
            const int newLineOffset = input.indexOf('\n', numberOffset);
            if (newLineOffset == -1)
                return Status::Incomplete;
            const QByteArray sizeString = input.mid(numberOffset, newLineOffset - numberOffset);
            bool isNumber;
            const int payloadLen = sizeString.toInt(&isNumber);
            if (!isNumber || payloadLen < 0)
                return Status::Invalid;
            m_expectedPayloadLength = payloadLen;
            input.remove(0, newLineOffset + 1);
        }
        const int bytesToAdd = m_expectedPayloadLength - m_payload.length();
        if (bytesToAdd < 0)
            return Status::Invalid;
        m_payload += input.left(bytesToAdd);
        input.remove(0, bytesToAdd);
        return isComplete() ? Status::Complete : Status::Incomplete;
    }

    QJsonObject retrievePacket()
    {
        if (!isComplete())
            return {};
        const auto packet = QJsonDocument::fromJson(QByteArray::fromBase64(m_payload)).object();
        m_payload.clear();
        m_expectedPayloadLength = -1;
        return packet;
    }

    static QByteArray createPacket(const QJsonObject &packet)
    {
        const QByteArray jsonData = QJsonDocument(packet).toJson().toBase64();
        return QByteArray(packetStart.data(), packetStart.size())
                .append(QByteArray::number(jsonData.length())).append('\n')
                .append(jsonData);
    }

private:
    bool isComplete() const { return m_payload.length() == m_expectedPayloadLength; }

    QByteArray m_payload;
    int m_expectedPayloadLength = -1;
};

class PacketReader : public QObject
{
    Q_OBJECT
public:
    explicit PacketReader(QObject *parent = nullptr) : QObject(parent) {}

    void handleData(const QByteArray &data)
    {
        m_incomingData += data;
        handleData();
    }

signals:
    void packetReceived(const QJsonObject &packet);
    void errorOccurred(const QString &msg);

private:
    void handleData()
    {
        switch (m_currentPacket.parseInput(m_incomingData)) {
        case Packet::Status::Invalid:
            emit errorOccurred(tr("Received invalid input."));
            break;
        case Packet::Status::Complete:
            emit packetReceived(m_currentPacket.retrievePacket());
            handleData();
            break;
        case Packet::Status::Incomplete:
            break;
        }
    }

    QByteArray m_incomingData;
    Packet m_currentPacket;
};


#endif // QBSSESSION_P_H
