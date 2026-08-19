// TeamCAMS - reborn Cabin Air Management System
// Copyright (C) 2015-2026  Amos Brocco,
//                          Cognitive Ergonomics and Work Psychology Team,
//                          Psychology Department of Fribourg University,
//                          Switzerland / Department of Innovative Technologies
//                          University of Applied Sciences and Arts of Southern
//                          Switzerland, Contact: amos.brocco@supsi.ch
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include "mqtttransport.h"
#include <QAbstractSocket>
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winsock2.h>
#endif
#ifdef Q_OS_WIN
#include <mstcpip.h>
#else
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#endif

static constexpr quint8 MQTT_CONNECT = 1;
static constexpr quint8 MQTT_CONNACK = 2;
static constexpr quint8 MQTT_PUBLISH = 3;
static constexpr quint8 MQTT_PUBACK = 4;
static constexpr quint8 MQTT_SUBSCRIBE = 8;
static constexpr quint8 MQTT_SUBACK = 9;
static constexpr quint8 MQTT_UNSUBSCRIBE = 10;
static constexpr quint8 MQTT_UNSUBACK = 11;
static constexpr quint8 MQTT_PINGREQ = 12;
static constexpr quint8 MQTT_PINGRESP = 13;
static constexpr quint8 MQTT_DISCONNECT = 14;

static constexpr quint8 MQTT_CONNACK_ACCEPTED = 0;
static constexpr quint8 MQTT_CONNACK_REFUSED_BAD_PROTOCOL = 1;
static constexpr quint8 MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED = 2;
static constexpr quint8 MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE = 3;
static constexpr quint8 MQTT_CONNACK_REFUSED_BAD_CREDENTIALS = 4;
static constexpr quint8 MQTT_CONNACK_REFUSED_NOT_AUTHORIZED = 5;

static constexpr quint8 MQTT_SUBSCRIBE_FLAGS = 0x02;
static constexpr quint8 MQTT_UNSUBSCRIBE_FLAGS = 0x02;

static constexpr quint8 MQTT_CLEAN_START = 0x02;
static constexpr quint8 MQTT_USERNAME = 0x80;
static constexpr quint8 MQTT_PASSWORD = 0x40;

static constexpr quint32 MQTT_MAX_REMAINING_LENGTH = 0x0FFFFFFF;
static constexpr quint8 MQTT_PROTOCOL_3_1_1 = 0x04;
static constexpr quint16 MQTT_KEEPALIVE_SEC = 60;

static constexpr quint8 mqttHeader(quint8 packetType, quint8 flags = 0) {
  return static_cast<quint8>((packetType << 4) | flags);
}

static constexpr uint8_t msb(uint16_t value) {
  return static_cast<uint8_t>(value >> 8);
}

static constexpr uint8_t lsb(uint16_t value) {
  return static_cast<uint8_t>(value);
}

static void appendTo(QByteArray &buffer, quint16 value) {
  buffer.append(msb(value));
  buffer.append(lsb(value));
}

#if !defined(Q_OS_WASM)
// TODO: This might not be necessary once we target 6.11 thanks to
// KeepAliveIdleOption and KeepAliveIntervalOption and KeepAliveCountOption
// see https://doc.qt.io/qt-6/qabstractsocket.html#SocketOption-enum
void configureSocketKeepAlive(QTcpSocket *socket) {
  if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
    return;
  }

  qintptr nativeFd = socket->socketDescriptor(); //
  if (nativeFd == -1)
    return;

  const int idleSeconds = 10;
  const int intervalSeconds = 2;
  const int probeCount = 3;

  int enableKeepAlive = 1;
