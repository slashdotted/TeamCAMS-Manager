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

#include "mqttcrypto.h"
extern "C" {
#include "monocypher.h"
}
#include <QCryptographicHash>

QJsonObject MqttCrypto::encrypt(const QJsonObject &plain,
                                const QString &sessionId,
                                const QByteArray &sessionKey,
                                quint64 sequence) {
  const QByteArray clearData =
      QJsonDocument(plain).toJson(QJsonDocument::Compact);

  if (sessionKey.size() != 32) {
    qWarning() << "[MqttCrypto] Invalid session key size:" << sessionKey.size();

    return {};
  }

  if (sessionId.isEmpty()) {
    qWarning() << "[MqttCrypto] Empty session id";

    return {};
  }

  QByteArray nonceSeed;
  nonceSeed.append(sessionId.toUtf8());
  nonceSeed.append(QByteArray::number(sequence));

  const QByteArray nonceHash =
      QCryptographicHash::hash(nonceSeed, QCryptographicHash::Sha256);

  if (nonceHash.size() < 24) {
    qWarning() << "[MqttCrypto] Failed to generate nonce";

    return {};
  }

  std::array<uint8_t, 24> nonce;
  std::copy_n(reinterpret_cast<const uint8_t *>(nonceHash.constData()),
              nonce.size(), nonce.begin());

  std::array<uint8_t, 32> key;
  std::copy_n(reinterpret_cast<const uint8_t *>(sessionKey.constData()),
              key.size(), key.begin());

  QByteArray ad;
  ad.append(sessionId.toUtf8());
  ad.append(QByteArray::number(sequence));

  std::vector<uint8_t> cipherText(static_cast<size_t>(clearData.size()));

  std::array<uint8_t, 16> mac{};

  crypto_aead_lock(cipherText.data(), mac.data(), key.data(), nonce.data(),
                   reinterpret_cast<const uint8_t *>(ad.constData()),
                   static_cast<size_t>(ad.size()),
                   reinterpret_cast<const uint8_t *>(clearData.constData()),
                   static_cast<size_t>(clearData.size()));

  const QByteArray macBytes(reinterpret_cast<const char *>(mac.data()),
                            static_cast<qsizetype>(mac.size()));

  const QByteArray cipherBytes(
      reinterpret_cast<const char *>(cipherText.data()),
      static_cast<qsizetype>(cipherText.size()));

  QJsonObject envelope;

  envelope["sid"] = sessionId;
  envelope["seq"] = static_cast<qint64>(sequence);
  envelope["mac"] = QString::fromLatin1(macBytes.toBase64());
  envelope["payload"] = QString::fromLatin1(cipherBytes.toBase64());

  return envelope;
}

QJsonObject MqttCrypto::decrypt(const QJsonObject &envelope,
                                const QString &sessionId,
                                const QByteArray &sessionKey,
                                quint64 lastSequence,
                                quint64 *receivedSequence) {
  const auto sidValue = envelope.value("sid");

  const auto seqValue = envelope.value("seq");

  const auto macValue = envelope.value("mac");

  const auto payloadValue = envelope.value("payload");

  if (!sidValue.isString() || !seqValue.isDouble() || !macValue.isString() ||
      !payloadValue.isString()) {

    qDebug() << "[MqttCrypto] Invalid envelope:" << envelope;

    return {};
  }

  const QString sid = sidValue.toString();

  if (sid != sessionId) {
    qDebug() << "[MqttCrypto] Invalid session id, got" << sid << "expecting"
             << sessionId;

    return {};
  }

  const quint64 seq = static_cast<quint64>(seqValue.toVariant().toULongLong());

  if (seq <= lastSequence) {
    qDebug() << "[MqttCrypto] Replay detected, got" << seq << "expecting >"
             << lastSequence;

    return {};
  }

  QByteArray nonceSeed;
  nonceSeed.append(sid.toUtf8());
  nonceSeed.append(QByteArray::number(seq));

  const QByteArray nonce =
      QCryptographicHash::hash(nonceSeed, QCryptographicHash::Sha256).left(24);

  QByteArray ad;
  ad.append(sid.toUtf8());
  ad.append(QByteArray::number(seq));

  const QByteArray mac = QByteArray::fromBase64(macValue.toString().toUtf8());

  const QByteArray cipherText =
      QByteArray::fromBase64(payloadValue.toString().toUtf8());

  if (sessionKey.size() != 32) {
    qDebug() << "[MqttCrypto] Invalid session key size:" << sessionKey.size();

    return {};
  }

  if (nonce.size() != 24) {
    qDebug() << "[MqttCrypto] Invalid nonce size:" << nonce.size();

    return {};
  }

  if (mac.size() != 16) {
    qDebug() << "[MqttCrypto] Invalid MAC size:" << mac.size();

    return {};
  }

  QByteArray plainText(cipherText.size(), Qt::Uninitialized);

  const int result = crypto_aead_unlock(
      reinterpret_cast<uint8_t *>(plainText.data()),
      reinterpret_cast<const uint8_t *>(mac.constData()),
      reinterpret_cast<const uint8_t *>(sessionKey.constData()),
      reinterpret_cast<const uint8_t *>(nonce.constData()),
      reinterpret_cast<const uint8_t *>(ad.constData()),
      static_cast<size_t>(ad.size()),
      reinterpret_cast<const uint8_t *>(cipherText.constData()),
      static_cast<size_t>(cipherText.size()));

  if (result != 0) {
    qDebug() << "[MqttCrypto] crypto_aead_unlock failed:" << result;

    return {};
  }

  const QJsonDocument doc = QJsonDocument::fromJson(plainText);

  if (!doc.isObject()) {
    qDebug() << "[MqttCrypto] Decrypted content is not a JSON object";

    return {};
  }

  if (receivedSequence) {
    *receivedSequence = seq;
  }

  return doc.object();
}
