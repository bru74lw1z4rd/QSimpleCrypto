/*
 * Copyright 2023 BrutalWizard (https://github.com/bru74lw1z4rd). All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution
 */

#include "include/QBlockCipher.h"

QSimpleCrypto::QBlockCipher::QBlockCipher()
{
}

///
/// \brief QSimpleCrypto::QBlockCipher::generateRandomSalt - Function generates salt (random bytes) by size.
/// \param size - Size of generated bytes.
/// \return Returns salt (random bytes).
///
QByteArray QSimpleCrypto::QBlockCipher::generateSalt(const quint16& size)
{
    unsigned char arr[sizeof(size)];
    RAND_bytes(arr, sizeof(size));

    return QByteArray(reinterpret_cast<char*>(arr), size);
}

///
/// \brief QSimpleCrypto::QBlockCipher::encryptAesBlockCipher - Function encrypts data with Aes Block Cipher algorithm.
/// \param data - Data that will be encrypted.
/// \param key - AES key. Example: "AABBCCEEFFGGHHKKLLMMNNOOPPRRSSTT"
/// \param iv - Initialization vector. Example: "AABBCCEEFFGGHHKKLLMMNNOOPPRRSSTT"
/// \param password - Encryption password.
/// \param salt - Random delta. Example: "qwerty123" or another random bytes generated with QSimpleCrypto::QBlockCipher::generateSalt.
/// \param rounds - Transformation rounds.
/// \param chiper - Can be used with OpenSSL EVP_CIPHER (ecb, cbc, cfb, ofb, ctr) - 128, 192, 256. Example: EVP_aes_256_cbc().
/// \param md - Hash algroitm (OpenSSL EVP_MD). Example: EVP_sha512().
/// \return Returns encrypted data on success or "" on failure.
///
QByteArray QSimpleCrypto::QBlockCipher::encryptAesBlockCipher(const QByteArray& data, const QByteArray& key, const QByteArray& iv,
    const QByteArray& password, const QByteArray& salt, const qint32 rounds,
    const EVP_CIPHER* cipher, const EVP_MD* md)
{
    try {
        /* Initialize EVP_CIPHER_CTX */
        std::unique_ptr<EVP_CIPHER_CTX, void (*)(EVP_CIPHER_CTX*)> encryptionCipher { EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free };
        if (!encryptionCipher) {
            throw std::runtime_error("Couldn't initialize \'encryptionCipher\'. EVP_CIPHER_CTX_new(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /* Reinterpret values for multi use */
        unsigned char* m_key = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(key.data()));
        unsigned char* m_iv = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(iv.data()));

        /* Set data length */
        int cipherTextLength(data.size() + AES_BLOCK_SIZE);
        int finalLength = 0;

        /* Initialize cipcherText. Here encrypted data will be stored */
        std::unique_ptr<unsigned char[]> cipherText { new unsigned char[cipherTextLength]() };
        if (cipherText == nullptr) {
            throw std::runtime_error("Couldn't allocate memory for 'cipherText'.");
        }

        /* Start encryption with password based encryption routine */
        if (!EVP_BytesToKey(cipher, md, reinterpret_cast<const unsigned char*>(salt.data()), reinterpret_cast<const unsigned char*>(password.data()), password.length(), rounds, m_key, m_iv)) {
            throw std::runtime_error("Couldn't start encryption routine. EVP_BytesToKey(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /* Initialize encryption operation. */
        if (!EVP_EncryptInit_ex(encryptionCipher.get(), cipher, nullptr, m_key, m_iv)) {
            throw std::runtime_error("Couldn't initialize encryption operation. EVP_EncryptInit_ex(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /*
         * Provide the message to be encrypted, and obtain the encrypted output.
         * EVP_EncryptUpdate can be called multiple times if necessary
         */
        if (!EVP_EncryptUpdate(encryptionCipher.get(), cipherText.get(), &cipherTextLength, reinterpret_cast<const unsigned char*>(data.data()), data.size())) {
            throw std::runtime_error("Couldn't provide message to be encrypted. EVP_EncryptUpdate(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /* Finalize the encryption. Normally ciphertext bytes may be written at this stage */
        if (!EVP_EncryptFinal(encryptionCipher.get(), cipherText.get() + cipherTextLength, &finalLength)) {
            throw std::runtime_error("Couldn't finalize encryption. EVP_EncryptFinal(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /* Finilize data to be readable with qt */
        return QByteArray(reinterpret_cast<char*>(cipherText.get()), cipherTextLength + finalLength);
    } catch (const std::exception& exception) {
        std::throw_with_nested(exception.what());
    } catch (...) {
        throw;
    }
}

///
/// \brief QSimpleCrypto::QBlockCipher::decryptAesBlockCipher - Function decrypts data with Aes Block Cipher algorithm.
/// \param data - Data that will be decrypted.
/// \param key - AES key. Example: "AABBCCEEFFGGHHKKLLMMNNOOPPRRSSTT"
/// \param iv - Initialization vector. Example: "AABBCCEEFFGGHHKKLLMMNNOOPPRRSSTT"
/// \param password - Decryption password.
/// \param salt - Random delta. Example: "qwerty123" or another random bytes generated with QSimpleCrypto::QBlockCipher::generateSalt.
/// \param rounds - Transformation rounds.
/// \param chiper - Can be used with OpenSSL EVP_CIPHER (ecb, cbc, cfb, ofb, ctr) - 128, 192, 256. Example: EVP_aes_256_cbc().
/// \param md - Hash algroitm (OpenSSL EVP_MD). Example: EVP_sha512().
/// \return Returns decrypted data on success or "" on failure.
///
QByteArray QSimpleCrypto::QBlockCipher::decryptAesBlockCipher(const QByteArray& data, const QByteArray& key, const QByteArray& iv,
    const QByteArray& password, const QByteArray& salt, const qint32 rounds,
    const EVP_CIPHER* cipher, const EVP_MD* md)
{
    try {
        /* Initialize EVP_CIPHER_CTX */
        std::unique_ptr<EVP_CIPHER_CTX, void (*)(EVP_CIPHER_CTX*)> decryptionCipher { EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free };
        if (!decryptionCipher) {
            throw std::runtime_error("Couldn't initialize \'decryptionCipher\'. EVP_CIPHER_CTX_new(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /* Reinterpret values for multi use */
        unsigned char* m_key = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(key.data()));
        unsigned char* m_iv = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(iv.data()));

        /* Set data length */
        int plainTextLength(data.size());
        int finalLength = 0;

        /* Initialize plainText. Here decrypted data will be stored */
        std::unique_ptr<unsigned char[]> plainText { new unsigned char[plainTextLength + AES_BLOCK_SIZE]() };
        if (plainText == nullptr) {
            throw std::runtime_error("Couldn't allocate memory for \'plainText\'. EVP_CIPHER_CTX_new(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /* Start encryption with password based encryption routine */
        if (!EVP_BytesToKey(cipher, md, reinterpret_cast<const unsigned char*>(salt.data()), reinterpret_cast<const unsigned char*>(password.data()), password.length(), rounds, m_key, m_iv)) {
            throw std::runtime_error("Couldn't start decryption routine. EVP_BytesToKey(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /* Initialize decryption operation. */
        if (!EVP_DecryptInit_ex(decryptionCipher.get(), cipher, nullptr, m_key, m_iv)) {
            throw std::runtime_error("Couldn't initialize decryption operation. EVP_DecryptInit_ex(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /*
         * Provide the message to be decrypted, and obtain the plaintext output.
         * EVP_DecryptUpdate can be called multiple times if necessary
         */
        if (!EVP_DecryptUpdate(decryptionCipher.get(), plainText.get(), &plainTextLength, reinterpret_cast<const unsigned char*>(data.data()), data.size())) {
            throw std::runtime_error("Couldn't provide message to be decrypted. EVP_DecryptUpdate(). Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /*
         * Finalize the decryption. A positive return value indicates success,
         * anything else is a failure - the plaintext is not trustworthy.
         */
        if (!EVP_DecryptFinal(decryptionCipher.get(), plainText.get() + plainTextLength, &finalLength)) {
            throw std::runtime_error("Couldn't finalize decryption. EVP_DecryptFinal. Error: " + QByteArray(ERR_error_string(ERR_get_error(), nullptr)));
        }

        /* Finilize data to be readable with qt */
        return QByteArray(reinterpret_cast<char*>(plainText.get()), plainTextLength + finalLength);
    } catch (const std::exception& exception) {
        std::throw_with_nested(exception.what());
    } catch (...) {
        throw;
    }
}