#ifdef Q_OS_WIN
  if (setsockopt(static_cast<SOCKET>(nativeFd), SOL_SOCKET, SO_KEEPALIVE,
                 reinterpret_cast<const char *>(&enableKeepAlive),
                 sizeof(enableKeepAlive)) != 0) {
#else
  if (setsockopt(static_cast<int>(nativeFd), SOL_SOCKET, SO_KEEPALIVE,
                 &enableKeepAlive, sizeof(enableKeepAlive)) != 0) {
#endif
    qDebug() << "[MqttTransport] failed to enable global SO_KEEPALIVE";
    return;
  }

#if defined(Q_OS_LINUX)
  int maxIdle = idleSeconds;
  int interval = intervalSeconds;
  int count = probeCount;

  setsockopt(static_cast<int>(nativeFd), IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle,
             sizeof(maxIdle)); //
  setsockopt(static_cast<int>(nativeFd), IPPROTO_TCP, TCP_KEEPINTVL, &interval,
             sizeof(interval)); //
  setsockopt(static_cast<int>(nativeFd), IPPROTO_TCP, TCP_KEEPCNT, &count,
             sizeof(count)); //
  qDebug() << "[MqttTransport] Linux Keep-Alive configured";

#elif defined(Q_OS_MAC) || defined(Q_OS_IOS)
  int maxIdle = idleSeconds;
  int interval = intervalSeconds;
  int count = probeCount;

  setsockopt(static_cast<int>(nativeFd), IPPROTO_TCP, TCP_KEEPALIVE, &maxIdle,
             sizeof(maxIdle));
  setsockopt(static_cast<int>(nativeFd), IPPROTO_TCP, TCP_KEEPINTVL, &interval,
             sizeof(interval));
  setsockopt(static_cast<int>(nativeFd), IPPROTO_TCP, TCP_KEEPCNT, &count,
             sizeof(count));
  qDebug() << "[MqttTransport] macOS Keep-Alive configured.";

#elif defined(Q_OS_WIN)
tcp_keepalive aliveConfig;
aliveConfig.onoff = 1;
aliveConfig.keepalivetime = idleSeconds * 1000;
aliveConfig.keepaliveinterval = intervalSeconds * 1000;

DWORD bytesReturned = 0;
WSAIoctl(static_cast<SOCKET>(nativeFd), SIO_KEEPALIVE_VALS, &aliveConfig,
         sizeof(aliveConfig), NULL, 0, &bytesReturned, NULL, NULL);
qDebug() << "[MqttTransport] Windows Keep-Alive configured";
#endif
}
#endif

struct MqttTransport::pimpl {
#if defined(Q_OS_WASM)
  QWebSocket *m_socket{nullptr};
#else
  QTcpSocket *m_socket{nullptr};
#endif
  MqttSecurityMode m_mode{MqttSecurityMode::Unsecure};
  QByteArray m_buffer;
  quint16 m_nextPacketId{1};
  QString m_managerUuid;
  QDateTime m_connected_time;
  QTimer m_keepAliveTimer;
  QDateTime m_lastTx;
  QDateTime m_lastRx;
  QSet<quint16> m_pendingSubscriptions;
  QSet<quint16> m_pendingUnsubscriptions;
  // Authenticated MQTT
  bool m_brokerRequiresAuth{false};
  QString m_brokerUsername;
  QString m_brokerPassword;
  QString m_host;
  quint16 m_port;
};

MqttTransport::MqttTransport(const QString &managerUuid, QObject *parent)
    : MessageTransport{parent}, m_pimpl{std::make_unique<pimpl>()} {
  qDebug() << "[MqttTransport] New instance for manager uuid:" << managerUuid;
  m_pimpl->m_managerUuid = managerUuid;
  m_pimpl->m_keepAliveTimer.setInterval(1000 * MQTT_KEEPALIVE_SEC / 2);
  m_pimpl->m_keepAliveTimer.start();

  connect(&m_pimpl->m_keepAliveTimer, &QTimer::timeout, this,
          &MqttTransport::checkKeepAlive);
}

void MqttTransport::checkKeepAlive() { sendPingReq(); }

MqttTransport::~MqttTransport() {
  if (m_pimpl->m_socket) {
    m_pimpl->m_socket->deleteLater();
  }
}

void MqttTransport::connectToBroker(const QString &host, quint16 port,
                                    MqttSecurityMode mode, bool requiresAuth,
                                    const QString &username,
                                    const QString &password) {
  m_pimpl->m_mode = mode;
  m_pimpl->m_brokerRequiresAuth = requiresAuth;
  m_pimpl->m_brokerUsername = username;
  m_pimpl->m_brokerPassword = password;
  m_pimpl->m_host = host;
  m_pimpl->m_port = port;

  if (m_pimpl->m_socket) {
    m_pimpl->m_socket->disconnect(this);
    m_pimpl->m_socket->deleteLater();
    m_pimpl->m_socket = nullptr;
  }

#if defined(Q_OS_WASM)
  m_pimpl->m_socket =
      new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this);
  connect(m_pimpl->m_socket, &QWebSocket::connected, this,
          &MqttTransport::onConnected);
  connect(m_pimpl->m_socket, &QWebSocket::disconnected, this,
          &MqttTransport::onDisconnected);
  connect(m_pimpl->m_socket, &QWebSocket::binaryMessageReceived, this,
          [this](const QByteArray &message) {
            m_pimpl->m_buffer.append(message);
            processBuffer();
          });
  connect(m_pimpl->m_socket, &QWebSocket::errorOccurred, this,
          [this](QAbstractSocket::SocketError error) {
            qWarning() << "[MqttTransport] Socket error:" << error;
            qWarning() << m_pimpl->m_socket->errorString();
            emit disconnected(m_pimpl->m_socket->errorString());
          });
  connect(m_pimpl->m_socket, &QWebSocket::stateChanged, this,
          [this, host, port](QAbstractSocket::SocketState state) {
            qDebug() << "[MqttTransport] State changed to" << state;
            emit stateChanged(state);
          });
#else
  if (m_pimpl->m_mode == MqttSecurityMode::Secure) {
    auto sslSocket = new QSslSocket(this);
    m_pimpl->m_socket = sslSocket;
    connect(m_pimpl->m_socket, &QTcpSocket::disconnected, this,
            &MqttTransport::onDisconnected);
    connect(sslSocket, &QSslSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError err) {
              auto sslSocketD = dynamic_cast<QSslSocket *>(m_pimpl->m_socket);
              if (sslSocketD) {
                qWarning() << "[MqttTransport] SSL Socket error:" << err
                           << sslSocketD->errorString();
                emit disconnected(m_pimpl->m_socket->errorString());
              }
            });
    connect(sslSocket, &QSslSocket::encrypted, this,
            &MqttTransport::onConnected);
    connect(sslSocket,
            qOverload<const QList<QSslError> &>(&QSslSocket::sslErrors), this,
            [](const QList<QSslError> &errors) {
              for (const auto &err : errors)
                qWarning() << "[MqttTransport] SSL error:" << err.errorString();
            });

    connect(m_pimpl->m_socket, &QTcpSocket::readyRead, this, [this]() {
      if (!m_pimpl->m_socket)
        return;
      auto data = m_pimpl->m_socket->readAll();
      m_pimpl->m_buffer.append(data);
      processBuffer();
    });
    connect(m_pimpl->m_socket, &QSslSocket::stateChanged, this,
            [this, host, port](QAbstractSocket::SocketState state) {
              emit stateChanged(state);
            });
  } else {
    m_pimpl->m_socket = new QTcpSocket(this);
    connect(m_pimpl->m_socket, &QTcpSocket::connected, this,
            &MqttTransport::onConnected);
    connect(m_pimpl->m_socket, &QTcpSocket::disconnected, this, [this]() {
      qWarning() << "[MqttTransport] Socked disconnected";
      emit disconnected(tr("SSL socket disconnected"));
    });
    connect(m_pimpl->m_socket, &QTcpSocket::readyRead, this, [this]() {
      if (!m_pimpl->m_socket)
        return;
      m_pimpl->m_buffer.append(m_pimpl->m_socket->readAll());
      processBuffer();
    });
    connect(m_pimpl->m_socket, &QTcpSocket::stateChanged, this,
            [this, host, port](QAbstractSocket::SocketState state) {
              emit stateChanged(state);
            });
    connect(m_pimpl->m_socket, &QSslSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError err) {
              qWarning() << "[MqttTransport] Socket error" << err
                         << m_pimpl->m_socket->errorString();
              emit disconnected(m_pimpl->m_socket->errorString());
            });
  }
#endif
  reconnect();
}

void MqttTransport::subscribe(const QString &topic) {
  if (!isConnected()) {
    return;
  }

  const quint16 packetId = m_pimpl->m_nextPacketId++;
  if (m_pimpl->m_nextPacketId == 0) {
    m_pimpl->m_nextPacketId = 1;
  }

  // Keep track of subscriptions requests
  m_pimpl->m_pendingSubscriptions.insert(packetId);

  qDebug() << "[MqttTransport] Subscribing to topic:" << topic
           << "(packetId =" << packetId << ")";

  QByteArray packet;
  packet.append(mqttHeader(MQTT_SUBSCRIBE, MQTT_SUBSCRIBE_FLAGS));

  // Construct payload
  QByteArray payload;
  appendTo(payload, packetId);
  auto topicUtf8 = topic.toUtf8();
  appendTo(payload, topicUtf8.size());
  payload.append(topicUtf8);
  payload.append(lsb(0x00)); // Options

  packet.append(encodeRemainingLength(payload.size()));
  packet.append(payload);

  writeRawData(packet);
}

void MqttTransport::unsubscribe(const QString &topic) {
  if (!isConnected()) {
    return;
  }

  const quint16 packetId = m_pimpl->m_nextPacketId++;
  if (m_pimpl->m_nextPacketId == 0) {
    m_pimpl->m_nextPacketId = 1;
  }

  // Keep track of unsubscription requests
  m_pimpl->m_pendingUnsubscriptions.insert(packetId);

  qDebug() << "[MqttTransport] Unsubscribing from topic:" << topic
           << "(packetId =" << packetId << ")";

  QByteArray packet;
  packet.append(mqttHeader(MQTT_UNSUBSCRIBE, MQTT_UNSUBSCRIBE_FLAGS));

  // Construct payload
  QByteArray payload;
  appendTo(payload, packetId);
  auto topicUtf8 = topic.toUtf8();
  appendTo(payload, topicUtf8.size());
  payload.append(topicUtf8);

  packet.append(encodeRemainingLength(payload.size()));
  packet.append(payload);

  writeRawData(packet);
}
void MqttTransport::publish(const QString &topic, const QByteArray &message) {
  qDebug() << "[MqttTransport] Publishing message to topic:" << topic;

  if (!isConnected())
    return;

  QByteArray packet;
  packet.append(mqttHeader(MQTT_PUBLISH));

  QByteArray variableHeader;
  auto topicUtf8 = topic.toUtf8();
  appendTo(variableHeader, topicUtf8.length());
  variableHeader.append(topicUtf8);

  QByteArray payload = variableHeader + message;
  packet.append(encodeRemainingLength(payload.length()));
  packet.append(payload);
  writeRawData(packet);
}

bool MqttTransport::isConnected() const {
  return m_pimpl->m_socket &&
         m_pimpl->m_socket->state() == QAbstractSocket::ConnectedState;
}

QAbstractSocket::SocketState MqttTransport::state() const {
  if (m_pimpl->m_socket)
    return m_pimpl->m_socket->state();
  else
    return QAbstractSocket::SocketState::UnconnectedState;
}

void MqttTransport::writeRawData(const QByteArray &data) {
  if (!m_pimpl->m_socket)
    return;
  m_pimpl->m_lastTx = QDateTime::currentDateTime();
#if defined(Q_OS_WASM)
  m_pimpl->m_socket->sendBinaryMessage(data);
#else
  m_pimpl->m_socket->write(data);
#endif
}

void MqttTransport::onConnected() {
#if !defined(Q_OS_WASM)
    qDebug() << "[MqttTransport] Setting socket keepalive option";
    configureSocketKeepAlive(m_pimpl->m_socket);
#endif

    qDebug() << "[MqttTransport] Connected to broker";

    m_pimpl->m_connected_time = QDateTime::currentDateTime();

    QByteArray packet;
    packet.append(mqttHeader(MQTT_CONNECT));

    QByteArray payload;
    // Protocol name (must be MQTT, 4 characters)
    payload.append(lsb(0x00));
    payload.append(lsb(0x04));
    payload.append("MQTT");
    payload.append(lsb(MQTT_PROTOCOL_3_1_1));

    quint8 connectFlags = MQTT_CLEAN_START;
    if (m_pimpl->m_brokerRequiresAuth) {
        connectFlags |= MQTT_USERNAME; // Username
        connectFlags |= MQTT_PASSWORD; // Password
    }
    payload.append(static_cast<char>(connectFlags));

    appendTo(payload, MQTT_KEEPALIVE_SEC);

    // Generate random client identifier
    const QString clientId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const QByteArray clientIdUtf8 = clientId.toUtf8();
    appendTo(payload, clientIdUtf8.size());
    payload.append(clientIdUtf8);

    if (m_pimpl->m_brokerRequiresAuth) {
        const QByteArray usernameUtf8 = m_pimpl->m_brokerUsername.toUtf8();
        appendTo(payload, usernameUtf8.size());
        payload.append(usernameUtf8);

        const QByteArray passwordUtf8 = m_pimpl->m_brokerPassword.toUtf8();
        appendTo(payload, passwordUtf8.size());
        payload.append(passwordUtf8);
    }

    packet.append(encodeRemainingLength(payload.size()));
    packet.append(payload);

    writeRawData(packet);
}

void MqttTransport::onDisconnected() {
  qDebug() << "[MqttTransport] Disconnected from broker after"
           << m_pimpl->m_connected_time.secsTo(QDateTime::currentDateTime())
           << "seconds ago";
  qDebug() << "[MqttTransport] Last received packet was"
           << m_pimpl->m_lastRx.secsTo(QDateTime::currentDateTime())
           << "seconds";
  emit disconnected(tr("Socket disconnected"));
}

void MqttTransport::reconnect() {

  qDebug() << "[MqttTransport] Connecting...";
  if (!m_pimpl->m_socket ||
      state() == QAbstractSocket::SocketState::ConnectedState) {
    return;
  }

#if defined(Q_OS_WASM)
  qDebug() << "[MqttTransport] Establishing"
           << ((m_pimpl->m_mode == MqttSecurityMode::Secure) ? "secure" : "unsecure")
           << "connection to" << m_pimpl->m_host << "on port" << m_pimpl->m_port;
  QString protocol =
      (m_pimpl->m_mode == MqttSecurityMode::Secure) ? "wss" : "ws";
  QUrl url(QString("%1://%2").arg(protocol, m_pimpl->m_host));
  url.setPort(m_pimpl->m_port);
  m_pimpl->m_socket->open(url);
#else
  if (m_pimpl->m_mode == MqttSecurityMode::Secure) {
    auto sslSocketD = dynamic_cast<QSslSocket *>(m_pimpl->m_socket);
    if (sslSocketD) {
      sslSocketD->connectToHostEncrypted(m_pimpl->m_host, m_pimpl->m_port);
    }
  } else {
    m_pimpl->m_socket->connectToHost(m_pimpl->m_host, m_pimpl->m_port);
  }
#endif
}

void MqttTransport::processBuffer() {
  m_pimpl->m_lastRx = QDateTime::currentDateTime();
  while (!m_pimpl->m_buffer.isEmpty()) {
    if (m_pimpl->m_buffer.size() < 2) {
      return;
    }
    // Fixed header
    const quint8 header = static_cast<quint8>(m_pimpl->m_buffer.at(0));
    const quint8 packetType = (header >> 4) & 0x0F;

    int multiplier = 1;
    int remainingLength = 0;
    int lenBytes = 0;
    bool remainingLengthComplete = false;
    for (int i = 1; i < m_pimpl->m_buffer.size() && i <= 4; ++i) {
      const quint8 encodedByte = static_cast<quint8>(m_pimpl->m_buffer.at(i));
      remainingLength += (encodedByte & 0x7F) * multiplier;
      multiplier *= 128;
      lenBytes = i;
      if ((encodedByte & 0x80) == 0) {
        remainingLengthComplete = true;
        break;
      }
    }
    if (!remainingLengthComplete) {
      if (m_pimpl->m_buffer.size() >= 5) {
        qWarning() << "[MqttTransport] processBuffer: Invalid Remaining Length";
        m_pimpl->m_buffer.clear();
      }
      return;
    }
    if (remainingLength < 0 || remainingLength > 268435455) {
      qWarning()
          << "[MqttTransport] processBuffer: Remaining Length out of range:"
          << remainingLength;

      m_pimpl->m_buffer.clear();
      return;
    }
    const int totalPacketLength = 1 + lenBytes + remainingLength;
    if (m_pimpl->m_buffer.size() < totalPacketLength) {
      return;
    }

    QByteArray packet = m_pimpl->m_buffer.left(totalPacketLength);
    m_pimpl->m_buffer.remove(0, totalPacketLength);
    const int payloadOffset = 1 + lenBytes;
    switch (packetType) {
    case MQTT_CONNACK: {
      if (packet.size() < payloadOffset + 2) {
        qWarning() << "[MqttTransport] processBuffer error, malformed CONNACK";
        break;
      }
      const quint8 returnCode =
          static_cast<quint8>(packet.at(payloadOffset + 1));
      switch (returnCode) {
      case MQTT_CONNACK_ACCEPTED:
        emit connected();
        break;
      case MQTT_CONNACK_REFUSED_BAD_PROTOCOL:
        qWarning() << "[MqttTransport] Connection failed: Unacceptable "
                      "protocol version";
        break;
      case MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED:
        qWarning() << "[MqttTransport] Connection failed: Identifier rejected";
        break;
      case MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE:
        qWarning() << "[MqttTransport] Connection failed: Server unavailable";
        break;
      case MQTT_CONNACK_REFUSED_BAD_CREDENTIALS:
        qWarning()
            << "[MqttTransport] Connection failed: Bad username or password";
        break;
      case MQTT_CONNACK_REFUSED_NOT_AUTHORIZED:
        qWarning() << "[MqttTransport] Connection failed: Not authorized";
        break;
      default:
        qWarning() << "[MqttTransport] Connection failed: Unknown CONNACK code"
                   << returnCode;
        break;
      }

      break;
    }
    case MQTT_SUBACK: {
      if (packet.size() < payloadOffset + 3) {
        qWarning() << "[MqttTransport] processBuffer error, malformed SUBACK";
        break;
      }
      const quint16 packetId =
          (static_cast<quint8>(packet.at(payloadOffset)) << 8) |
          static_cast<quint8>(packet.at(payloadOffset + 1));

      if (!m_pimpl->m_pendingSubscriptions.remove(packetId)) {
        qWarning() << "[MqttTransport] processBuffer error, unexpected SUBACK"
                   << packetId;
        break;
      }
      emit subscribed();
      break;
    }
    case MQTT_UNSUBACK: {
      if (packet.size() < payloadOffset + 2) {
        qWarning() << "[MqttTransport] processBuffer error, malformed UNSUBACK";
        break;
      }
      const quint16 packetId =
          (static_cast<quint8>(packet.at(payloadOffset)) << 8) |
          static_cast<quint8>(packet.at(payloadOffset + 1));
      if (!m_pimpl->m_pendingUnsubscriptions.remove(packetId)) {
        qWarning() << "[MqttTransport] processBuffer error, unexpected UNSUBACK"
                   << packetId;
        break;
      }
      emit unsubscribed();
      break;
    }
    case MQTT_PINGRESP: {
      break;
    }
    case MQTT_PUBLISH: {
      const int topicLengthOffset = payloadOffset;
      if (packet.size() < topicLengthOffset + 2) {
        qWarning() << "[MqttTransport] processBuffer error, malformed PUBLISH";
        break;
      }
      const int topicLen =
          (static_cast<quint8>(packet.at(topicLengthOffset)) << 8) |
          static_cast<quint8>(packet.at(topicLengthOffset + 1));
      const int topicOffset = topicLengthOffset + 2;
      if (topicLen <= 0 || packet.size() < topicOffset + topicLen) {
        qWarning()
            << "[MqttTransport] processBuffer error, invalid topic length";
        break;
      }
      const QString topic =
          QString::fromUtf8(packet.constData() + topicOffset, topicLen);
      const QByteArray message = packet.mid(topicOffset + topicLen);
      emit messageReceived(message, topic);
      break;
    }
    default: {
      qWarning()
          << "[MqttTransport] processBuffer error, unsupported packet type"
          << packetType;
      break;
    }
    }
  }
}

QByteArray MqttTransport::encodeRemainingLength(int length) {
  QByteArray encoded;

  if (length < 0 || length > MQTT_MAX_REMAINING_LENGTH) {
    qWarning() << "[MQTT TX] invalid remaining length:" << length;
    return encoded;
  }

  do {
    quint8 encodedByte = static_cast<quint8>(length % 128);
    length /= 128;
    if (length > 0) {
      encodedByte |= 0x80;
    }
    encoded.append(static_cast<char>(encodedByte));
  } while (length > 0);

  return encoded;
}

QString MqttTransport::transportId() const { return m_pimpl->m_managerUuid; }

void MqttTransport::sendPingReq() {
  if (!isConnected()) {
    return;
  }

  const auto now = QDateTime::currentDateTime();
  if (!m_pimpl->m_lastTx.isValid()) {
    m_pimpl->m_lastTx = now;
  }

  if (m_pimpl->m_lastTx.secsTo(now) > (MQTT_KEEPALIVE_SEC / 4)) {
    QByteArray packet;
    packet.append(mqttHeader(MQTT_PINGREQ));
    packet.append(encodeRemainingLength(0));
    writeRawData(packet);
    m_pimpl->m_lastTx = now;
  }
}
